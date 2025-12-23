#pragma once

#include "ui_utils.h"
#include "widget.h"

namespace ui {


class Label : public Widget
{
public:
    Label() = delete;
    Label(const std::string& name, Widget* parent = nullptr);
    ~Label() = default;

    auto& textPadding() const { return _textPadding; }
    void setTextPadding(const Vec2& pad) { _textPadding = pad; }

    const auto& text() { return _text; }
    void setText(const std::string& text) { _text = text; }

    const auto& textColor() { return status().text_color; }
    void setTextColor(const Color& color) { status().text_color = color; }

    float fontSize() { return _fontSize; }
    const auto& fontName() { return _fontName; }

    Align textAlign() { return _textAlign; }
    void setTextAlign(Align align) { _textAlign = align; }

    void setFont(const std::string& name, int size);

    void draw() override;

private:
    void adjust();

private:
    static const float DefaultFontSize;
    static const std::string DefaultFontName;

    std::string _text;
    
    Vec2 _textPadding = {0, 0};
    Align _textAlign = Align::Center;

    std::string _fontName;
    float _fontSize = 20;
    Font* _font = nullptr;
};


class Button : public Label
{
public:
    signal<Button*> on_click;

public:
	Button(const std::string& name, Widget* parent = nullptr);
    ~Button();

    void update(float delta) override;
    void draw() override;

    void onMouseEnter() override;
    void onMouseLeave() override;

    void onMouseLeftClick() override;

    void onMouseLeftDown()override;
    void onMouseLeftUp()override;

    WidgetState state() { return _state; }

    virtual WidgetStatus& status() override;

private:
    void setState(WidgetState state);

private:
    WidgetState _state = WidgetState::Normal;

    std::map<WidgetState, WidgetStatus> _status;
};




}