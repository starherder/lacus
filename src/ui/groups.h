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

    const std::list<SharedPtr>& children() const { return _children; }

protected:    
    Vec2 getContentPos() const override { return Vec2{ 0.0f, 0.0f }; }

    virtual void onChildAdded(Widget* child);
    virtual void onChildRemoved(Widget* child);

    void onChildSizeChanged(Widget* child) override;
    void onChildVisibleChanged(Widget* child) override;

private:
    std::list<SharedPtr> _children;

    bool _clipChildren = true;
};

///////////////////////////////////////////////////////////////////////////////////////

class ExpandGroup : public Group, public utility::sigslot::SlotHandler
{
public:
public:
    ExpandGroup() = delete;
    ExpandGroup(const std::string& name, Widget* parent = nullptr);
    ~ExpandGroup();

    void update(float delta) override;
    void draw() override;

    std::list<SharedPtr> items() const;

private:
    Vec2 getContentPos() const override { return _contentPos; }

    void onHorizonalSlide(class SliderBar* slider);
    void onVerticalSlide(class SliderBar* slider);

    void onChildAdded(Widget* child) override;
    void onChildRemoved(Widget* child) override;

    void onChildPosChanged(Widget* child) override;
    void onChildSizeChanged(Widget* child) override;

    void ajustContent();
    void ajustScrollbar();

private:
    const int slider_bar_size = 25;

    Vec2 _contentPos = { 0, 0 };
    Vec2 _contentSize = { 0, 0 };

    class SliderBar* _horizonSlider = nullptr;
    class SliderBar* _verticalSlider = nullptr;
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

    void onChildPosChanged(Widget* child) override;
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