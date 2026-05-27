#include "game_widgets.h"


namespace ui
{
    DeclareWidgetType(CardWidget, "card");
    DeclareWidgetType(CardGroup, "card_group");

    using namespace engine;


    CardWidget::CardWidget(const std::string& name, Widget* parent) : Group(name, parent)
    {
        _status[WidgetState::Normal] = WidgetUtils::normalStatus;
        _status[WidgetState::Hover] = WidgetUtils::hoveredStatus;
        _status[WidgetState::Selected] = WidgetUtils::selectedStatus;
        _status[WidgetState::Disabled] = WidgetUtils::disabledStatus;

        setState(WidgetState::Normal);

        setBorderRound(DefaultBorderRound);

        setBorderSize(DfaultBorderSize);

        setSize(DefaultSize);

        setMovable(true);

        setAcceptEvent(true);

        setCanDragOut(true);
    }

    CardWidget::~CardWidget()
    {
    }

    bool CardWidget::onLoad(XmlNode* node)
    {
        if (!node) 
        { 
            return false; 
        }

        if (!Group::onLoad(node))
        {
            return false;
        }

        adjust();

        _title = getChild<Label>("lbl_name", true);
        _level = getChild<Label>("lbl_level", true);
        _icon = getChild<Label>("lbl_icon", true);
        _desc = getChild<TextBox>("txt_desc", true);

        return true;
    }

    void CardWidget::setState(WidgetState state)
    {
        _state = state;
    }

    void CardWidget::onMouseEnter(const Vec2& mpos)
    {
        on_mouse_enter.emit(this);

        setState(WidgetState::Hover);
    }

    void CardWidget::onMouseLeave(const Vec2& mpos)
    {
        on_mouse_leave.emit(this);

        setState(WidgetState::Normal);
    }

    void CardWidget::onMouseLeftClick(const Vec2& mpos)
    {
        if (_state != WidgetState::Selected)
        {
            on_select.emit(this, true);
            setState(WidgetState::Selected);
        }
    }

    void CardWidget::onMouseLeftDrag(const Vec2& mpos, const Vec2& offset)
    {
        on_drag.emit(this);
    }

    void CardWidget::onMouseLeftDown(const Vec2& mpos)
    {
    }

    void CardWidget::onMouseLeftUp(const Vec2& mpos)
    {
    }

    void CardWidget::adjust()
    {
        if (!children().empty())
        {
            auto& child = children().front();
            if (child)
            {
                child->rawSetPos({ 0, 0 });
                child->rawSetSize(size());
            }
        }
    }

    WidgetStatus& CardWidget::status()
    {
        if (_state < WidgetState::Normal || _state >= WidgetState::Max)
            return Widget::status();
        return _status[_state];
    }

    void CardWidget::setTitle(const std::string& title) 
    {
        if(_title) _title->setText(title); 
    }
    
    void CardWidget::setDesc(const std::string& desc) 
    { 
        if(_desc) _desc->setText(desc); 
    }
    
    void CardWidget::setLevel(int level) 
    { 
        if(_level) _level->setText(std::to_string(level)); 
    }
    
    void CardWidget::setIcon(const std::string& icon) 
    { 
        if (_icon) _icon->setTexTile(icon);
    }

    std::string CardWidget::getCfgid() 
    { 
        return getData<std::string>("cfgid"); 
    }

    std::string CardWidget::getTitle() 
    { 
        return _title ? _title->text() : "";
    }

    std::string CardWidget::getDesc() 
    { 
        return _desc ? _desc->text() : "";
    }
    
    int CardWidget::getLevel() 
    { 
        return _level ? std::stoi(_level->text()) : 0; 
    }
    
    void CardWidget::onSizeChanged(const Vec2& oldPos, const Vec2& newPos)
    {
        adjust();
    }


    /////////////////////////////////////////////////////////////////

    CardGroup::CardGroup(const std::string& name, Widget* parent) : Group(name, parent)
    {
        setClipChildren(false);
    }

    CardGroup::~CardGroup()
    {
    }

    CardWidget* CardGroup::addCard(const Properties& props)
    {
        auto cfgid = props["cfgid"].convert<std::string>();
        auto name = props["name"].convert<std::string>();
        auto desc = props["desc"].convert<std::string>();
        auto icon = props["icon"].convert<std::string>();

        auto lvopt = props.get<int>("level");
        int level = lvopt ? lvopt.value() : 0;

        auto widget = createChild<CardWidget>(name);
        widget->load(_tempCardNode);
        
        widget->setTitle(name);
        widget->setLevel(level);
        widget->setDesc(desc);
        widget->setIcon(icon);
        widget->setData("cfgid", cfgid);
        widget->setData("index", (int)children().size());
     
        widget->on_drag.connect(this, &CardGroup::onChildDrag);
        widget->on_select.connect(this, &CardGroup::onChildSelect);
        widget->on_mouse_enter.connect(this, &CardGroup::onChildMouseEnter);
        widget->on_mouse_leave.connect(this, &CardGroup::onChildMouseLeave);

        adjustChildren();
        return widget;
    }

    void CardGroup::removeCard(CardWidget* cardWidget)
    {
        for (auto& widget : children()) {
            if (widget.get() == cardWidget)
            {
                removeChild(widget->name());
                
                adjustChildren();
                return;
            }
        }
    }

    void CardGroup::onChildSelect(CardWidget* card, bool selected)
    {
        if(selected)
        {
            adjustChildren();
        }
    }

    void CardGroup::onChildDrag(CardWidget* card)
    {
    }

    void CardGroup::onChildMouseEnter(CardWidget* card)
    {
        auto wpos = card->pos();
        wpos.y = -20;
        card->setPos(wpos);
    }
    
    void CardGroup::onChildMouseLeave(CardWidget* card)
    {
        auto wpos = card->pos();
        wpos.y = padding().y;
        card->setPos(wpos);
    }

    void CardGroup::adjustChildren()
    {
        if (_overlap) 
        {
            overlapChildren();
        }
        else 
        {
            tileChildren();
        }
    }

    void CardGroup::overlapChildren()
    {
        if (_coord == Coordinate::Horizontal) 
        {
            float total = 0.0f;
            for (auto& card : children()) 
            {
                total += card->size().x;
            }

            float over = float(total - size().x) / children().size();

            auto pos = _padding;
            for (auto& widget : children())
            {
                auto card = dynamic_cast<CardWidget*>(widget.get());
                if(!card)
                {
                    continue;
                }

                card->setPos(pos);

                pos.x += card->size().x - over;
            }
        }
        else 
        {
            float total = 0.0f;
            for (auto& card : children()) 
            {
                total += card->size().y;
            }

            float over = float(total - size().y) / children().size();

            auto pos = _padding;
            for (auto& card : children()) 
            {
                card->setPos(pos);
                pos.y += card->size().y - over;
            }
        }
    }

    void CardGroup::tileChildren()
    {
        int index = 0;
        for (auto& card : children()) 
        {
            float x = (_coord == Coordinate::Horizontal) ? _padding.x + index * (_space + card->size().x) : _padding.x;
            float y = (_coord == Coordinate::Vertical) ? _padding.y + index * (_space + card->size().y) : _padding.y;

            card->setPos({x, y});
            index++;
        }
    }

    void CardGroup::onChildAdded(Widget* child)
    {
        adjustChildren();
    }

    void CardGroup::onChildRemoved(Widget* child)
    {
        adjustChildren();
    }

    void CardGroup::setOverlap(bool enable)
    {
        _overlap = enable;

        adjustChildren();
    }

    void CardGroup::setPadding(const Vec2& padding)
    {
        _padding = padding;

        adjustChildren();
    }

    void CardGroup::setSpacing(float space)
    {
        _space = space;

        adjustChildren();
    }

    void CardGroup::setCoordinate(Coordinate coord)
    {
        _coord = coord;

        adjustChildren();
    }

    void CardGroup::onSizeChanged(const Vec2& oldPos, const Vec2& newPos)
    {
        adjustChildren();
    }

    bool CardGroup::onLoad(XmlNode* node)
    {
        if (!node || !Widget::onLoad(node)) 
        {
            return false;
        }

        _tempCardNode = node->FirstChildElement("card");

        _coord = WidgetUtils::getCoord(node->Attribute("coordinate"));
        
        _padding = ToVec2(node->Attribute("pad"));

        _space = node->FloatAttribute("space");

        _overlap = node->BoolAttribute("overlap", true);
        
        adjustChildren();

        return true;
    }
}