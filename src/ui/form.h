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

    template<typename WidgetType>
    WidgetType* getWidget(const std::string& name)
    {
        auto widget = getWidgetInGroup(_rootGroup.get(), name);
        return dynamic_cast<WidgetType*>(widget);
    }

    void update(float delta);
    void draw();

    void show();
    void hide();

    auto& name() { return _name; }
    bool visible() { return _visible; }
    bool focused() { return _focused; }

    const Vec2& pos()  const { return _pos; }
    void setPos(const Vec2& pos);

    auto& size() const { return _size; }
    void setSize(const Vec2& sz);

    const Rect& getRect() const;
    void setRect(const Rect& rect);

    virtual void onShow(bool visible) {}
    virtual void onClose() {}
    virtual void onUpdate(float delta) {}
    virtual void onDraw() {}

    virtual void onPosChanged() {}
    virtual void onSizeChanged() {}

    Widget* hoverWidget() { return _hoverWidget; }

    void setMaximize(bool v);
    bool isMaximize() { return _maximized; }

public:
    virtual void onMouseLeftClick(const Vec2& pos);
    virtual void onMouseRightClick(const Vec2& pos);

    virtual void onMouseLeftDown(const Vec2& pos);
    virtual void onMouseLeftUp(const Vec2& pos);

    virtual void onMouseRightDown(const Vec2& pos);
    virtual void onMouseRightUp(const Vec2& pos);

    virtual void onMouseLeftDrag(const Vec2& pos, const Vec2& offset);
    virtual void onMouseWheel(const Vec2& pos, float dir);
    virtual void onMouseMotion(const Vec2& pos, const Vec2& offset);

    virtual void onWindowResized(const Vec2& size);

private:
    Widget* getWidgetInGroup(Group* group, const Vec2& pos);
    Widget* getWidgetInGroup(Group* group, const std::string& name);

private:
    std::string _name;

    bool _maximized = false;
    bool _visible = true;
    bool _focused = false;
    bool _dragMovable = true;

    Vec2 _pos = {0, 0};
    Vec2 _size = {400, 300};

    std::unique_ptr<Group> _rootGroup = nullptr;
    Widget* _hoverWidget = nullptr;
};

using FormPtr = Form::SharedPtr;

}