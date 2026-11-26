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

    const Vec2& padding() const { return _padding; }
    void setPadding(const Vec2& padding);

    Rect getClipRect() const;

    template<typename WidgetType>
    WidgetType* createChild(const std::string& name);

    template<typename WidgetType>
    WidgetType* getChild(const std::string& name, bool recursion=false);

    void addWidget(WidgetPtr widget, int index=-1);

    void removeChild(const std::string& name);

    void clear();

    void bringTop(Widget* child);
    void bringBottom(Widget* child);

    WidgetPtr moveOut(Widget* child);

    const std::list<WidgetPtr>& children() const { return _children; }

protected:
    bool onLoad(XmlNode* node) override;

    Vec2 getContentPos() const override { return Vec2{ 0.0f, 0.0f }; }

    virtual void onChildAdded(Widget* child);
    virtual void onChildRemoved(Widget* child);
    virtual void onPaddingChanged(const Vec2& oldPadding, const Vec2& newPadding);

    void onChildSizeChanged(Widget* child) override;
    void onChildVisibleChanged(Widget* child) override;

    void onPosChanged(const Vec2& oldPos, const Vec2& newPos) override;
    void onSizeChanged(const Vec2& oldPos, const Vec2& newPos) override;
    void onVisibleChanged(bool oldVisual, bool newVisual) override;

protected:
    Vec2 _padding = { 0.0f, 0.0f };

private:
    std::list<WidgetPtr> _children;

    bool _clipChildren = true;
};

///////////////////////////////////////////////////////////////////////////////////////

class BackGroup : public Group
{
public:
    BackGroup() = delete;
    BackGroup(const std::string& name, Widget* parent = nullptr);
    ~BackGroup();

protected:
    bool onLoad(XmlNode* node) override;

    void adjustChildren();

    virtual void onChildAdded(Widget* child);
    virtual void onChildRemoved(Widget* child);

    void onSizeChanged(const Vec2& oldPos, const Vec2& newPos) override;

};


///////////////////////////////////////////////////////////////////////////////////////

class ExpandGroup : public Group, public utility::sigslot::SlotHandler
{
public:
public:
    ExpandGroup() = delete;
    ExpandGroup(const std::string& name, Widget* parent = nullptr);
    ~ExpandGroup();

    std::vector<Widget*> items() const;

protected:
    bool onLoad(XmlNode* node) override;

    Vec2 getContentPos() const override { return _contentPos; }

    virtual bool isContentWidget(Widget* child) const;
    virtual Vec2 getContentEndPadding() const { return _padding; }

    void onHorizontalSlide(class SliderBar* slider);
    void onVerticalSlide(class SliderBar* slider);

    void onChildAdded(Widget* child) override;
    void onChildRemoved(Widget* child) override;

    void onChildPosChanged(Widget* child) override;
    void onChildSizeChanged(Widget* child) override;

    void onMouseLeftDrag(const Vec2& pos, const Vec2& offset) override;
    void onMouseWheel(const Vec2& pos, float dir) override;

    void adjustContent();
    void adjustScrollbar();

    void onSizeChanged(const Vec2& oldPos, const Vec2& newPos) override;

protected:
    const int slider_bar_size = 25;

    Vec2 _contentPos = { 0, 0 };
    Vec2 _contentSize = { 0, 0 };

    class SliderBar* _horizonSlider = nullptr;
    class SliderBar* _verticalSlider = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////

class GridGroup : public ExpandGroup
{
public:
    GridGroup() = delete;
    GridGroup(const std::string& name, Widget* parent = nullptr);
    ~GridGroup() = default;

    const Vec2& spacing() const { return _spacing; }
    void setSpacing(const Vec2& spacing);

    int rows() const { return _rows; }
    void setRows(int rows);

    int columns() const { return _columns; }
    void setColumns(int columns);

    const Vec2& childSize() const { return _childSize; }
    void setChildSize(const Vec2& childSize);

    int getInsertIndexAt(const Vec2& pos) const;

protected:
    bool onLoad(XmlNode* node) override;

    bool isContentWidget(Widget* child) const override;
    Vec2 getContentEndPadding() const override { return _contentEndPadding; }

    void adjustLayout();

    void onPaddingChanged(const Vec2& oldPadding, const Vec2& newPadding) override;

    void onChildAdded(Widget* child) override;
    void onChildRemoved(Widget* child) override;
    void onChildPosChanged(Widget* child) override;
    void onChildSizeChanged(Widget* child) override;
    void onChildVisibleChanged(Widget* child) override;

    void onSizeChanged(const Vec2& oldPos, const Vec2& newPos) override;

private:
    Vec2 _spacing = { 20.0f, 20.0f };
    Vec2 _childSize = { 0.0f, 0.0f };
    Vec2 _contentEndPadding = { 20.0f, 20.0f };
    int _rows = 1;
    int _columns = 1;
};

///////////////////////////////////////////////////////////////////////////////////////

class HorizontalLayout : public Group
{
public:
    HorizontalLayout() = delete;
    ~HorizontalLayout() = default;
	HorizontalLayout(const std::string& name, Widget* parent = nullptr);

    float spacing() { return _spacing; }
    void setSpacing(float spacing) { _spacing = spacing; }

protected:
    bool onLoad(XmlNode* node) override;

    virtual void adjustLayout();

    void onSizeChanged(const Vec2& oldPos, const Vec2& newPos) override;

    void onChildAdded(Widget* child) override;
    void onChildRemoved(Widget* child) override;
    void onPaddingChanged(const Vec2& oldPadding, const Vec2& newPadding) override;

    void onChildPosChanged(Widget* child) override;
    void onChildSizeChanged(Widget* child) override;
    void onChildVisibleChanged(Widget* child) override;

protected:
    float _spacing = 10; 
};

using HLayout = HorizontalLayout;

///////////////////////////////////////////////////////////////////////////////////////

class VerticalLayout : public HorizontalLayout
{
public:
    VerticalLayout() = delete;
    ~VerticalLayout() = default;
	VerticalLayout(const std::string& name, Widget* parent = nullptr);

protected:
    bool onLoad(XmlNode* node) override;

    void adjustLayout() override;

    void onChildSizeChanged(Widget* child) override;
};

using VLayout = VerticalLayout;

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
WidgetType* Group::getChild(const std::string& name, bool recursion)
{
    for(auto& child : _children)
    {
        if(child->name() == name)
        {
            return dynamic_cast<WidgetType*>(child.get());
        }

        if (recursion && child->isGroup())
        {
            if (auto group = dynamic_cast<Group*>(child.get()))
            {
                auto widget = group->getChild<WidgetType>(name, true);
                if (widget)
                {
                    return widget;
                }
            }
        }
    }

    return nullptr;
}

}
