#pragma once

#include "ui_utils.h"
#include "widget.h"
#include "groups.h"

namespace ui {


///////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////

class Button : public Label
{
public:
    signal<Button*> on_click;

public:
	Button(const std::string& name, Widget* parent = nullptr);
    ~Button();

    void onMouseEnter(const Vec2& pos) override;
    void onMouseLeave(const Vec2& pos) override;

    void onMouseLeftClick(const Vec2& pos) override;

    void onMouseLeftDown(const Vec2& pos) override;
    void onMouseLeftUp(const Vec2& pos) override;

    WidgetState state() { return _state; }

    virtual WidgetStatus& status() override;

private:
    void setState(WidgetState state);

private:
    WidgetState _state = WidgetState::Normal;

    std::map<WidgetState, WidgetStatus> _status;
};


///////////////////////////////////////////////////////////////////////

class ProgressBar : public Group
{
public: 
    signal<ProgressBar*> on_process_changed;    

public:
    ProgressBar() = delete;
    ProgressBar(const std::string& name, Widget* parent = nullptr);
    ~ProgressBar() = default;

    Coordinate direction() { return _direction; } 
    void setDirection(Coordinate dir);

    float progress() { return _progress; }
    void setProgress(float progress);

    Widget* getForeground() { return _foreground; }
    Widget* getBackground() { return this; }

private:
    Vec2 DefaultSize = {100, 20};
    Coordinate _direction = Coordinate::Horizonal;

    float _progress = 0.0f;
    Widget* _foreground = nullptr;
};

///////////////////////////////////////////////////////////////////////

class SliderBlock : public Button
{
public:
    SliderBlock() = delete;
    ~SliderBlock() = default;
    SliderBlock(const std::string& name, Widget* parent = nullptr);

public:
    void onMouseLeftDrag(const Vec2& pos, const Vec2& offset) override;
};

class SliderBar : public Group
{
public:
    signal<SliderBar*> on_value_changed;
    friend class SliderBlock;

public:
    SliderBar() = delete;
    SliderBar(const std::string& name, Widget* parent = nullptr);
    ~SliderBar() = default;

    float value() { return _value; }
    void setValue(float value);

    float maxValue() { return _maxValue; }
    void setMaxValue(float maxValue);

    Coordinate direction() { return _direction; } 
    void setDirection(Coordinate dir);

    Widget* getSlider() { return _slider; }
    Widget* getBkground() { return this; }

private:
    void onSliderBlockDrag(const Vec2& pos, const Vec2& offset);

private:
    Vec2 DefaultSize = {100, 30};

    float _value = 0.0f;
    float _maxValue = 100.0f;

    Coordinate _direction = Coordinate::Horizonal;

    SliderBlock* _slider = nullptr;
};

}