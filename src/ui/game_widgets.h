#pragma once

#include "ui/ui.h"

namespace ui
{

    class CardWidget : public Group
    {
    public:
        signal<CardWidget*, bool> on_select;
        signal<CardWidget*> on_drag;

        signal<CardWidget*> on_mouse_enter;
        signal<CardWidget*> on_mouse_leave;

    public:
        CardWidget(const std::string& name, Widget* parent = nullptr);
        ~CardWidget();

        bool selected() { return _state == WidgetState::Selected; }

        void setTitle(const std::string& title) { _title->setText(title); }
        void setDesc(const std::string& desc) { _desc->setText(desc); }
        void setLevel(int level) { _level->setText(std::to_string(level)); }

        auto getCfgid() { return getData<std::string>("cfgid"); }
        auto getTitle() { return _title->text(); }
        auto getDesc() { return _desc->text(); }
        auto getLevel() { return std::stoi(_level->text()); }

        virtual void update(float delta);
        virtual void draw();

    private:
        void onMouseEnter(const Vec2& pos) override;
        void onMouseLeave(const Vec2& pos) override;
        
        void onMouseLeftDown(const Vec2& pos) override;
        void onMouseLeftUp(const Vec2& pos) override;

        void onMouseLeftClick(const Vec2& pos) override;
        void onMouseLeftDrag(const Vec2& pos, const Vec2& offset) override;

        WidgetState state() { return _state; }

        virtual WidgetStatus& status() override;

        void setState(WidgetState state);

    protected:

        float DefaultBorderRound = 10.0f;
        float DfaultBorderSize = 5.0f;

        Vec2 DefaultSize = { 250, 300 };

        Label* _title = nullptr;
        Label* _level = nullptr;
        TextBox* _desc = nullptr;
        Button* _info = nullptr;

        WidgetState _state = WidgetState::Normal;

        std::map<WidgetState, WidgetStatus> _status;
    };

	class CardGroup : public Group, public utility::sigslot::SlotHandler
	{
    public:
        CardGroup(const std::string& name, Widget* parent = nullptr);
        ~CardGroup();

        CardWidget* addCard(const Properties& props);

        void removeCard(CardWidget*);

        void update(float delta) override;
        
        void draw() override;

        bool isOverlap() { return _overlap; }
        void setOverlap(bool enable);

        const Vec2& padding() { return _padding; }
        void setPadding(const Vec2& padding);

        float spacing() { return _space; }
        void setSpacing(float space);

        Coordinate getCoordinate() { return _coord; }
        void setCoordinate(Coordinate coord);

    private:
        void adjustChildren();

        void overlapChildren();

        void tileChildren();

        void onChildAdded(Widget* child) override;

        void onChildRemoved(Widget* child) override;

        void onSizeChanged(const Vec2& oldPos, const Vec2& newPos) override;

        void onChildSelect(CardWidget* card, bool selected);
        
        void onChildDrag(CardWidget* card);

        void onChildMouseEnter(CardWidget* card);

        void onChildMouseLeave(CardWidget* card);


        bool onLoad(XmlNode* node);

    private:
        ui::Coordinate _coord = ui::Coordinate::Horizonal;

        Vec2 _padding = {10.0f, 10.0f};

        float _space = 10.0f;

        bool _overlap = false;
	};


}