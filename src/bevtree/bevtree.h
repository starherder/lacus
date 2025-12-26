#pragma once

#include "braintree/brain_tree.h"
#include "utility/i_singleton.h"

#define RegisterBehaviorNode(Name, NodeType)\
class Behavior##NodeType##Register			\
{										    \
public:									    \
	Behavior##NodeType##Register()			\
	{									    \
        BevTreeManager::inst().registerNode<NodeType>(Name);\
	}									    \
}g_Behavior##node##RegisterIniter;


namespace bevtree 
{
    using XmlNode = BrainTree::XmlNode;   
    using BevNodePtr = std::shared_ptr<BrainTree::Node>; 

    class NodeCreator
    {
    public:
        using Ptr = std::shared_ptr<NodeCreator>;
        virtual BevNodePtr create(BevNodePtr parent) = 0;
    };

    template<typename NodeType>
    class BevNodeCreator : public NodeCreator
    {
    public:
        BevNodePtr create(BevNodePtr parent) {
            auto node = std::make_shared<NodeType>();
            return node;
        }
    };

    class BevTreeManager : public utility::ISingleton<BevTreeManager>
    {
    public:
        BevTreeManager();
        ~BevTreeManager();

        template<typename NodeType>
        void registerNode(const std::string_view& name) {
            _creators[name] = std::make_shared<BevNodeCreator<NodeType>>();
        }

        BevNodePtr createNode(const std::string_view& name, BevNodePtr parent = nullptr);

    private:
        NodeCreator::Ptr getNodeCreator(const std::string_view& name);

    private:

        std::unordered_map<std::string_view, NodeCreator::Ptr> _creators;
    };


    // --------------------bevtree-------------------------
    class BehaviorTree : public BrainTree::BehaviorTree
    {
    public:
        bool load(const XmlNode* root) override;

    private:
        BevNodePtr loadNode(const XmlNode* node, BevNodePtr parent);

    private:
        std::string_view _name;
    };

    
    // --------------------composite-------------------------

    class Selector : public BrainTree::Selector
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class Sequence : public BrainTree::Sequence
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class StatefulSelector : public BrainTree::StatefulSelector
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class StatefulSequence : public BrainTree::StatefulSequence
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class ParallelSequence : public BrainTree::ParallelSequence
    {
    public:
        bool load(const XmlNode* node) override;
    };

    // --------------------decorator-------------------------

    class Succeeder : public BrainTree::Succeeder
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class Failer : public BrainTree::Failer
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class Inverter : public BrainTree::Inverter
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class Repeater : public BrainTree::Repeater
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class UntilSuccess : public BrainTree::UntilSuccess
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class UntilFailure : public BrainTree::UntilFailure
    {
    public:
        bool load(const XmlNode* node) override;
    };








}