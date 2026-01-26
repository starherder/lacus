#include "game_widgets.h"


namespace ui
{
    DeclareWidgetType(CardWidget, "card");
    DeclareWidgetType(CardGroup, "card_group");

    using namespace engine;

    const Vec2 CardTitlePos = {30, 30};
    const float CardTitleFontSize = 20;
    const Color CardTitleColor = Color::Dark;

    const Vec2 CardDescPos = { 50, 100 };
    const float CardDescFontSize = 14;
    const Color CardDescColor = Color::Light;

    CardWidget::CardWidget(const std::string& name, Widget* parent) : Group(name, parent)
    {
        setState(WidgetState::Normal);

        _status[WidgetState::Normal] = WigetUtils::normalStatus;
        _status[WidgetState::Hover] = WigetUtils::hoveredStatus;
        _status[WidgetState::Selected] = WigetUtils::selectedStatus;
        _status[WidgetState::Disabled] = WigetUtils::disabledStatus;

        setBorderRound(DefaultBorderRound);

        setBorderSize(DfaultBorderSize);

        setSize(DefaultSize);

        setMovable(true);

        setAcceptEvent(true);

        setCanDragOut(true);

        _info = createChild<Button>("_info_");
        _info->setSize({30, 30});
        _info->setPos({200, 10});

        _title = createChild<Label>("_title_");
        _title->setPos({20, 50});
        _title->setSize({140, 50});
        _title->setFont("fonts/msyh.ttf", 25);
        _title->setTextAlign(Align::Left);
        _title->setTextColor(Color::Dark);

        _level = createChild<Label>("_level_");
        _level->setPos({170, 50});
        _level->setSize({60, 50});

        _desc = createChild<TextBox>("_desc_");
        _desc->setPos({20, 110});
        _desc->setSize({210, 180});
    }

    CardWidget::~CardWidget()
    {
    }

    void CardWidget::update(float delta)
    {
        Group::update(delta);
    }

    void CardWidget::draw()
    {
        Group::draw();
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

    WidgetStatus& CardWidget::status()
    {
        if (_state < WidgetState::Normal || _state >= WidgetState::Max)
            return Widget::status();
        return _status[_state];
    }

    /////////////////////////////////////////////////////////////////

    CardGroup::CardGroup(const std::string& name, Widget* parent) : Group(name, parent)
    {
        setClipChildren(false);
    }

    CardGroup::~CardGroup()
    {
    }

    void CardGroup::setOverlap(bool enable)
    {
        _overlap = enable;

        adjustChildren();
    }
    
    CardWidget* CardGroup::addCard(const Properties& props)
    {
        auto cfgid = props["cfgid"].convert<std::string>();
        auto name = props["name"].convert<std::string>();
        auto desc = props["desc"].convert<std::string>();

        auto lvopt = props.get<int>("level");
        int level = lvopt ? lvopt.value() : 0;

        int index = (int)children().size();
        auto widget = createChild<CardWidget>(name);
        widget->setTitle(name);
        widget->setLevel(level);
        widget->setDesc(desc);
        widget->setData("cfgid", cfgid);
        widget->setData("index", index);
     
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

    void CardGroup::update(float delta)
    {
        Group::update(delta);
    }

    void CardGroup::draw()
    {
        Group::draw();
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
        if (_coord == Coordinate::Horizonal) 
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
            float x = (_coord == Coordinate::Horizonal) ? _padding.x + index * (_space + card->size().x) : _padding.x;
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

    bool CardGroup::onLoad(XmlNode* node)
    {
        if (!node) {
            return false;
        }

        adjustChildren();
        return true;
    }
}