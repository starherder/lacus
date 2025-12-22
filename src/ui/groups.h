#pragma once

#include "widget.h"

namespace ui {

class Group : public Widget
{
public:
    Group() = delete;
	Group(const std::string& name, Widget* parent = nullptr);
    ~Group();

    bool isGroup() const override { return true; }
    void update(float delta) override;
    void draw() override;

    bool clipChildren() const { return _clipChildren; }
    void setClipChildren(bool clip) { _clipChildren = clip; }

    template<typename WidgetType>
    WidgetType* createChild(const std::string& name);

    template<typename WidgetType>
    WidgetType* getChild(const std::string& name);

    void removeChild(const std::string& name);

    const auto& children() const { return _children; }
    
private:
    std::map<std::string, Widget::UniquePtr> _children;

    bool _clipChildren = true;
};



template<typename WidgetType>
WidgetType* Group::createChild(const std::string& name)
{
    auto it = _children.find(name);
    if(it == _children.end())
    {
        auto ptr = std::make_unique<WidgetType>(name, this);
        auto pWidget = ptr.get();
        _children[name] = std::move(ptr);
        return pWidget;
    }
    else
    {
        return dynamic_cast<WidgetType*>(it->second.get());
    }
}

template<typename WidgetType>
WidgetType* Group::getChild(const std::string& name)
{
    auto it = _children.find(name);
    if (it != _children.end())
    {
        return dynamic_cast<WidgetType*>(it->second.get());
    }
    return nullptr;
}
}