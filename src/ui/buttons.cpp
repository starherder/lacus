#include "buttons.h"
#include "gui_manager.h"
#include "magic_enum/magic_enum.h"

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
        _state = WidgetState::Normal;

        _status[WidgetState::Normal] = WigetUtils::normalStatus;
        _status[WidgetState::Hover] = WigetUtils::hoveredStatus;
        _status[WidgetState::Pressed] = WigetUtils::pressedStatus;
        _status[WidgetState::Disabled] = WigetUtils::disabledStatus;
    }

    Button::~Button()
    {
    }

    void Button::update(float delta)
    {

    }

    void Button::draw()
    {
        Label::draw();
    }

    /////////////////////////////////////////////////////////////////

    void Button::setState(WidgetState state)
    {
        //spdlog::info("btn({}) change state from {} to {}", name(), 
        //    magic_enum::enum_name(_state), magic_enum::enum_name(state));

        _state = state;
    }

    void Button::onMouseEnter() 
    {
        setState(WidgetState::Hover);
    }

    void Button::onMouseLeave() 
    {
        setState(WidgetState::Normal);
    }

    void Button::onMouseLeftClick()
    {
        on_click.emit(this);

        setState(WidgetState::Hover);
    }

    void Button::onMouseLeftDown() 
    {
        setState(WidgetState::Pressed);
    }

    void Button::onMouseLeftUp() 
    {
        setState(WidgetState::Normal);
    }

    WidgetStatus& Button::status()
    { 
        if(_state<WidgetState::Normal || _state>=WidgetState::Max)
            return Widget::status();
        return _status[_state]; 
    }
}