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

        bool onLoad(XmlNode* node) override;

        bool selected() { return _state == WidgetState::Selected; }

        void setTitle(const std::string& title);
        void setDesc(const std::string& desc);
        void setLevel(int level);
        void setIcon(const std::string& icon);

        std::string getCfgid();
        std::string getTitle();
        std::string getDesc();
        int getLevel();

    private:
        void onMouseEnter(const Vec2& pos) override;
        void onMouseLeave(const Vec2& pos) override;
        
        void onMouseLeftDown(const Vec2& pos) override;
        void onMouseLeftUp(const Vec2& pos) override;

        void onMouseLeftClick(const Vec2& pos) override;
        void onMouseLeftDrag(const Vec2& pos, const Vec2& offset) override;

        void onSizeChanged(const Vec2& oldPos, const Vec2& newPos) override;

        WidgetState state() { return _state; }

        virtual WidgetStatus& status() override;

        void setState(WidgetState state);

        void adjust();

    protected:
        float DefaultBorderRound = 10.0f;
        float DfaultBorderSize = 5.0f;

        Vec2 DefaultSize = { 250, 300 };

        Label* _title = nullptr;
        Label* _level = nullptr;
        Label* _icon = nullptr;
        TextBox* _desc = nullptr;

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

        XmlNode* _tempCardNode = nullptr;
	};


}