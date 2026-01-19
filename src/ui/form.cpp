#include "form.h"
#include "gui_manager.h"
#include "base_widgets.h"
#include "group_widgets.h"

namespace ui {


Form::Form(const std::string& name) : _name(name)
{
    _rootGroup = std::make_unique<Group>("group_main");

    root()->setMovable(true);
    root()->setAcceptEvent(true);
}

Form::~Form()
{
}

void Form::setMaximize(bool v)
{ 
    _maximized = v; 

    if (isMaximize())
    {
        setPos({ 0, 0 });
        setSize(ui::GuiManager::inst().windowSize());
    }
}

void Form::setPos(const Vec2& pos)
{ 
    _pos = pos; 
    onPosChanged(); 
}

void Form::setSize(const Vec2& sz)
{ 
    _size = sz; 
    onSizeChanged(); 
}

const Rect& Form::getRect() const
{ 
    static Rect rect; 
    rect = Rect{ _pos, _size }; 
    return rect; 
}

void Form::setRect(const Rect& rect) 
{ 
    setPos(rect.pos()); 
    setSize(rect.size()); 
}

Widget* Form::getWidgetInGroup(Group* group, const Vec2& pos)
{
    auto& children = group->children();

    for(auto it=children.rbegin(); it!=children.rend(); it++)
    {
        auto& ptr = *it;
        if(ptr && ptr->isPosInMe(pos))
        {
            if(ptr->isGroup())
            {
                auto widget = getWidgetInGroup((Group*)ptr.get(), pos);
                if(widget && widget->acceptEvent())
                {
                    return widget;
                }
            }

            if (ptr->acceptEvent()) 
            {
                return ptr.get();
            }
        }
    }
    return nullptr;
}

Widget* Form::getWidgetInGroup(Group* group, const std::string& name)
{
    auto& children = group->children();

    for (auto it = children.rbegin(); it != children.rend(); it++)
    {
        auto& ptr = *it;
        if (!ptr) 
        {
            continue;
        }
        
        if (ptr->name() == name) 
        {
            return ptr.get();
        }

        if (ptr->isGroup())
        {
            auto widget = getWidgetInGroup((Group*)ptr.get(), name);
            if (widget) 
            {
                return widget;
            }
        }
    }
    return nullptr;
}

Widget* Form::getWidgetAtPos(const Vec2& pos)
{
    auto widget = getWidgetInGroup(_rootGroup.get(), pos);
    if(!widget) 
    {
        return _rootGroup.get();
    }
    return widget;
}

void Form::update(float delta)
{
    _rootGroup->setPos(_pos);
    _rootGroup->setSize(_size);
    _rootGroup->update(delta);

    onUpdate(delta);
}

void Form::draw()
{
    if(!visible())
    {
        return;
    }

    auto& renderer = GuiManager::inst().renderer();
    auto oldClipRect = renderer.getClipRect();

    renderer.setClipRect({_pos, _size});

    _rootGroup->draw();

    onDraw();

    renderer.setClipRect(oldClipRect);
}

void Form::show()
{
    _visible = true;
    onShow(true);
}

void Form::hide()
{
    _visible = false;
    onShow(false);
}

void Form::onWindowResized(const Vec2& size)
{
    if (isMaximize())
    {
        setPos({ 0, 0 });
        setSize(ui::GuiManager::inst().windowSize());
    }
}

void Form::onMouseLeftClick(const Vec2& pos)
{
    auto widget = _hoverWidget; 
    if(widget && widget->focused() && widget->acceptEvent())
    {
        widget->onMouseLeftClick(pos);
    }
}

void Form::onMouseRightClick(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget && widget->focused() && widget->acceptEvent())
    {
        widget->onMouseRightClick(pos);
    }
}

void Form::onMouseLeftDown(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget && widget->acceptEvent())
    {
        widget->setFocused(true);
        widget->onMouseLeftDown(pos);
        _focused = true;
    }
}

void Form::onMouseLeftUp(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget && widget->acceptEvent())
    {
        widget->onMouseLeftUp(pos);
    }
}

void Form::onMouseRightDown(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget && widget->focused() && widget->acceptEvent())
    {
        widget->setFocused(true);
        widget->onMouseRightDown(pos);
        _focused = true;
    }
}

void Form::onMouseRightUp(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget && widget->focused())
    {
        widget->onMouseRightUp(pos);
    }
}

void Form::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
{
    auto widget = _hoverWidget;
    if(!widget) 
    {
        return;
    }

    if(!widget->movable())
    {
        widget->setFocused(true);
        widget->onMouseLeftDrag(pos, offset);
    }
    else 
    {
        if(_dragMovable && _focused)
        {
            _pos += offset;
        }
    }
}

void Form::onMouseWheel(const Vec2& pos, float dir)
{
    auto widget = _hoverWidget;
    if (widget && widget->acceptEvent())
    {
        widget->setFocused(true);
        widget->onMouseWheel(pos, dir);
    }
}

void Form::onMouseMotion(const Vec2& pos, const Vec2& offset)
{
    if(!getRect().contains(pos))
    {
        _hoverWidget = nullptr;
        return;
    }

    auto widget = getWidgetAtPos(pos);
    if(widget == _hoverWidget)
    {
        return;
    }

    if(_hoverWidget)
    {
        _hoverWidget->onMouseLeave(pos);
    }

    if(widget && widget->acceptEvent())
    {
        widget->onMouseEnter(pos);
    }

    _hoverWidget = widget;
}



}