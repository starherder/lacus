#include "groups.h"
#include "gui_manager.h"

namespace ui {


	Group::Group(const std::string& name, Widget* parent) : Widget(name, parent)
    {

    }

    Group::~Group()
    {

    }

    void Group::update(float delta)
    {
        for(auto& [name, ptr] : _children) {
            ptr->update(delta);
        }
    }

    void Group::draw()
    {
        if(!visible()) 
        {
            return;
        }
        auto realPos = getRealPos();
        auto& renderer = GuiManager::inst().renderer();
        auto oldClipRect = renderer.getClipRect();

        if(_clipChildren)
        {
            renderer.setClipRect({realPos, _size});
        }

        Widget::draw();

        for(auto& [name, ptr] : _children) {
            ptr->draw();
        }

        renderer.setClipRect(oldClipRect);
    } 

    void Group::removeChild(const std::string& name)
    {
        _children.erase(name);
    }
}