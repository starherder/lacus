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

void Widget::setTexture(Texture* tex, FRect uv)
{
    if(!tex)
    {
        return;
    }

    auto sz = tex->size();

    status().texture = tex; 
    status().tex_rect = FRect{uv.x*sz.x, uv.y*sz.y, uv.w*sz.x, uv.h*sz.y};
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
        if (ColorUtils::is_valid(state.groud_color)) 
        {
            renderer.setDrawColor(state.groud_color);
        }

        renderer.drawTexture(state.texture, state.tex_rect, {relPos.x, relPos.y, _size.x, _size.y});
    }
    else
    {
        if(ColorUtils::is_valid(state.groud_color))
        {
            renderer.setDrawColor(state.groud_color);
            renderer.drawFillRect({relPos.x, relPos.y, _size.x, _size.y});
        }
    }
            
    if(ColorUtils::is_valid(state.border_color))
    {
        renderer.setDrawColor(state.border_color);
        renderer.drawRect({relPos.x, relPos.y, _size.x, _size.y});
    }
}

////////////////////////////////////////////////////////////////////////////

}