#pragma once

#include "ui_utils.h"

namespace ui {

class Widget 
{
public:
    using UniquePtr = std::unique_ptr<Widget>;

public:
    Widget() = delete;
    Widget(const std::string& name, Widget* parent=nullptr);
    virtual ~Widget();

    virtual void update(float delta);
    virtual void draw();

    const auto& name() { return _name; }

    const Vec2& pos()  const { return _pos; }
    void setPos(const Vec2& pos) { _pos = pos; }

    auto& size() const { return _size; }
    void setSize(const Vec2& sz) { _size = sz; }

    auto& visible() const { return _visible; }
    void setVisible(bool visible) { _visible = visible; }

    auto& handleEvent() const { return _handleEvent; }
    void setHandleEvent(bool handle) { _handleEvent = handle;}

    auto focused() const { return _focused; }
    void setFocused(bool focus) { _focused = focus; }

    Texture* texture() { return status().texture; }
    void setTexture(Texture* tex, const Rect& uv_rect={0,0,1,1});

    const Color& bgColor() { return status().groud_color; }
    void setBgColor(const Color& c) { status().groud_color = c; }

    const Color& borderColor() { return status().border_color; }
    void setBorderColor(const Color& c) { status().border_color = c; }

    Widget* parent() const { return _parent; }

    bool isPosInMe(const Vec2& pos);

    virtual bool isGroup() const { return false; }
    virtual WidgetStatus& status() { return _normalStatus; }
    
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

protected:
    Vec2 getRealPos() const;

    std::string _name;
    Widget* _parent = nullptr;

    bool _visible = true;
    bool _focused = false;
    bool _handleEvent = false;
    

    Vec2 _pos = {0, 0};
    Vec2 _size = {100, 30};

    WidgetStatus _normalStatus = WigetUtils::normalStatus;
};


///////////////////////////////////////////////////////////////////////////////



}