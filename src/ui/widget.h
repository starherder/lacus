#pragma once

#include "ui_utils.h"

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
    void setPos(const Vec2& pos) { _pos = pos; }

    auto& size() const { return _size; }
    void setSize(const Vec2& sz);
    void rawSetSize(const Vec2& sz) { _size = sz; }
    
    auto& visible() const { return _visible; }
    void setVisible(bool visible);

    auto& handleEvent() const { return _handleEvent; }
    void setHandleEvent(bool handle) { _handleEvent = handle;}

    auto focused() const { return _focused; }
    void setFocused(bool focus) { _focused = focus; }

    Texture* texture() { return status().texture; }
    void setTexture(Texture* tex, const Rect& uv_rect={0,0,1,1});

    const Color& bgColor() { return status().ground_color; }
    void setBgColor(const Color& c) { status().ground_color = c; }

    const Color& borderColor() { return status().border_color; }
    void setBorderColor(const Color& c) { status().border_color = c; }

    virtual bool isGroup() const { return false; }
    bool isPosInMe(const Vec2& pos);

    bool scaleInGroup() const { return _scaleInGroup; }
    void setScaleInGroup(bool scale) { _scaleInGroup = scale; }

    Widget* parent() const { return _parent; }

    template<typename T>
    T getData(const std::string& key) const;
    void setData(const std::string& key, const utility::Var& value);
    
public:
    virtual void onMouseEnter() {}
    virtual void onMouseLeave() {}

    virtual void onMouseLeftClick() {}
    virtual void onMouseRightClick() {}

    virtual void onMouseLeftDown() {}
    virtual void onMouseLeftUp() {}

    virtual void onMouseRightDown() {}
    virtual void onMouseRightUp() {}

    virtual void onMouseLeftDrag(const Vec2& offset) {}
    virtual void onMouseWheel(float dir) {}

    virtual void onChildSizeChanged(Widget* child) {}
    virtual void onChildVisibleChanged(Widget* child) {}

protected:
    Vec2 getRealPos() const;
    
    virtual WidgetStatus& status() { return _normalStatus; }

    std::string _name;
    Widget* _parent = nullptr;

    bool _visible = true;
    bool _focused = false;
    bool _handleEvent = false;
    bool _scaleInGroup = true;

    Vec2 _pos = {0, 0};
    Vec2 _size = {100, 30};

    Properties _properties;

    WidgetStatus _normalStatus = WigetUtils::normalStatus;
};


template<typename T>
T Widget::getData(const std::string& key) const
{
    return (T)_properties[key];
}
///////////////////////////////////////////////////////////////////////////////



}