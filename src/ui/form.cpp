#include "form.h"
#include "gui_manager.h"

#include "groups.h"

namespace ui {


Form::Form(const std::string& name) : _name(name)
{
    _rootGroup = std::make_unique<Group>("group_main");
}

Form::~Form()
{
}

Widget* Form::getWidgetInGroup(Group* group, const Vec2& pos)
{
    for(auto& ptr : group->children() )
    {
        if(ptr && ptr->isPosInMe(pos))
        {
            if(ptr->isGroup())
            {
                auto widget = getWidgetInGroup((Group*)ptr.get(), pos);
                if(widget) 
                {
                    return widget;
                }
            }

            return ptr.get();
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

void Form::onMouseLeftClick(const Vec2& pos)
{
    auto widget = _hoverWidget; 
    if (widget)
    {
        if(widget->focused())
        {
            widget->onMouseLeftClick(pos);
        }
        _focused = false;
    }
}

void Form::onMouseRightClick(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget)
    {
        widget->onMouseRightClick(pos);
        _focused = false;
    }
}

void Form::onMouseLeftDown(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget)
    {
        widget->setFocused(true);
        widget->onMouseLeftDown(pos);
        _focused = true;
    }
}

void Form::onMouseLeftUp(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget)
    {
        widget->onMouseLeftUp(pos);
        _focused = false;
    }
}

void Form::onMouseRightDown(const Vec2& pos)
{
    auto widget = _hoverWidget;
    if (widget && widget->focused())
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
        _focused = false;
    }
}

void Form::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
{
    auto widget = _hoverWidget;
    if(widget && widget->handleEvent())
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
    if (widget)
    {
        widget->setFocused(true);
        widget->onMouseWheel(pos, dir);
    }
}

void Form::onMouseMotion(const Vec2& pos, const Vec2& offset)
{
    if(!Rect{_pos, _size}.contains(pos))
    {
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

    if(widget)
    {
        widget->onMouseEnter(pos);
    }

    _hoverWidget = widget;
}



}