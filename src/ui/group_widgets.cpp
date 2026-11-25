#include "group_widgets.h"
#include "base_widgets.h"

#include "gui_manager.h"
#include "widget.h"

#include <algorithm>

namespace ui 
{

    DeclareWidgetType(Group, "group");
    DeclareWidgetType(ExpandGroup, "exgroup");
    DeclareWidgetType(GridGroup, "gridgroup");
    DeclareWidgetType(HLayout, "hlayout");
    DeclareWidgetType(VLayout, "vlayout");

    struct GridGroupGrids_Creator : public WidgetCreator
    {
        const char* typeName() override { return "grids"; }
        WidgetPtr create(const std::string& name, Widget* widget) override
        {
            return std::make_shared<GridGroup>(name, widget);
        }
    };

    struct GridGroupGrids_Initializer
    {
        GridGroupGrids_Initializer()
        {
            ui::GuiManager::inst().addWidgetCreator<GridGroupGrids_Creator>();
        }
        ~GridGroupGrids_Initializer() = default;
    } g_GridGroupGrids_Initializer;

	Group::Group(const std::string& name, Widget* parent) : Widget(name, parent)
    {
    }

    Group::~Group()
    {
    }

    bool Group::onLoad(XmlNode* node)
    {
        _clipChildren = node->BoolAttribute("clip_children", true);
        _padding = WidgetUtils::parseVec2OrMetric(node->Attribute("pad"), _padding);
        _padding = WidgetUtils::parseVec2OrMetric(node->Attribute("padding"), _padding);

        auto child = node->FirstChildElement();
        while (child)
        {
            auto widget = ui::GuiManager::inst().createWidget(child->Name(), this);
            if (widget)
            {
                widget->load(child);
            }
            else
            {
                LogError("create widget: {} failed.", child->Name());
            }

            child = child->NextSiblingElement();
        }

        return true;
    }

    void Group::update(float delta)
    {
        if(!visible()) 
        {
            return;
        }

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

        bool pushedClip = clipChildren();
        if(pushedClip)
        {
            GuiManager::inst().painter().pushClipRect(GuiManager::inst().uiToScreenRect(getClipRect()));
        }

        Widget::draw();

        for(auto& ptr : _children) 
        {
            ptr->draw();
        }

        if (pushedClip)
        {
            GuiManager::inst().painter().popClipRect();
        }
    } 
    
    void Group::setPadding(const Vec2& padding)
    {
        auto oldPadding = _padding;
        if (oldPadding == padding)
        {
            return;
        }

        _padding = padding;
        onPaddingChanged(oldPadding, _padding);
    }

    void Group::addWidget(WidgetPtr widget, int index)
    {
        if(!widget)
        {
            LogError("Group::addWidget widiget==nullptr.");
            return;
        }

        if(index < 0 || index >= _children.size())
        {
            index = -1;
        }

        bool addOK = false;
        if (index == -1) 
        {
            _children.push_back(widget);
            addOK = true;
        }
        else
        {
            if (index >= 0 && index < _children.size())
            {
                int curindex = 0;
                for (auto it = _children.begin(); it != _children.end(); it++)
                {
                    if (curindex == index) 
                    {
                        _children.insert(it, widget);
                    
                        addOK = true;
                        break;
                    }

                    curindex++;
                }
            }
        }

        if (addOK) 
        {
            widget->setParent(this);
            onChildAdded(widget.get());
        }
        else
        {
            LogError("add widget ({}) to index {} failed. child_count = {}", widget->name(), index, _children.size());
        }
    }

    void Group::removeChild(const std::string& name)
    {
        for(auto it=_children.begin(); it!=_children.end(); it++)
        {
            if ((*it)->name() != name)
            {
                continue;
            }
            auto ptr = *it;
            onChildRemoved(ptr.get());

            _children.erase(it);
            return;
        }
    }

    void Group::clear()
    {
        _children.clear();
    }

    void Group::bringTop(Widget* child)
    {
        for (auto it = _children.begin(); it != _children.end(); it++)
        {
            auto ptr = *it;
            if (ptr.get() == child) 
            {
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
            if (ptr.get() == child) 
            {
                _children.erase(it);
                _children.push_front(ptr);
                return;
            }
        }
    }

    Rect Group::getClipRect() const
    {
        auto parentWidget = parent();
        auto parentGroup = dynamic_cast<Group*>(parentWidget);
        if (!parentGroup)
        {
            return getAbsRect();
        }

        if (clipChildren())
        {
            return parentGroup->getClipRect().intersect(getAbsRect());
        }

        return parentGroup->getClipRect();
    }

    WidgetPtr Group::moveOut(Widget* child)
    {
        for (auto it = _children.begin(); it != _children.end(); it++)
        {
            auto ptr = *it;
            if (ptr.get() == child) 
            {
                _children.erase(it);

                ptr->setParent(nullptr);
                return ptr;
            }
        }

        return nullptr;
    }

    void Group::onChildAdded(Widget* child)
    {
    }

    void Group::onChildRemoved(Widget* child)
    {
    }

    void Group::onPaddingChanged(const Vec2& oldPadding, const Vec2& newPadding)
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
    }
    
    void Group::onSizeChanged(const Vec2& oldPos, const Vec2& newPos)
    {
    }

    void Group::onVisibleChanged(bool oldVisual, bool newVisual)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    BackGroup::BackGroup(const std::string& name, Widget* parent) : Group(name, parent)
    {
    }

    BackGroup::~BackGroup()
    {
    }

    void BackGroup::adjustChildren()
    {
        for (auto& child : children())
        {
            child->setSize(size());
        }
    }

    bool BackGroup::onLoad(XmlNode* node)
    {
        if (!Group::onLoad(node))
        {
            return false;
        }

        adjustChildren();
        return true;
    }

    void BackGroup::onChildAdded(Widget* child)
    {
        adjustChildren();
    }

    void BackGroup::onChildRemoved(Widget* child)
    {
    }

    void BackGroup::onSizeChanged(const Vec2& oldPos, const Vec2& newPos)
    {
        adjustChildren();
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    ExpandGroup::ExpandGroup(const std::string& name, Widget* parent) : Group(name, parent)
    {
        _padding = { 20.0f, 20.0f };
        _contentPos = {0, 0};
        _contentSize = size();

        setMovable(true);

        setAcceptEvent(true);

        _horizonSlider = createChild<ui::SliderBar>("__h_slider__");
        _horizonSlider->setDirection(ui::Coordinate::Horizontal);
        _horizonSlider->on_value_changed.connect(this, &ExpandGroup::onHorizontalSlide);

        _verticalSlider = createChild<ui::SliderBar>("__v_slider__");
        _verticalSlider->setDirection(ui::Coordinate::Vertical);
        _verticalSlider->on_value_changed.connect(this, &ExpandGroup::onVerticalSlide);

        adjustScrollbar();
    }

    ExpandGroup::~ExpandGroup()
    {
    }

    bool ExpandGroup::onLoad(XmlNode* node)
    {
        if (!Group::onLoad(node))
        {
            return false;
        }

        adjustContent();
        return true;
    }

    bool ExpandGroup::isContentWidget(Widget* child) const
    {
        return child && child != _horizonSlider && child != _verticalSlider;
    }

    void ExpandGroup::onHorizontalSlide(SliderBar* slider)
    {
        _contentPos.x = -slider->value();

        adjustScrollbar();
    }

    void ExpandGroup::onVerticalSlide(SliderBar* slider)
    {
        _contentPos.y = -slider->value();

        adjustScrollbar();
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
            if (!isContentWidget(ctrl.get()))
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

        contentRB += getContentEndPadding();

        _contentSize = contentRB;

        bool showHorizontal = false;
        bool showVertical = false;
        Vec2 viewportSize = size();
        for (int i = 0; i < 2; ++i)
        {
            viewportSize = {
                size().x - (showVertical ? slider_bar_size : 0.0f),
                size().y - (showHorizontal ? slider_bar_size : 0.0f)
            };

            bool needHorizontal = _contentSize.x > viewportSize.x;
            bool needVertical = _contentSize.y > viewportSize.y;
            if (needHorizontal == showHorizontal && needVertical == showVertical)
            {
                break;
            }

            showHorizontal = needHorizontal;
            showVertical = needVertical;
        }

        viewportSize = {
            size().x - (showVertical ? slider_bar_size : 0.0f),
            size().y - (showHorizontal ? slider_bar_size : 0.0f)
        };

        float maxHorizontal = std::max(0.0f, _contentSize.x - viewportSize.x);
        float maxVertical = std::max(0.0f, _contentSize.y - viewportSize.y);

        _contentPos.x = showHorizontal ? std::clamp(_contentPos.x, -maxHorizontal, 0.0f) : 0.0f;
        _contentPos.y = showVertical ? std::clamp(_contentPos.y, -maxVertical, 0.0f) : 0.0f;

        if (_horizonSlider)
        {
            float hxoffset = showVertical ? slider_bar_size : 0;
            Vec2 hssize{ size().x - hxoffset, slider_bar_size};
            Vec2 hspos{ 0, size().y - slider_bar_size};

            _horizonSlider->rawSetPos(hspos);
            _horizonSlider->rawSetSize(hssize);

            if (showHorizontal)
            {
                _horizonSlider->rawSetMaxValue(maxHorizontal);
                _horizonSlider->setValue(-_contentPos.x);
            }
            else
            {
                _horizonSlider->rawSetValue(0.0f);
            }

            _horizonSlider->rawSetVisible(showHorizontal);
        }

        if (_verticalSlider)
        {
            float vyoffset = showHorizontal ? slider_bar_size : 0;
            Vec2 vssize{ slider_bar_size, size().y - vyoffset};
            Vec2 vspos{ size().x - slider_bar_size, 0 };

            _verticalSlider->rawSetPos(vspos);
            _verticalSlider->rawSetSize(vssize);

            if (showVertical)
            {
                _verticalSlider->rawSetMaxValue(maxVertical);
                _verticalSlider->setValue(-_contentPos.y);
            }
            else
            {
                _verticalSlider->rawSetValue(0.0f);
            }

            _verticalSlider->rawSetVisible(showVertical);
        }

        adjustScrollbar();
    }

    void ExpandGroup::onSizeChanged(const Vec2& oldPos, const Vec2& newPos)
    {
        Group::onSizeChanged(oldPos, newPos);

        adjustContent();
    }

    std::vector<Widget*> ExpandGroup::items()
    {
        std::vector<Widget*> result;
        for(auto child : children()) {
            if(isContentWidget(child.get())) {
                result.push_back(child.get());
            }
        }
        return result;
    }
    ///////////////////////////////////////////////////////////////////////////////////////

    GridGroup::GridGroup(const std::string& name, Widget* parent)
        : ExpandGroup(name, parent)
    {
        _contentEndPadding = _padding;
    }

    void GridGroup::setSpacing(const Vec2& spacing)
    {
        _spacing = spacing;
        adjustLayout();
    }

    void GridGroup::setRows(int rows)
    {
        _rows = std::max(1, rows);
        adjustLayout();
    }

    void GridGroup::setColumns(int columns)
    {
        _columns = std::max(1, columns);
        adjustLayout();
    }

    bool GridGroup::onLoad(XmlNode* node)
    {
        if (!ExpandGroup::onLoad(node))
        {
            return false;
        }

        _spacing = WidgetUtils::parseVec2OrMetric(node->Attribute("space"), _spacing);
        _spacing = WidgetUtils::parseVec2OrMetric(node->Attribute("spacing"), _spacing);

        _rows = std::max(1, node->IntAttribute("row", _rows));
        _rows = std::max(1, node->IntAttribute("rows", _rows));

        _columns = std::max(1, node->IntAttribute("col", _columns));
        _columns = std::max(1, node->IntAttribute("cols", _columns));
        _columns = std::max(1, node->IntAttribute("columns", _columns));

        adjustLayout();
        return true;
    }

    bool GridGroup::isContentWidget(Widget* child) const
    {
        return ExpandGroup::isContentWidget(child);
    }

    void GridGroup::onPaddingChanged(const Vec2& oldPadding, const Vec2& newPadding)
    {
        adjustLayout();
    }

    void GridGroup::adjustLayout()
    {
        auto widgets = items();

        Vec2 cellSize = { 0.0f, 0.0f };
        std::vector<Widget*> visibleWidgets;
        for (auto widget : widgets)
        {
            if (!widget || !widget->visible())
            {
                continue;
            }

            visibleWidgets.push_back(widget);
            cellSize.x = std::max(cellSize.x, widget->size().x);
            cellSize.y = std::max(cellSize.y, widget->size().y);
        }

        if (visibleWidgets.empty())
        {
            _contentEndPadding = _padding;
            ExpandGroup::adjustContent();
            return;
        }

        int index = 0;
        int columns = std::max(1, _columns);
        Vec2 contentRB = { 0.0f, 0.0f };
        for (auto widget : visibleWidgets)
        {
            int row = index / columns;
            int col = index % columns;

            Vec2 pos = {
                _padding.x + col * (cellSize.x + _spacing.x),
                _padding.y + row * (cellSize.y + _spacing.y)
            };
            widget->rawSetPos(pos);

            contentRB.x = std::max(contentRB.x, pos.x + widget->size().x);
            contentRB.y = std::max(contentRB.y, pos.y + widget->size().y);

            index++;
        }

        int contentRows = std::max(_rows, (int)((visibleWidgets.size() + columns - 1) / columns));
        Vec2 desiredContentSize = {
            _padding.x * 2.0f + columns * cellSize.x + (columns - 1) * _spacing.x,
            _padding.y * 2.0f + contentRows * cellSize.y + (contentRows - 1) * _spacing.y
        };
        _contentEndPadding = {
            std::max(_padding.x, desiredContentSize.x - contentRB.x),
            std::max(_padding.y, desiredContentSize.y - contentRB.y)
        };

        ExpandGroup::adjustContent();
    }

    void GridGroup::onChildAdded(Widget* child)
    {
        ExpandGroup::onChildAdded(child);
        adjustLayout();
    }

    void GridGroup::onChildRemoved(Widget* child)
    {
        ExpandGroup::onChildRemoved(child);
        adjustLayout();
    }

    void GridGroup::onChildPosChanged(Widget* child)
    {
        if (!isContentWidget(child))
        {
            ExpandGroup::onChildPosChanged(child);
            return;
        }

        adjustLayout();
    }

    void GridGroup::onChildSizeChanged(Widget* child)
    {
        if (!isContentWidget(child))
        {
            ExpandGroup::onChildSizeChanged(child);
            return;
        }

        adjustLayout();
    }

    void GridGroup::onChildVisibleChanged(Widget* child)
    {
        if (isContentWidget(child))
        {
            adjustLayout();
        }
    }

    void GridGroup::onSizeChanged(const Vec2& oldPos, const Vec2& newPos)
    {
        ExpandGroup::onSizeChanged(oldPos, newPos);
        adjustLayout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////

	HorizontalLayout::HorizontalLayout(const std::string& name, Widget* parent )
        : Group(name, parent)
    {
    }

    bool HorizontalLayout::onLoad(XmlNode* node)
    {
        if (!Group::onLoad(node))
        {
            return false;
        }

        _spacing = WidgetUtils::parseMetric(node->Attribute("space"), 10.0f);

        adjustLayout();
        return true;
    }

    void HorizontalLayout::adjustLayout()
    {
        float width_used = _padding.x * 2;
        float scalable_height = size().y - _padding.y * 2;

        std::vector<Widget*> scalable_widgets;

        for(auto& ptr : children())
        {
            if(!ptr->visible()) 
            {
                continue;
            }

            ptr->rawSetSize({ptr->size().x, scalable_height });

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

    void HorizontalLayout::onChildAdded(Widget* child) 
    {
        child->setScaleInGroup(true);
        adjustLayout();
    }

    void HorizontalLayout::onChildRemoved(Widget* child) 
    {
        adjustLayout();
    }

    void HorizontalLayout::onPaddingChanged(const Vec2& oldPadding, const Vec2& newPadding)
    {
        adjustLayout();
    }

    void HorizontalLayout::onChildPosChanged(Widget* child)
    {
        adjustLayout();
    }

    void HorizontalLayout::onChildSizeChanged(Widget* child) 
    {
        if(child->size().x > 0)
        {
            child->setScaleInGroup(false);
        }

        adjustLayout();
    }

    void HorizontalLayout::onChildVisibleChanged(Widget* child) 
    {
        adjustLayout();
    }

    void HorizontalLayout::onSizeChanged(const Vec2& oldPos, const Vec2& newPos)
    {
        Group::onSizeChanged(oldPos, newPos);

        adjustLayout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////

	VerticalLayout::VerticalLayout(const std::string& name, Widget* parent)
        : HorizontalLayout(name, parent)
    {
    }

    bool VerticalLayout::onLoad(XmlNode* node)
    {
        if (!HorizontalLayout::onLoad(node))
        {
            return false;
        }

        adjustLayout();
        return true;
    }

    void VerticalLayout::adjustLayout()
    {
        float height_used = _padding.y * 2;
        float scalable_width = size().x - _padding.x * 2;

        std::vector<Widget*> scalable_widgets;

        for(auto& ptr : children())
        {
            if(!ptr->visible()) 
            {
                continue;
            }

            ptr->rawSetSize({scalable_width, ptr->size().y});

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

    void VerticalLayout::onChildSizeChanged(Widget* child)
    {
        if (child->size().y > 0)
        {
            child->setScaleInGroup(false);
        }

        adjustLayout();
    }

}
