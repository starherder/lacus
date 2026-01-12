#pragma once

#include "braintree/brain_tree.h"
#include "utility/i_singleton.h"

#include <filesystem>

#define RegisterBehaviorNode(Name, NodeType)\
class Behavior##NodeType##Register			\
{										    \
public:									    \
	Behavior##NodeType##Register()			\
	{									    \
        bevtree::BevTreeManager::inst().registerNode<NodeType>(Name);\
	}									    \
}g_Behavior##NodeType##RegisterIniter;


namespace bevtree 
{
    using XmlNode = BrainTree::XmlNode;   
    using Blackboard = BrainTree::Blackboard;
    using Status = BrainTree::Node::Status;
    using BevNode = BrainTree::Node;
    using BevNodePtr = std::shared_ptr<BrainTree::Node>; 

    // --------------------bevtree-------------------------
    class BehaviorTree : public BrainTree::BehaviorTree, 
                         public std::enable_shared_from_this<BehaviorTree>
    {
    public:
        using Ptr = std::shared_ptr<BehaviorTree>;

        BehaviorTree(const std::string& name) : _name(name) { }

        ~BehaviorTree() {}

        bool load(const XmlNode* root) override;

        auto& name() { return _name; }

        BevNode* getNode(const std::string& name);

    private:
        BevNodePtr loadNode(const XmlNode* node, BevNodePtr parent);

    private:
        std::string _name;
        std::unordered_map<std::string, BevNodePtr> _namedNodes;
    };


    // --------------------bev creator-------------------------
    class NodeCreator
    {
    public:
        using Ptr = std::shared_ptr<NodeCreator>;
        virtual BevNodePtr create() = 0;
    };

    template<typename NodeType>
    class BevNodeCreator : public NodeCreator
    {
    public:
        BevNodePtr create() 
        {
            return std::make_shared<NodeType>();
        }
    };

    // --------------------bev manager-------------------------
    class BevTreeManager : public utility::ISingleton<BevTreeManager>
    {
        friend class BehaviorTree;

    public:
        BevTreeManager();

        ~BevTreeManager();

        bool load(const std::filesystem::path& cfgdir);

        BehaviorTree::Ptr createBevTree(const std::string& name);

        template<typename NodeType>
        void registerNode(const std::string& name);

    private:
        BevNodePtr createNode(const std::string& name);

        NodeCreator::Ptr getNodeCreator(const std::string& name);

    private:
        std::unordered_map<std::string, std::shared_ptr<tinyxml2::XMLDocument>> _xmlDocs;

        std::unordered_map<std::string, XmlNode*> _xmlNodes;

        std::unordered_map<std::string, NodeCreator::Ptr> _creators;
    };

    template<typename NodeType>
    void BevTreeManager::registerNode(const std::string& name)
    {
        _creators[name.data()] = std::make_shared<BevNodeCreator<NodeType>>();
    }

    // --------------------compositor-------------------------

    class Selector : public BrainTree::Selector
    {};

    class Sequence : public BrainTree::Sequence
    {};

    class Parallel : public BrainTree::ParallelSequence
    {
    public:
        bool load(const XmlNode* node) override;
    };

    class StatefulSelector : public BrainTree::StatefulSelector
    {};

    class StatefulSequence : public BrainTree::StatefulSequence
    {};

    class RandomSelector : public BrainTree::Composite 
    {
    public:
        Status update() override;
    };


    // --------------------decorator-------------------------

    class Succeeder : public BrainTree::Succeeder
    {};

    class Failer : public BrainTree::Failer
    {};

    class Inverter : public BrainTree::Inverter
    {};

    class UntilSuccess : public BrainTree::UntilSuccess
    {};

    class UntilFailure : public BrainTree::UntilFailure
    {};

    class Repeater : public BrainTree::Repeater
    {
    public:
        bool load(const XmlNode* node) override;
    };
}