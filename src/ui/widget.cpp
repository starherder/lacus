#include "widget.h"
#include "gui_manager.h"

#include <format>

namespace ui {

DeclareWidgetType(Widget, "widget");


Widget::Widget(const std::string& name, Widget* parent) 
    : _name(name), _parent(parent)
{
}
        
Widget::~Widget()
{
}

void Widget::setPos(const Vec2& pt)
{
    auto oldPos = pos();
    if (pt == oldPos)
    {
        return;
    }

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
    auto oldSize = size();
    if (sz == oldSize)
    {
        return;
    }

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


bool Widget::load(XmlNode* node) 
{ 
    if (!node) return false;

    int id = ui::GuiManager::inst().generateId();
    auto def_name = std::format("widget_{}", id);

    _name = node->Attribute("name");
    _name = _name.empty() ? def_name : _name;

    auto pos = ToVec2(node->Attribute("pos"));
    setPos(pos);

    auto size = ToVec2(node->Attribute("size"));
    setSize(size);

    auto borderSize = node->FloatAttribute("border_size");
    setBorderSize(borderSize);

    auto borderRound = node->FloatAttribute("broder_round");
    setBorderRound(borderRound);

    auto visible = node->BoolAttribute("visible", true);
    setVisible(visible);

    auto canDragOut = node->BoolAttribute("drag_out");
    setCanDragOut(canDragOut);

    _normalStatus.ground_color.fromHexString(node->Attribute("ground_color"));
    _normalStatus.border_color.fromHexString(node->Attribute("border_color"));
    _normalStatus.text_color.fromHexString(node->Attribute("text_color"));

    SPDLOG_INFO("widget: name({}) pos({}) size({}) border_size({}) border_round({}) visible({}) ground_color({}), border_color({})",
        _name, _pos, _size,_borderSize, _borderRound, _visible, _normalStatus.ground_color, _normalStatus.border_color);

    return onLoad(node); 
}


bool Widget::onLoad(XmlNode* node)
{
    return true;
}

void Widget::update(float delta)
{
}

void Widget::draw()
{
    auto relPos = getAbsPos();
    auto& state = status();
    auto& painter = GuiManager::inst().painter();

    Rect bksize = {relPos.x, relPos.y, _size.x, _size.y};

    if(state.texture)
    {
        painter.drawTexture(state.texture, state.tex_rect, bksize, _borderRound);
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
        auto& painter = GuiManager::inst().painter();
        painter.drawRect(state.border_color, bksize, _borderRound);
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