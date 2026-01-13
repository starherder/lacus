#include "group_widgets.h"
#include "base_widgets.h"

#include "gui_manager.h"
#include "widget.h"

namespace ui {


	Group::Group(const std::string& name, Widget* parent) : Widget(name, parent)
    {
        adjustClipRect();
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
            renderer.setClipRect(_clipRect);
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

    void Group::bringTop(Widget* child)
    {
        for (auto it = _children.begin(); it != _children.end(); it++)
        {
            auto ptr = *it;
            if (ptr.get() == child) {
                _children.erase(it);
                _children.push_back(ptr);
                return;
            }
        }
    }

    void Group::bringBottom(Widget* child)
    {
        for (auto it = _children.begin(); it != _children.end(); it++)
        {
            auto ptr = *it;
            if (ptr.get() == child) {
                _children.erase(it);
                _children.push_front(ptr);
                return;
            }
        }
    }

    void Group::onChildAdded(Widget* child)
    {
    }

    void Group::onChildRemoved(Widget* child)
    {
    }

    void Group::onChildSizeChanged(Widget* child) 
    {
    }

    void Group::onChildVisibleChanged(Widget* child) 
    {
    }

    void Group::onPosChanged(const Vec2& oldPos, const Vec2& newPos)
    {
        adjustClipRect();
    }
    
    void Group::onSizeChanged(const Vec2& oldPos, const Vec2& newPos)
    {
        adjustClipRect();
    }

    void Group::onVisibleChanged(bool oldVisual, bool newVisual)
    {
    }

    void Group::adjustClipRect()
    {
        auto parentGroup = dynamic_cast<Group*>(parent());
        if (parentGroup)
        {
            auto& renderer = GuiManager::inst().renderer();
            _clipRect = renderer.intersectRect(parentGroup->clipRect(), Rect{ pos(), size() });
        }
        else
        {
            _clipRect = Rect{ pos(), size() };
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////

    ExpandGroup::ExpandGroup(const std::string& name, Widget* parent) : Group(name, parent)
    {
        _contentPos = {0, 0};
        _contentSize = size();

        setDragable(true);

        setNoEvent(false);

        _horizonSlider = createChild<ui::SliderBar>("__h_slider__");
        _horizonSlider->setDirection(ui::Coordinate::Horizonal);
        _horizonSlider->on_value_changed.connect(this, &ExpandGroup::onHorizonalSlide);

        _verticalSlider = createChild<ui::SliderBar>("__v_slider__");
        _verticalSlider->setDirection(ui::Coordinate::Vertical);
        _verticalSlider->on_value_changed.connect(this, &ExpandGroup::onVerticalSlide);

        adjustScrollbar();
    }

    ExpandGroup::~ExpandGroup()
    {
    }

    void ExpandGroup::onHorizonalSlide(SliderBar* slider)
    {
        _contentPos.x = -slider->value();

        adjustScrollbar();
    }

    void ExpandGroup::onVerticalSlide(SliderBar* slider)
    {
        _contentPos.y = -slider->value();

        adjustScrollbar();
    }

    std::list<Widget::SharedPtr> ExpandGroup::items() const
    {
        std::list<Widget::SharedPtr> result;

        for (auto& ctrl : children()) {
            if(ctrl.get() != _verticalSlider && ctrl.get() != _horizonSlider) {
                result.push_back(ctrl);
            }
        }

        return result;
    }

    void ExpandGroup::update(float delta)
    {
    }

    void ExpandGroup::draw()
    {
        if (!visible())
        {
            return;
        }

        auto realPos = getRealPos();
        auto& renderer = GuiManager::inst().renderer();

        auto oldClipRect = renderer.getClipRect();
        if (clipChildren())
        {
            renderer.setClipRect({ realPos, _size });
        }

        Widget::draw();

        Rect paintRect{ realPos + Vec2{1,1}, _size - Vec2{2,2} };
        if (_horizonSlider->visible())
        {
            paintRect.h -= _horizonSlider->size().y;
        }

        if (_verticalSlider->visible())
        {
            paintRect.w -= _verticalSlider->size().x;
        }

        renderer.setClipRect(paintRect);

        for (auto& ptr : children())
        {
            ptr->draw();
        }

        renderer.setClipRect(oldClipRect);
    }

    void ExpandGroup::onChildAdded(Widget* child)
    {
        bringTop(_verticalSlider);

        bringTop(_horizonSlider);
     
        adjustContent();
    }

    void ExpandGroup::onChildRemoved(Widget* child)
    {
        adjustContent();
    }

    void ExpandGroup::onChildPosChanged(Widget* child)
    {
        adjustContent();
    }

    void ExpandGroup::onChildSizeChanged(Widget* child)
    {
        adjustContent();
    }

    void ExpandGroup::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
    {
        if (_horizonSlider)
        {
            float val = _horizonSlider->value() - offset.x;
            _horizonSlider->setValue(val);
        }

        if (_verticalSlider)
        {
            float val = _verticalSlider->value() - offset.y;
            _verticalSlider->setValue(val);
        }
    }

    void ExpandGroup::onMouseWheel(const Vec2& pos, float dir)
    {
        if (_verticalSlider)
        {
            float val = _verticalSlider->value() - dir * 10;
            _verticalSlider->setValue(val);
        }
    }

    void ExpandGroup::adjustScrollbar()
    {
        if (_horizonSlider)
        {
            Vec2 hspos{ -_contentPos.x, size().y - slider_bar_size - _contentPos.y };
            _horizonSlider->rawSetPos(hspos);

            float hxoffset = (_verticalSlider && _verticalSlider->visible()) ? slider_bar_size : 0;
            Vec2 hssize{ size().x - hxoffset, slider_bar_size };
            _horizonSlider->rawSetSize(hssize);
        }

        if (_verticalSlider)
        {
            Vec2 vspos{ size().x - slider_bar_size - _contentPos.x,  -_contentPos.y };
            _verticalSlider->rawSetPos(vspos);

            float vyoffset = (_horizonSlider && _horizonSlider->visible()) ? slider_bar_size : 0;
            Vec2 vssize{ slider_bar_size, size().y - vyoffset };
            _verticalSlider->rawSetSize(vssize);
        }
    }

    void ExpandGroup::adjustContent()
    {
        _contentSize = { 0, 0 };

        Vec2 contentRB = { 0, 0 };
        for (auto& ctrl : children())
        {
            if (ctrl->name() == "__h_slider__" || ctrl->name() == "__h_slider__")
            {
                continue;
            }

            if (ctrl->pos().x + ctrl->size().x > contentRB.x)
            {
                contentRB.x = ctrl->pos().x + ctrl->size().x;
            }

            if (ctrl->pos().y + ctrl->size().y > contentRB.y)
            {
                contentRB.y = ctrl->pos().y + ctrl->size().y;
            }
        }

        if (_horizonSlider && _horizonSlider->visible()) 
        {
            contentRB.x += _horizonSlider->size().x;
        }

        if (_verticalSlider && _verticalSlider->visible())
        {
            contentRB.y += _verticalSlider->size().y;
        }

        _contentSize = contentRB;

        if (_horizonSlider)
        {
            float hxoffset = (_verticalSlider && _verticalSlider->visible()) ? slider_bar_size : 0;
            Vec2 hssize{ size().x - hxoffset, slider_bar_size};
            Vec2 hspos{ 0, size().y - slider_bar_size};

            _horizonSlider->rawSetPos(hspos);
            _horizonSlider->rawSetSize(hssize);

            _horizonSlider->rawSetValue(_contentPos.x);
            _horizonSlider->rawSetMaxValue(_contentSize.x - size().x);
            _horizonSlider->rawSetVisible(_contentSize.x > size().x);
        }

        if (_verticalSlider)
        {
            float vyoffset = (_horizonSlider && _horizonSlider->visible()) ? slider_bar_size : 0;
            Vec2 vssize{ slider_bar_size, size().y - vyoffset};
            Vec2 vspos{ size().x - slider_bar_size, 0 };

            _verticalSlider->rawSetPos(vspos);
            _verticalSlider->rawSetSize(vssize);

            _verticalSlider->rawSetValue(_contentPos.x);
            _verticalSlider->rawSetMaxValue(_contentSize.y - size().y);
            _verticalSlider->rawSetVisible(_contentSize.y > size().y);
        }

        adjustScrollbar();
    }

    ///////////////////////////////////////////////////////////////////////////////////////

	HorizonalLayout::HorizonalLayout(const std::string& name, Widget* parent )
        : Group(name, parent)
    {
    }

    void HorizonalLayout::update(float delta) 
    {
    }

    void HorizonalLayout::adjustLayout()
    {
        float width_used = _padding.x * 2;

        std::vector<Widget*> scalable_widgets;

        for(auto& ptr : children())
        {
            if(!ptr->visible()) 
            {
                continue;
            }

            if(ptr->scaleInGroup())
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

            ptr->rawSetPos({width_used, _padding.y});

            if(ptr->scaleInGroup())
            {
                ptr->rawSetSize({scalable_width, scalable_height});
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
        child->setScaleInGroup(true);
        adjustLayout();
    }

    void HorizonalLayout::onChildRemoved(Widget* child) 
    {
        adjustLayout();
    }

    void HorizonalLayout::onChildPosChanged(Widget* child)
    {
        adjustLayout();
    }

    void HorizonalLayout::onChildSizeChanged(Widget* child) 
    {
        child->setScaleInGroup(false);
        adjustLayout();
    }

    void HorizonalLayout::onChildVisibleChanged(Widget* child) 
    {
        adjustLayout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////



	VerticalLayout::VerticalLayout(const std::string& name, Widget* parent)
        : HorizonalLayout(name, parent)
    {
    }

    void VerticalLayout::update(float delta)
    {
    }
    
    void VerticalLayout::adjustLayout()
    {
        float height_used = _padding.y * 2;

        std::vector<Widget*> scalable_widgets;

        for(auto& ptr : children())
        {
            if(!ptr->visible()) 
            {
                continue;
            }

            if(ptr->scaleInGroup())
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

            ptr->rawSetPos({_padding.x, height_used});

            if(ptr->scaleInGroup())
            {
                ptr->rawSetSize({scalable_width, scalable_height});
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