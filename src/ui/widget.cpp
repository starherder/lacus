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


bool Widget::isPosInMe(const Vec2& pos)
{
    auto realPos = getRealPos();
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

Vec2 Widget::getRealPos() const
{
    if(!_parent) { 
        return _pos;
    }

    return _pos + _parent->getRealPos();
}

void Widget::update(float delta)
{
}

void Widget::draw()
{
    auto relPos = getRealPos();
    auto& state = status();
    auto& renderer = GuiManager::inst().renderer();

    if(state.texture)
    {
        if (state.groud_color.isValid()) 
        {
            renderer.setDrawColor(state.groud_color);
        }

        renderer.drawTexture(state.texture, state.tex_rect, {relPos.x, relPos.y, _size.x, _size.y});
    }
    else
    {
        if(state.groud_color.isValid())
        {
            renderer.setDrawColor(state.groud_color);
            renderer.drawFillRect({relPos.x, relPos.y, _size.x, _size.y});
        }
    }
            
    if(state.border_color.isValid())
    {
        renderer.setDrawColor(state.border_color);
        renderer.drawRect({relPos.x, relPos.y, _size.x, _size.y});
    }
}

////////////////////////////////////////////////////////////////////////////

}