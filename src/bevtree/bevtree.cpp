#include "bevtree.h"
#include "spdlog/spdlog.h"





namespace bevtree 
{
    BevTreeManager::BevTreeManager()
    {
        registerNode<bevtree::Selector>("selector");
        registerNode<bevtree::Sequence>("sequence");
        registerNode<bevtree::Parallel>("parallel");

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

    bool BevTreeManager::load(const std::filesystem::path& filepath)
    {
        try 
        {
            _xmlFile = std::make_unique<rapidxml::file<>>(filepath.string().c_str());
            _xmlDoc = std::make_unique<rapidxml::xml_document<>>();
            _xmlDoc->parse<0>(_xmlFile->data());
  
            auto root = _xmlDoc->first_node();
            if(!root) 
            {
                spdlog::error("root node not found.");
                return false;
            }

            auto bevnode = root->first_node();
            while(bevnode) 
            {
                auto nameattr = bevnode->first_attribute("name");
                if(!nameattr) 
                {
                    spdlog::error("root node name NOT found.");
                    return false;
                }

                auto name = std::string(nameattr->value(), nameattr->value_size());
                _xmlNodes[name] = bevnode;

                bevnode = bevnode->next_sibling();
            }
        }
        catch (std::exception& e) 
        {
            spdlog::error("load file ({}) failed. err = {}", filepath.string(), e.what());
            return false;
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
            spdlog::error("load bevtree ({}) failed.", name);
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

        auto nameattr = btnode->first_attribute("name");
        if(!nameattr) {
            spdlog::info("name not found.");
            return false;
        }

        _name = std::string(nameattr->value(), nameattr->value_size());

        auto rootnode = btnode->first_node();
        if (!rootnode) {
            spdlog::error("bevtree must have a root node");
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
        auto type = std::string(xmlnode->name(), xmlnode->name_size());

        auto node = BevTreeManager::inst().createNode(type);
        if (!node) {
            spdlog::error("create node '{}' failed", type);
            return nullptr;
        }

        if(parent) {
            node->load(xmlnode);
            node->setBlackboard(parent->getBlackboard());
            parent->addChild(node);
        }

        auto nameattr = xmlnode->first_attribute("name");
        if(nameattr) {
            auto name = std::string(nameattr->value(), nameattr->value_size());
            _namedNodes[name] = node;
        }

        auto xmlchild = xmlnode->first_node();
        while (xmlchild) {
            loadNode(xmlchild, node);
            xmlchild = xmlchild->next_sibling();
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
            auto countattr = node->first_attribute("count");
            if(countattr)
            {
                int count = std::stoi(countattr->value());
                setLimit(count);
            }
        } 
        catch (std::exception& e ) 
        {
            spdlog::warn("load repeator count failed. err = {}", e.what());
        }

        return true;
    }







}