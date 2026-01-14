#include "widget.h"
#include "gui_manager.h"

namespace ui {


Widget::Widget(const std::string& name, Widget* parent) 
    : _name(name), _parent(parent)
{
}
        
Widget::~Widget()
{
}

void Widget::setPos(const Vec2& pt)
{
    rawSetPos(pt);

    if (_parent)
    {
        _parent->onChildPosChanged(this);
    }
}

void Widget::rawSetPos(const Vec2& pt)
{
    Vec2 oldPos = _pos;

    _pos = pt;

    onPosChanged(oldPos, pt);
}

void Widget::setSize(const Vec2& sz) 
{ 
    rawSetSize(sz);

    if(_parent)
    {
        _parent->onChildSizeChanged(this);
    }
}

void Widget::rawSetSize(const Vec2& sz)
{
    Vec2 oldSz = _size;

    _size = sz;

    onSizeChanged(oldSz, sz);
}

void Widget::setVisible(bool vis) 
{
    rawSetVisible(vis);

    if(_parent)
    {
        _parent->onChildVisibleChanged(this);
    }
}

void Widget::rawSetVisible(bool vis)
{
    bool oldVis = _visible;

    _visible = vis;

    onVisibleChanged(oldVis, vis);
}

void Widget::setData(const std::string& key, const utility::Var& value)
{
    _properties[key] = value;
}

bool Widget::isPosInMe(const Vec2& pos)
{
    auto realPos = getAbsPos();
    return _visible && pos.x >= realPos.x && pos.x <= realPos.x+_size.x
                    && pos.y >= realPos.y && pos.y <= realPos.y+_size.y;
}

void Widget::setTexture(Texture* tex, const Rect& uv)
{
    if(!tex)
    {
        return;
    }

    auto sz = tex->size();

    status().texture = tex; 
    status().tex_rect = Rect{uv.x*sz.x, uv.y*sz.y, uv.w*sz.x, uv.h*sz.y};
}

Vec2 Widget::getAbsPos() const
{
    if(!_parent) { 
        return _pos;
    }

    return _pos + _parent->getAbsPos() + _parent->getContentPos();
}

Rect Widget::getAbsRect() const
{
    return {getAbsPos(), _size};
}

Rect Widget::getClipRect() const
{
    auto parentWidget = parent();
    if (!parentWidget)
    {
        return getAbsRect();
    }

    return parentWidget->getClipRect().intersect(getAbsRect());
}

void Widget::update(float delta)
{
}

void Widget::draw()
{
    auto relPos = getAbsPos();
    auto& state = status();
    auto& renderer = GuiManager::inst().renderer();
    auto& painter = GuiManager::inst().painter();

    Rect bksize = {relPos.x, relPos.y, _size.x, _size.y};

    if(state.texture)
    {
        if (state.ground_color.isValid()) 
        {
            renderer.setDrawColor(state.ground_color);
        }

        renderer.drawTexture(state.texture, state.tex_rect, bksize);
    }
    else
    {
        if(state.ground_color.isValid())
        {
            painter.fillRect(state.ground_color, bksize, _borderRound);
        }
    }
            
    if(state.border_color.isValid())
    {
        painter.drawRect(state.border_color, bksize, _borderRound, _borderSize);
    }
}

void Widget::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
{
    if (_parent) _parent->onMouseLeftDrag(pos, offset);
}

void Widget::onMouseWheel(const Vec2& pos, float dir)
{
    if (_parent) _parent->onMouseWheel(pos, dir);
}

////////////////////////////////////////////////////////////////////////////

}