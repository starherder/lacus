#pragma once

#include "ui_utils.h"
#include "widget.h"
#include "group_widgets.h"

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

    int fontSize() { return _fontSize; }
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
    int _fontSize = 16;
    Font* _font = nullptr;
};

///////////////////////////////////////////////////////////////////////

// multi-line label

class TextBox : public Label
{
public:
    TextBox() = delete;
    TextBox(const std::string& name, Widget* parent = nullptr);
    ~TextBox();

    void draw() override;
};


///////////////////////////////////////////////////////////////////////

class Button : public Label
{
public:
    signal<Button*> on_click;

public:
	Button(const std::string& name, Widget* parent = nullptr);
    ~Button();

protected:    
    void onMouseEnter(const Vec2& pos) override;
    void onMouseLeave(const Vec2& pos) override;

    void onMouseLeftClick(const Vec2& pos) override;

    void onMouseLeftDown(const Vec2& pos) override;
    void onMouseLeftUp(const Vec2& pos) override;

    WidgetState state() { return _state; }

    virtual WidgetStatus& status() override;

    void setState(WidgetState state);

protected:
    WidgetState _state = WidgetState::Normal;

    std::map<WidgetState, WidgetStatus> _status;
};


///////////////////////////////////////////////////////////////////////

class CheckBox : public Button
{
public:
    signal<CheckBox*> on_check_changed;

public:
    CheckBox() = delete;
    ~CheckBox() = default;
    CheckBox(const std::string& name, Widget* parent = nullptr);

    bool checked() const { return _checked; }
    void setChecked(bool checked);

private:
    void onMouseEnter(const Vec2& pos) override;
    void onMouseLeave(const Vec2& pos) override;

    void onMouseLeftClick(const Vec2& pos) override;

    void onMouseLeftDown(const Vec2& pos) override;
    void onMouseLeftUp(const Vec2& pos) override;

private:
    bool _checked = false;
};

///////////////////////////////////////////////////////////////////////

class RadioGroupImpl : public utility::sigslot::SlotHandler
{
public:
    signal<int> on_item_select;

public:
    RadioGroupImpl() = delete;
    RadioGroupImpl(RadioGroupImpl&&) = delete;
    RadioGroupImpl(const RadioGroupImpl&) = delete;
    RadioGroupImpl(Group* group);
    ~RadioGroupImpl();

    int addItem(const std::string& text);
    void removeItem(int index);

    size_t itemCount();

    int getSelectIndex();
    void setSelectItem(int index);

    const auto& items() { return _items; }

private:
    void onItemSelect(CheckBox* cb);

private:
    Group* _group = nullptr;

    std::list<CheckBox*> _items;
};

class RadioHLayGroup : public HorizonalLayout
{
public:
    signal<int> on_item_select;

public:
    RadioHLayGroup() = delete;
    ~RadioHLayGroup() {}
    RadioHLayGroup(const std::string& name, Widget* parent = nullptr);

    int addItem(const std::string& text);
    void removeItem(int index);

    size_t itemCount() { return children().size(); }

    int getSelectIndex();
    void setSelectItem(int index);

private:
    class std::unique_ptr<RadioGroupImpl> _radioGroup = nullptr;
};

class RadioVLayGroup : public VerticalLayout
{
public:
    signal<int> on_item_select;

public:
    RadioVLayGroup() = delete;
    ~RadioVLayGroup() {}
    RadioVLayGroup(const std::string& name, Widget* parent = nullptr);

    int addItem(const std::string& text);
    void removeItem(int index);

    size_t itemCount() { return children().size(); }

    int getSelectIndex();
    void setSelectItem(int index);

private:
    class std::unique_ptr<RadioGroupImpl> _radioGroup = nullptr;
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

    Coordinate direction() const { return _direction; } 
    void setDirection(Coordinate dir);

    float progress() const { return _progress; }
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
    void onMouseLeftDown(const Vec2& pos) override;
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

    void setSize(const Vec2& sz) override;
    void rawSetSize(const Vec2& sz) override;

    float value() const { return _value; }
    void setValue(float value);
    void rawSetValue(float value);

    float maxValue() const { return _maxValue; }
    void setMaxValue(float maxValue);
    void rawSetMaxValue(float maxValue);

    Coordinate direction() const { return _direction; } 
    void setDirection(Coordinate dir);

    Widget* getSlider() { return _slider; }
    Widget* getBkground() { return this; }

private:
    void onSliderBlockDrag(const Vec2& pos, const Vec2& offset);

    void onMouseLeftDown(const Vec2& pos) override;

    void adjustSliderSize();

private:
    Vec2 DefaultSize = {100, 30};

    float _value = 0.0f;
    float _maxValue = 100.0f;

    Coordinate _direction = Coordinate::Horizonal;

    SliderBlock* _slider = nullptr;

    float _beginValue = 0.0f;
    Vec2 _beginPos;
};


class ListBox : public ExpandGroup
{
public:
    signal<int> on_item_select;

public:
    ListBox() = delete;
    ListBox(const std::string& name, Widget* parent = nullptr);
    ~ListBox();

    int addItem(const std::string& text);
    void removeItem(int index);

    size_t itemCount();

    int getSelectIndex();
    void setSelectItem(int index);

    void setItemHeight(float height);

private:
    float _itemHeight = 25;

    class std::unique_ptr<RadioGroupImpl> _radioGroup = nullptr;
};

}