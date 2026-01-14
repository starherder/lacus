#include "game_widgets.h"


namespace ui
{
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

        setDragable(true);

        setNoEvent(false);

        setCanDragOut(true);

        auto bgVLay = createChild<VLayout>("_bg_vlay_");
        bgVLay->setSize(DefaultSize - Vec2{4,4});
        bgVLay->setPos({ 2,2 });

        auto headHLay = bgVLay->createChild<HLayout>("_hlay_head_");
        _info = headHLay->createChild<Button>("_info_");

        auto titleHLay = bgVLay->createChild<HLayout>("_hlay_title_");
        _title = titleHLay->createChild<Label>("_title_");

        _level = titleHLay->createChild<Label>("_level_");
        _level->setSize({100, 50});

        _desc = bgVLay->createChild<TextBox>("_desc_");
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
    
    CardWidget* CardGroup::addCard(const std::string& cfg)
    {
        int index = (int)children().size();
        auto name = fmt::format("_card_{}_", index);

        auto widget = createChild<CardWidget>(name);
        widget->setTitle(name);
        widget->setLevel(index*10);

        auto desc = fmt::format("this is test card-{}", index);
        widget->setDesc(desc);

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
            for (auto& card : children())
            {
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
}