#include "bevtree.h"
#include "spdlog/spdlog.h"





namespace bevtree 
{
    BevTreeManager::BevTreeManager()
    {
        registerNode<bevtree::Selector>("selector");
        registerNode<bevtree::Sequence>("sequence");
        registerNode<bevtree::Parallel>("parallel");

        registerNode<bevtree::RandomSelector>("random_selector");
        registerNode<bevtree::StatefulSelector>("stateful_selector");
        registerNode<bevtree::StatefulSequence>("stateful_sequence");

        registerNode<bevtree::Succeeder>("succeeder");
        registerNode<bevtree::Failer>("failer");
        registerNode<bevtree::Inverter>("inverter");
        registerNode<bevtree::Repeater>("repeater");

        registerNode<bevtree::UntilSuccess>("until_success");
        registerNode<bevtree::UntilFailure>("until_failure");
    }

    BevTreeManager::~BevTreeManager()
    {
    }

    bool BevTreeManager::load(const std::filesystem::path& cfgdir)
    {
        using namespace tinyxml2;


        for (const auto& entry : std::filesystem::directory_iterator(cfgdir))
        {
            if (entry.is_regular_file())
            {
                auto filename = entry.path();
                auto xmlDoc = std::make_shared<XMLDocument>();
                XMLError error = xmlDoc->LoadFile(filename.string().c_str());
                if (error != XML_SUCCESS)
                {
                    SPDLOG_ERROR("load bevtree file({}) failed.", filename.string());
                    continue;
                }

                _xmlDocs[filename.filename().string()] = xmlDoc;

                auto root = xmlDoc->RootElement();
                auto bevnode = root->FirstChildElement();
                while (bevnode) {
                    auto name = bevnode->Attribute("name");
                    _xmlNodes[name] = bevnode;

                    bevnode = bevnode->NextSiblingElement();
                }
            }
        }

        return true;
    }

    BevNodePtr BevTreeManager::createNode(const std::string& name)
    {
        auto creator = getNodeCreator(name);
        if (!creator) return nullptr;
        return creator->create();
    }

    NodeCreator::Ptr BevTreeManager::getNodeCreator(const std::string& name)
    {
        auto it = _creators.find(name);
        if (it == _creators.end()) return nullptr;
        return it->second;
    }


    BehaviorTree::Ptr BevTreeManager::createBevTree(const std::string& name)
    {
        auto it = _xmlNodes.find(name);
        if(it == _xmlNodes.end()) return nullptr;
        auto xmlNode = it->second;

        auto bevtree = std::make_shared<BehaviorTree>(name);
        bool res = bevtree->load(xmlNode);
        if (!res)
        {
            SPDLOG_ERROR("load bevtree ({}) failed.", name);
            return nullptr;
        }

        return bevtree;
    }

    //////////////////////////////////////////////////////////////////////////////////////////


    BevNode* BehaviorTree::getNode(const std::string& name)
    {
        auto it = _namedNodes.find(name);
        if(it == _namedNodes.end()) return nullptr;
        return it->second.get();
    }

    bool BehaviorTree::load(const XmlNode* btnode)
    {
        assert(btnode);

        _name = btnode->Attribute("name");

        auto rootnode = btnode->FirstChildElement();
        if (!rootnode) {
            SPDLOG_ERROR("bevtree must have a root node");
            return false; 
        }

        auto root = loadNode(rootnode, shared_from_this());
        if(!root) {
            return false;
        }

        setRoot(root);

        return true;
    }

    BevNodePtr BehaviorTree::loadNode(const XmlNode* xmlnode, BevNodePtr parent)
    {
        //auto type = std::string(xmlnode->name(), xmlnode->name_size());
        auto type = xmlnode->Name();

        auto node = BevTreeManager::inst().createNode(type);
        if (!node) {
            SPDLOG_ERROR("create node '{}' failed", type);
            return nullptr;
        }

        if(parent) {
            node->load(xmlnode);
            node->setBlackboard(parent->getBlackboard());
            parent->addChild(node);
        }

        std::string name = xmlnode->Attribute("name");
        if (!name.empty())
        {
            _namedNodes[name] = node;
        }
        
        auto xmlchild = xmlnode->FirstChildElement();
        while (xmlchild) {
            loadNode(xmlchild, node);
            xmlchild = xmlchild->NextSiblingElement();
        }

        return node;
    }


    BevNode::Status RandomSelector::update()
    {
        assert(hasChildren() && "Composite has no children");

        it = children.begin() + rand() % children.size();
        return (*it)->tick();
    }
   
    bool Repeater::load(const XmlNode* node)
    {
        try 
        {
            int count = node->IntAttribute("count", 0);
            setLimit(count);
        } 
        catch (std::exception& e ) 
        {
            SPDLOG_WARN("load repeator count failed. err = {}", e.what());
        }

        return true;
    }


    bool Parallel::load(const XmlNode* node)
    {
        try
        {
            failOnAll = node->BoolAttribute("fail_on_all", true);
            successOnAll = node->BoolAttribute("success_on_all", true);
        }
        catch (std::exception& e)
        {
            SPDLOG_WARN("load repeator count failed. err = {}", e.what());
        }

        return true;
    }





}