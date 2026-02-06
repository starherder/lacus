#include "widget.h"
#include "gui_manager.h"

#include <format>

namespace ui {

DeclareWidgetType(Widget, "widget");

static const std::map<std::string, std::string>& parseData(const std::string& str)
{
    static std::map<std::string, std::string> result;
    result.clear();

    auto sv = utility::StringUtil::split(str, ',');
    for(auto& val : sv)
    {
        std::string str{val};
        auto p = utility::StringUtil::split(str, ':');
        if(p.size() == 2) {
            std::string k{ p[0] };
            std::string v{ p[1] };
            result[k] = v;
        }
    }

    return result;
}

Widget::Widget(const std::string& name, Widget* parent) 
    : _name(name), _parent(parent)
{
    _normalStatus = WidgetUtils::normalStatus;
    _normalStatus.border_color = Color::Invalid;
    _normalStatus.ground_color = Color::Invalid;
    _normalStatus.text_color = Color::Invalid;
    _normalStatus.texture = nullptr;
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

void Widget::setTexTile(const std::string& texname)
{
    if (texname.empty())
    {
        return;
    }

    std::string texset = "";
    std::string textile = texname;
    
    const auto& texarr = utility::StringUtil::split(texname, ':');
    if (texarr.size() == 2) 
    {
        texset = texarr[0];
        textile = texarr[1];
    }

    auto tex = GuiManager::inst().getTexTile(textile, texset);
    setTexTile(tex);
}

void Widget::setTexTile(TexTile* tex)
{
    status().texture = tex;
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

    auto borderRound = node->FloatAttribute("border_round");
    setBorderRound(borderRound);

    auto visible = node->BoolAttribute("visible", true);
    setVisible(visible);

    auto canDragOut = node->BoolAttribute("drag_out");
    setCanDragOut(canDragOut);

    auto acceptEvent = node->BoolAttribute("accept_event", true);
    setAcceptEvent(acceptEvent);

    auto texname = node->Attribute("texture");
    setTexTile(texname);

    auto canDrag = node->BoolAttribute("can_dragout", false);
    setCanDragOut(canDrag);

    auto canDrop = node->BoolAttribute("can_dropin", false);
    setCanDropIn(canDrop);

    auto data = node->Attribute("data");
    if(strlen(data) > 0 ) {
        auto& datamap = parseData(data);
        for(auto& [k, v] : datamap) {
            setData(k, v);
        }
    }

    WidgetUtils::parseColorIfValid(_normalStatus.ground_color, node->Attribute("ground_color"));
    WidgetUtils::parseColorIfValid(_normalStatus.border_color, node->Attribute("border_color"));
    WidgetUtils::parseColorIfValid(_normalStatus.text_color, node->Attribute("text_color"));

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
        painter.drawTexTile(state.texture, bksize, borderRound(), state.ground_color);
    }
    else
    {
        if(state.ground_color.isValid())
        {
            painter.fillRect(state.ground_color, bksize, borderRound());
        }
    }
            
    if(state.border_color.isValid())
    {
        auto& painter = GuiManager::inst().painter();
        painter.drawRect(state.border_color, bksize, borderRound(), borderSize());
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