#pragma once

#include "ui_utils.h"

namespace ui {

class Widget;
class Group;

class Form : public sigslot::SlotHandler
{
public:
    using SharedPtr = std::shared_ptr<Form>;

public:
    Form() = delete;
    Form(const std::string& name);
    virtual ~Form();

    Group* root() { return _rootGroup.get(); }
    Widget* getWidgetAtPos(const Vec2& pos);

    virtual void update(float delta);
    virtual void draw();

    void show();
    void hide();

    auto& name() { return _name; }
    bool visible() { return _visible; }

    const Vec2& pos()  const { return _pos; }
    void setPos(const Vec2& pos) { _pos = pos; }

    auto& size() const { return _size; }
    void setSize(const Vec2& sz) { _size = sz; }

    virtual void onShow(bool visible) {}
    virtual void onClose() {}
    virtual void onUpdate(float delta) {}
    virtual void onDraw() {}

public:
    void onMouseLeftClick(const Vec2& pos);
    void onMouseRightClick(const Vec2& pos);

    void onMouseLeftDown(const Vec2& pos);
    void onMouseLeftUp(const Vec2& pos);

    void onMouseRightDown(const Vec2& pos);
    void onMouseRightUp(const Vec2& pos);

    void onMouseLeftDrag(const Vec2& pos, const Vec2& offset);
    void onMouseWheel(const Vec2& pos, float dir);
    void onMouseMotion(const Vec2& pos, const Vec2& offset);

private:
    Widget* getWidgetInGroup(Group* group, const Vec2& pos);

private:
    std::string _name;
    bool _visible = true;
    bool _dragMovable = true;

    Vec2 _pos;
    Vec2 _size;

    std::unique_ptr<Group> _rootGroup = nullptr;
    Widget* _hoverWidget = nullptr;
};

}