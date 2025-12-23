#pragma once

#include "widget.h"

namespace ui {

///////////////////////////////////////////////////////////////////////////////////////

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

protected:    
    virtual void onChildAdded(Widget* child);
    virtual void onChildRemoved(Widget* child);

    void onChildSizeChanged(Widget* child) override;
    void onChildVisibleChanged(Widget* child) override;

private:
    std::list<SharedPtr> _children;

    bool _clipChildren = true;
};

///////////////////////////////////////////////////////////////////////////////////////

class HorizonalLayout : public Group
{
public:
    HorizonalLayout() = delete;
    ~HorizonalLayout() = default;
	HorizonalLayout(const std::string& name, Widget* parent = nullptr);

    void update(float delta) override;

    const Vec2& padding() { return _padding; }
    void setPadding(const Vec2& padding) { _padding = padding; }

    float spacing() { return _spacing; }
    void setSpacing(float spacing) { _spacing = spacing; }

protected:
    virtual void ajustLayout();

    void onChildAdded(Widget* child) override;
    void onChildRemoved(Widget* child) override;

    void onChildSizeChanged(Widget* child) override;
    void onChildVisibleChanged(Widget* child) override;

protected:
    Vec2 _padding = Vec2{10, 10};
    float _spacing = 10; 
};

///////////////////////////////////////////////////////////////////////////////////////

class VerticalLayout : public HorizonalLayout
{
public:
    VerticalLayout() = delete;
    ~VerticalLayout() = default;
	VerticalLayout(const std::string& name, Widget* parent = nullptr);

    void update(float delta) override;

private:
    void ajustLayout() override;
};

///////////////////////////////////////////////////////////////////////////////////////

template<typename WidgetType>
WidgetType* Group::createChild(const std::string& name)
{
    auto child = getChild<WidgetType>(name);
    if(child)
    {
        return child;
    }

    auto widget = std::make_shared<WidgetType>(name, this);
    _children.push_back(widget);
    onChildAdded(widget.get());
    return widget.get();
}

template<typename WidgetType>
WidgetType* Group::getChild(const std::string& name)
{
    for(auto& child : _children)
    {
        if(child->name() == name)
        {
            return dynamic_cast<WidgetType*>(child.get());
        }
    }

    return nullptr;
}

}