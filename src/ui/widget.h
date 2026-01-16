#pragma once

#include "ui_utils.h"

//#define USE_GFX_PAINTER

namespace ui {

class Widget 
{
public:
    using SharedPtr = std::shared_ptr<Widget>;

public:
    Widget() = delete;
    Widget(const std::string& name, Widget* parent=nullptr);
    virtual ~Widget();

    const auto& name() { return _name; }

    virtual void update(float delta);
    virtual void draw();

    const Vec2& pos()  const { return _pos; }
    virtual void setPos(const Vec2& pos);
    virtual void rawSetPos(const Vec2& pos);

    const auto& size() const { return _size; }
    virtual void setSize(const Vec2& sz);
    virtual void rawSetSize(const Vec2& sz);
    
    auto visible() const { return _visible; }
    virtual void setVisible(bool visible);
    virtual void rawSetVisible(bool visible);

    auto dragable() const { return _dragable; }
    void setDragable(bool drag) { _dragable = drag; }

    auto canDragOut() const { return _canDragOut; }
    void setCanDragOut(bool d) { _canDragOut = d; }

    auto& noEvent() const { return _noEvent; }
    void setNoEvent(bool noEvent) { _noEvent = noEvent; }

    auto focused() const { return _focused; }
    void setFocused(bool focus) { _focused = focus; }

    Texture* texture() { return status().texture; }
    void setTexture(Texture* tex, const Rect& uv_rect={0,0,1,1});

    const Color& bgColor() { return status().ground_color; }
    void setBgColor(const Color& c) { status().ground_color = c; }

    const Color& borderColor() { return status().border_color; }
    void setBorderColor(const Color& c) { status().border_color = c; }

    float borderRound() { return _borderRound; }
    void setBorderRound(float round) { _borderRound = round; }

    float borderSize() { return _borderSize; }
    void setBorderSize(float sz) { _borderSize = sz; }

    virtual bool isGroup() const { return false; }
    bool isPosInMe(const Vec2& pos);

    bool scaleInGroup() const { return _scaleInGroup; }
    void setScaleInGroup(bool scale) { _scaleInGroup = scale; }

    Widget* parent() const { return _parent; }
    void setParent(Widget* widget) { _parent = widget; }

    template<typename T>
    T getData(const std::string& key) const;
    void setData(const std::string& key, const utility::Var& value);

    Vec2 getAbsPos() const;

    Rect getAbsRect() const;

public:
    virtual void onMouseEnter(const Vec2& pos) {}
    virtual void onMouseLeave(const Vec2& pos) {}

    virtual void onMouseLeftClick(const Vec2& pos) {}
    virtual void onMouseRightClick(const Vec2& pos) {}

    virtual void onMouseLeftDown(const Vec2& pos) {}
    virtual void onMouseLeftUp(const Vec2& pos) {}

    virtual void onMouseRightDown(const Vec2& pos) {}
    virtual void onMouseRightUp(const Vec2& pos) {}

    virtual void onMouseLeftDrag(const Vec2& pos, const Vec2& offset);
    virtual void onMouseWheel(const Vec2& pos, float dir);

    virtual void onChildSizeChanged(Widget* child) {}
    virtual void onChildPosChanged(Widget* child) {}
    virtual void onChildVisibleChanged(Widget* child) {}

    virtual void onPosChanged(const Vec2& oldPos, const Vec2& newPos) {}
    virtual void onSizeChanged(const Vec2& oldPos, const Vec2& newPos) {}
    virtual void onVisibleChanged(bool oldVisual, bool newVisual) {}

protected:

    virtual Vec2 getContentPos() const { return Vec2{0.0f, 0.0f}; }
    
    virtual WidgetStatus& status() { return _normalStatus; }

    std::string _name;
    Widget* _parent = nullptr;

    bool _visible = true;
    bool _focused = false;
    bool _dragable= false;
    bool _noEvent = true;
    bool _scaleInGroup = true;
    bool _canDragOut = false;

    float _borderSize = 1.0f;
    float _borderRound = 0.0f;

    Vec2 _pos = {0, 0};
    Vec2 _size = {100, 30};

    Properties _properties;

    WidgetStatus _normalStatus = WigetUtils::normalStatus;
};

using WidgetPtr = Widget::SharedPtr;

template<typename T>
T Widget::getData(const std::string& key) const
{
    return _properties[key].convert<T>();
}
///////////////////////////////////////////////////////////////////////////////



}