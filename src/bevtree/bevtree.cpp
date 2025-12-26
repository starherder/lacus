#include "bevtree.h"
#include "rapidxml/rapidxml.hpp"
#include "spdlog/spdlog.h"

namespace bevtree 
{


    BevTreeManager::BevTreeManager()
    {
        registerNode<Selector>("Selector");
        registerNode<Sequence>("Sequence");
        registerNode<StatefulSelector>("StatefulSelector");
        registerNode<StatefulSequence>("StatefulSequence");
        registerNode<ParallelSequence>("ParallelSequence");

        registerNode<Succeeder>("Succeeder");
        registerNode<Inverter>("Inverter");
        registerNode<Repeater>("Repeater");
        registerNode<UntilSuccess>("UntilSuccess");
        registerNode<UntilFailure>("UntilFailure");
    }

    BevTreeManager::~BevTreeManager()
    {
    }

    BevNodePtr BevTreeManager::createNode(const std::string_view& name, BevNodePtr parent)
    {
        auto creator = getNodeCreator(name);
        if (!creator) return nullptr;
        return creator->create(parent);
    }

    NodeCreator::Ptr BevTreeManager::getNodeCreator(const std::string_view& name)
    {
        auto it = _creators.find(name);
        if (it == _creators.end()) return nullptr;
        return it->second;
    }

    //////////////////////////////////////////////////////////////////////////////////////////



    bool BehaviorTree::load(const XmlNode* btnode)
    {
        assert(btnode);
        _name = std::string_view(btnode->name(), btnode->name_size());

        auto rootnode = btnode->first_node();
        if (!rootnode) {
            spdlog::error("bevtree must have a root node");
            return false; 
        }

        auto root = loadNode(rootnode, nullptr);
        setRoot(root);

        return true;
    }

    BevNodePtr BehaviorTree::loadNode(const XmlNode* xmlnode, BevNodePtr parent)
    {
        auto name = std::string_view(xmlnode->name(), xmlnode->name_size());
        
        auto node = BevTreeManager::inst().createNode(name);
        if (!node) {
            spdlog::error("create node '{}' failed", name);
            return nullptr;
        }

        auto xmlchild = xmlnode->first_node();
        while (xmlchild) {

            loadNode(xmlchild, node);
            xmlchild = xmlchild->next_sibling();
        }

        if(parent) {
            parent->addChild(node);
        }

        return node;
    }


    bool Selector::load(const XmlNode* node)
    {
        return true;
    }


    bool Sequence::load(const XmlNode* node)
    {
        return true;
    }

    bool StatefulSelector::load(const XmlNode* node)
    {
        return true;
    }

    bool StatefulSequence::load(const XmlNode* node)
    {
        return true;
    }

    bool ParallelSequence::load(const XmlNode* node)
    {
        return true;
    }

    bool Succeeder::load(const XmlNode* node)
    {
        return true;
    }

    bool Failer::load(const XmlNode* node)
    {
        return true;
    }

    bool Inverter::load(const XmlNode* node)
    {
        return true;
    }

    bool Repeater::load(const XmlNode* node)
    {
        return true;
    }

    bool UntilSuccess::load(const XmlNode* node)
    {
        return true;
    }

    bool UntilFailure::load(const XmlNode* node)
    {
        return true;
    }







}