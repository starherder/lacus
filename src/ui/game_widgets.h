#pragma once

#include "ui/ui.h"

namespace ui
{

    class CardWidget : public Group
    {
    public:
        signal<CardWidget*> on_select;
        signal<CardWidget*> on_drag;

    public:
        CardWidget(const std::string& name, Widget* parent = nullptr);
        ~CardWidget();

        void setTitle(const std::string& title) { _title->setText(title); }
        void setDesc(const std::string& desc) { _desc->setText(desc); }
        void setLevel(int level) { _level->setText(std::to_string(level)); }

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



	class CardGroup : public Group
	{
    public:
        CardGroup(const std::string& name, Widget* parent = nullptr);
        ~CardGroup();

        CardWidget* addCard(const std::string& cfg);

        void removeCard(CardWidget*);

        void update(float delta) override;
        
        void draw() override;

        void setOverlap(bool enable);

    private:
        void adjustChildren();

        void overlapChildren();

        void tileChildren();

    private:
        ui::Coordinate _coord = ui::Coordinate::Horizonal;

        Vec2 _padding = {10.0f, 10.0f};

        float _space = 10.0f;

        bool _overlap = true;
	};


}