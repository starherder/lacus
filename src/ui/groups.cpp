#include "groups.h"
#include "gui_manager.h"
#include "widget.h"

namespace ui {


	Group::Group(const std::string& name, Widget* parent) : Widget(name, parent)
    {
    }

    Group::~Group()
    {
    }

    void Group::update(float delta)
    {
        for(auto& ptr : _children) 
        {
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

        for(auto& ptr : _children) 
        {
            ptr->draw();
        }

        renderer.setClipRect(oldClipRect);
    } 

    void Group::removeChild(const std::string& name)
    {
        for(auto it=_children.begin(); it!=_children.end(); it++)
        {
            auto ptr = *it;
            onChildRemoved(ptr.get());

            _children.erase(it);
            return;
        }
    }
    
    void Group::onChildAdded(Widget* child)
    {
    }

    void Group::onChildRemoved(Widget* child)
    {
    }


    ///////////////////////////////////////////////////////////////////////////////////////



	HorizonalLayout::HorizonalLayout(const std::string& name, Widget* parent )
        : Group(name, parent)
    {
    }

    void HorizonalLayout::update(float delta) 
    {
    }

    void HorizonalLayout::ajustLayout()
    {
        int width_used = _padding.x * 2;

        std::vector<Widget*> scalable_widgets;

        for(auto& ptr : children())
        {
            if(!ptr->visible()) 
            {
                continue;
            }

            if(ptr->getProperty<bool>("_scalable"))
            {
                scalable_widgets.push_back(ptr.get());
            }
            else 
            {
                width_used += ptr->size().x;
            }
            width_used += _spacing;
        }
        
        if(scalable_widgets.empty())
        {
            return;
        }
        
        width_used -= _spacing;
        
        float scalable_width = (size().x - width_used) / scalable_widgets.size();
        float scalable_height = size().y - _padding.y * 2;

        width_used = _padding.x;

        for(auto& ptr : children())
        {
            if(!ptr->visible()) 
            {
                continue;
            }

            ptr->setPos({width_used, _padding.y});

            if(ptr->getProperty<bool>("_scalable"))
            {
                ptr->setSize({scalable_width, scalable_height});
                width_used += scalable_width;
            }
            else 
            {
                width_used += ptr->size().x;
            }

            width_used += _spacing;
        }
    }

    void HorizonalLayout::onChildAdded(Widget* child) 
    {
        child->setProperty("_scalable", true);

        ajustLayout();
    }

    void HorizonalLayout::onChildRemoved(Widget* child) 
    {
        ajustLayout();
    }




    ///////////////////////////////////////////////////////////////////////////////////////



	VerticalLayout::VerticalLayout(const std::string& name, Widget* parent)
        : HorizonalLayout(name, parent)
    {
    }

    void VerticalLayout::update(float delta)
    {
    }
    
    void VerticalLayout::ajustLayout()
    {
        int height_used = _padding.y * 2;

        std::vector<Widget*> scalable_widgets;

        for(auto& ptr : children())
        {
            if(!ptr->visible()) 
            {
                continue;
            }

            if(ptr->getProperty<bool>("_scalable"))
            {
                scalable_widgets.push_back(ptr.get());
            }
            else 
            {
                height_used += ptr->size().y;
            }

            height_used += _spacing;
        }
        
        if(scalable_widgets.empty())
        {
            return;
        }
        
        height_used -= _spacing;
        
        float scalable_width = size().x - _padding.x * 2;
        float scalable_height = (size().y - height_used) / scalable_widgets.size();

        height_used = _padding.y;

        for(auto& ptr : children())
        {
            if(!ptr->visible()) 
            {
                continue;
            }

            ptr->setPos({_padding.x, height_used});

            if(ptr->getProperty<bool>("_scalable"))
            {
                ptr->setSize({scalable_width, scalable_height});
                height_used += scalable_height;
            }
            else 
            {
                height_used += ptr->size().y;
            }

            height_used += _spacing;
        }

    }




}