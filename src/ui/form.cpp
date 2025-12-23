#include "form.h"
#include "gui_manager.h"


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
                return getWidgetInGroup((Group*)ptr.get(), pos);
            }

            return ptr.get();
        }
    }
    return nullptr;
}

Widget* Form::getWidgetAtPos(const Vec2& pos)
{
    return getWidgetInGroup(_rootGroup.get(), pos);
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
    auto widget = getWidgetAtPos(pos);
    if (widget && widget->focused())
    {
        widget->onMouseLeftClick();
    }
}

void Form::onMouseRightClick(const Vec2& pos)
{
    auto widget = getWidgetAtPos(pos);
    if (widget)
    {
        widget->onMouseRightClick();
    }
}

void Form::onMouseLeftDown(const Vec2& pos)
{
    auto widget = getWidgetAtPos(pos);
    if (widget)
    {
        widget->setFocused(true);
        widget->onMouseLeftDown();
    }
}

void Form::onMouseLeftUp(const Vec2& pos)
{
    auto widget = getWidgetAtPos(pos);
    if (widget)
    {
        widget->onMouseLeftUp();
    }
}

void Form::onMouseRightDown(const Vec2& pos)
{
    auto widget = getWidgetAtPos(pos);
    if (widget && widget->focused())
    {
        widget->setFocused(true);
        widget->onMouseRightDown();
    }
}
void Form::onMouseRightUp(const Vec2& pos)
{
    auto widget = getWidgetAtPos(pos);
    if (widget && widget->focused())
    {
        widget->onMouseRightUp();
    }
}

void Form::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
{
    auto widget = getWidgetAtPos(pos);
    if(widget && widget->handleEvent())
    {
        widget->setFocused(true);
        widget->onMouseLeftDrag(offset);
    }
    else 
    {
        if(_dragMovable)
        {
            _pos += offset;
        }
    }
}

void Form::onMouseWheel(const Vec2& pos, float dir)
{
    auto widget = getWidgetAtPos(pos);
    if (widget)
    {
        widget->setFocused(true);
        widget->onMouseWheel(dir);
    }
}

void Form::onMouseMotion(const Vec2& pos, const Vec2& offset)
{
    auto widget = getWidgetAtPos(pos);
    if(widget == _hoverWidget)
    {
        return;
    }

    if(_hoverWidget)
    {
        _hoverWidget->onMouseLeave();
    }

    if(widget)
    {
        widget->onMouseEnter();
    }

    _hoverWidget = widget;
}

}