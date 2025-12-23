#include "buttons.h"
#include "gui_manager.h"
#include "magic_enum/magic_enum.h"
#include "ui_utils.h"

namespace ui {

    const float Label::DefaultFontSize = 20.0f;
    const std::string Label::DefaultFontName = "fonts/msyh.ttf";

    Label::Label(const std::string& name, Widget* parent)
        : Widget(name, parent)
    {
        adjust();
    }

    void Label::setFont(const std::string& name, int size)
    {
        _fontName = name;
        _fontSize = size;
        adjust();
    }

    void Label::adjust()
    {
        if (_fontSize <= 0) {
            _fontSize = DefaultFontSize;
        }

        if (_fontName.empty()) {
            _fontName = DefaultFontName;
        }

        _font = GuiManager::inst().fontManager().get(HashString(_fontName.c_str()), _fontSize);
    }

    void Label::draw()
    {
        Widget::draw();

        auto& renderer = GuiManager::inst().renderer();
        auto textSize = renderer.getTextSize(_text, _font);
        auto realPos = getRealPos();

        if(_textAlign == Align::Left)
        {
            realPos += _textPadding;
        }
        else if (_textAlign == Align::Center)
        {
            realPos += (size()-textSize) / 2.0f;
            realPos += _textPadding;
        }
        else if(_textAlign == Align::Right)
        {
            realPos += size() - textSize;
            realPos -= _textPadding;
        }
        else 
        {
            spdlog::error("invalid text align: {}", magic_enum::enum_name(_textAlign));
        }

        auto& state = status();
        renderer.drawText(_text, _font, realPos, state.text_color);
    }


    //////////////////////////////////////////////////////////////////////

    Button::Button(const std::string& name, Widget* parent) : Label(name, parent)
    {
        _handleEvent = true;
        setState(WidgetState::Normal);

        _status[WidgetState::Normal] = WigetUtils::normalStatus;
        _status[WidgetState::Hover] = WigetUtils::hoveredStatus;
        _status[WidgetState::Pressed] = WigetUtils::pressedStatus;
        _status[WidgetState::Disabled] = WigetUtils::disabledStatus;
    }

    Button::~Button()
    {
    }

    void Button::setState(WidgetState state)
    {
        _state = state;
    }

    void Button::onMouseEnter(const Vec2& pos) 
    {
        setState(WidgetState::Hover);
    }

    void Button::onMouseLeave(const Vec2& pos) 
    {
        setState(WidgetState::Normal);
    }

    void Button::onMouseLeftClick(const Vec2& pos)
    {
        on_click.emit(this);
        setState(WidgetState::Hover);
    }

    void Button::onMouseLeftDown(const Vec2& pos) 
    {
        setState(WidgetState::Pressed);
    }

    void Button::onMouseLeftUp(const Vec2& pos) 
    {
        setState(WidgetState::Normal);
    }

    WidgetStatus& Button::status()
    { 
        if(_state<WidgetState::Normal || _state>=WidgetState::Max)
            return Widget::status();
        return _status[_state]; 
    }

    /////////////////////////////////////////////////////////////////


    CheckBox::CheckBox(const std::string& name, Widget* parent) 
        : Button(name, parent)
    {
        _status[WidgetState::Selected] = WigetUtils::selectedStatus;
        _status[WidgetState::SelectedHover] = WigetUtils::selectHoverStatus;
    }

    void CheckBox::setChecked(bool checked)
    {
        _checked = checked;
        on_check_changed.emit(this);

        if(_checked)
        {
            setState(WidgetState::Selected);
        }
        else 
        {
            setState(WidgetState::Normal);
        }
    }

    void CheckBox::onMouseEnter(const Vec2& pos) 
    {
        if(_checked)
        {
            setState(WidgetState::SelectedHover);
        }
        else 
        {
            setState(WidgetState::Hover);
        }
    }

    void CheckBox::onMouseLeave(const Vec2& pos) 
    {
        if(_checked)
        {
            setState(WidgetState::Selected);
        }
        else 
        {
            setState(WidgetState::Normal);
        }
    }

    void CheckBox::onMouseLeftClick(const Vec2& pos)
    {
        _checked = !_checked;
        on_check_changed.emit(this);

        if (_checked)
        {
            setState(WidgetState::SelectedHover);
        }
        else
        {
            setState(WidgetState::Hover);
        }
    }

    void CheckBox::onMouseLeftDown(const Vec2& pos) 
    {
        //setState(WidgetState::Selected);
    }

    void CheckBox::onMouseLeftUp(const Vec2& pos) 
    {
        //onMouseEnter(pos);
    }

    /////////////////////////////////////////////////////////////////
    ProgressBar::ProgressBar(const std::string& name, Widget* parent)
        : Group(name, parent)
    {
        _foreground = createChild<Widget>("_foreground_");
        _foreground->setBgColor(Color::LightBlue);

        setBgColor(Color::PaleBlue);
        setSize(DefaultSize);
        setProgress(0);
    }

    void ProgressBar::setDirection(Coordinate dir) 
    { 
        _direction = dir; 
        setProgress(_progress);
    }

    void ProgressBar::setProgress(float progress)
    {
        _progress = progress;

        Vec2 foreSize = {0, 0};

        if(_direction == Coordinate::Horizonal)
        {
            foreSize.x = progress * size().x;
            foreSize.y = size().y;
        }
        else 
        {
            foreSize.x = size().x;
            foreSize.y = progress * size().y;

            Vec2 fore_pos = _foreground->pos();
            fore_pos.y = size().y - foreSize.y;
            _foreground->setPos(fore_pos);
        }

        _foreground->setSize(foreSize);
    }

    /////////////////////////////////////////////////////////////////

    SliderBlock::SliderBlock(const std::string& name, Widget* parent) : Button(name, parent)
    {
    }

    void SliderBlock::onMouseLeftDrag(const Vec2& pos, const Vec2& offset) 
    {
        if(!parent()) return;

        auto parent_wgt = dynamic_cast<SliderBar*>(parent());
        if(parent_wgt)
        {
            parent_wgt->onSliderBlockDrag(pos, offset);
        }
    }

    SliderBar::SliderBar(const std::string& name, Widget* parent) 
        : Group(name, parent)
    {
        setHandleEvent(true);
        setBgColor(Color::PaleBlue);
        setSize(DefaultSize);

        Vec2 sliderSize;
        if(_direction == Coordinate::Horizonal)
        {
            sliderSize.x = DefaultSize.y;
            sliderSize.y = DefaultSize.y;
        }
        else 
        {
            sliderSize.x = DefaultSize.x;
            sliderSize.y = DefaultSize.x;
        }

        _slider = createChild<SliderBlock>("_slider_");
        _slider->setSize(sliderSize);

        setValue(0);
    }

    void SliderBar::setMaxValue(float maxValue)
    {
        _maxValue = maxValue;
        setValue(_value);
    }

    void SliderBar::setDirection(Coordinate dir) 
    { 
        _direction = dir; 
        setValue(_value);
    }

    void SliderBar::setValue(float value) 
    { 
        _value = value;
        _maxValue = _maxValue==0 ? 100 : _maxValue;
        _maxValue = std::max(_maxValue, _value);

        float progress = std::clamp(_value/_maxValue, 0.0f, 1.0f);

        auto pos = _slider->pos();
        if(_direction == Coordinate::Horizonal)
        {
            pos.x = progress * (size().x - _slider->size().x);
            pos.y = (size().y - _slider->size().y) / 2.0f;
        }
        else 
        {
            pos.x = (size().x - _slider->size().x) / 2.0f;
            pos.y = progress * (size().y - _slider->size().y);
        }

        _slider->setPos(pos);
        on_value_changed.emit(this);
    }

    void SliderBar::onSliderBlockDrag(const Vec2& pos, const Vec2& offset)
    {
        _maxValue = _maxValue==0 ? 100 : _maxValue;
        _maxValue = std::max(_maxValue, _value);

        float delta = (_direction==Coordinate::Horizonal) ? offset.x/size().x : offset.y/size().y;
        float progress = std::clamp(delta + _value/_maxValue, 0.0f, 1.0f);

        setValue(progress * _maxValue);
    }
}