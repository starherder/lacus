#include "base_widgets.h"
#include "gui_manager.h"
#include "magic_enum/magic_enum.h"
#include "ui_utils.h"

namespace ui 
{
    DeclareWidgetType(Label, "label");
    DeclareWidgetType(TextBox, "text");
    DeclareWidgetType(Button, "button");

    DeclareWidgetType(CheckBox, "check");
    DeclareWidgetType(RadioBox, "radio");

    DeclareWidgetType(RadioHLayGroup, "radio_hlay");
    DeclareWidgetType(RadioVLayGroup, "radio_vlay");

    DeclareWidgetType(ProgressBar, "progress");
    DeclareWidgetType(SliderBar, "slider");
    DeclareWidgetType(ListBox, "list");
    
    // --------------------------------------------------------------------

    const int Label::DefaultFontSize = 20;
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

    bool Label::onLoad(XmlNode* node)
    {
        if (!node) {
            return false;
        }

        _text = node->Attribute("text");
        _text_color.fromHexString(node->Attribute("text_color"));
        if (_text_color.a <= 0) {
            _text_color.a = 255;
        }

        _textPadding = ToVec2(node->Attribute("text_padding"));

        auto getAlign = [](const std::string& str) {
            if (str == "left") return Align::Left;
            else if (str == "right") return Align::Right;
            else if (str == "center") return Align::Center;
            else if (str == "top") return Align::Top;
            else if (str == "bottom") return Align::Bottom;
            
            return Align::Center;
        };

        _textAlign = getAlign(node->Attribute("align"));

        _fontName = node->Attribute("font_name");
        _fontName = _fontName.empty() ? DefaultFontName : _fontName;
        _fontSize = node->IntAttribute("font_size", 20);

        adjust();

        return true;
    }

    void Label::draw()
    {
        Widget::draw();

        auto& renderer = GuiManager::inst().renderer();
        auto textSize = renderer.getTextSize(_text, _font);
        auto realPos = getAbsPos();

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
            SPDLOG_ERROR("invalid text align: {}", magic_enum::enum_name(_textAlign));
        }

        auto& state = status();
        GuiManager::inst().painter().drawText(_text, _font, realPos, state.text_color);
    }

    //////////////////////////////////////////////////////////////////////

    TextBox::TextBox(const std::string& name, Widget* parent) : Widget(name, parent)
    {
        adjust();
    }

    TextBox::~TextBox()
    {
    }

    bool TextBox::onLoad(XmlNode* node)
    {
        if (!node)  { 
            return false; 
        }

        _fontName = node->Attribute("font_name");
        _fontName = _fontName.empty() ? Label::DefaultFontName : _fontName;
        _fontSize = node->IntAttribute("font_size", 20);

        _text = node->Attribute("text");
        _text_color.fromHexString(node->Attribute("text_color"));
        
        if (_text_color.a <= 0)  {
            _text_color.a = 255;
        }

        adjust();
        return true;
    }

    void TextBox::setFont(const std::string& name, int size)
    {
        _fontName = name;
        _fontSize = size;
        adjust();
    }

    void TextBox::adjust()
    {
        if (_fontSize <= 0) {
            _fontSize = Label::DefaultFontSize;
        }

        if (_fontName.empty()) {
            _fontName = Label::DefaultFontName;
        }

        _font = GuiManager::inst().fontManager().get(HashString(_fontName.c_str()), _fontSize);
    }

    void TextBox::draw()
    {
        auto& renderer = GuiManager::inst().renderer();
        auto textSize = renderer.getTextSize(text(), _font);
        auto realPos = getAbsPos();

        GuiManager::inst().painter().drawText(text(), _font, realPos, _text_color, size().x);
    }

    //////////////////////////////////////////////////////////////////////

    Button::Button(const std::string& name, Widget* parent) : Label(name, parent)
    {
        setState(WidgetState::Normal);
        setAcceptEvent(true);

        _status[WidgetState::Normal] = WigetUtils::normalStatus;
        _status[WidgetState::Hover] = WigetUtils::hoveredStatus;
        _status[WidgetState::Pressed] = WigetUtils::pressedStatus;
        _status[WidgetState::Disabled] = WigetUtils::disabledStatus;
    }

    Button::~Button()
    {
    }

    bool Button::onLoad(XmlNode* node)
    {
        if (!Label::onLoad(node)) {
            return false;
        }

        return false;
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

    bool CheckBox::onLoad(XmlNode* node)
    {
        if (!Button::load(node)) {
            return false;
        }

        _checked = node->BoolAttribute("checked");

        adjust();

        return true;
    }

    void CheckBox::adjust()
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

    void CheckBox::setChecked(bool checked)
    {
        if (checked == _checked) 
        { 
            return; 
        }

        _checked = checked;
        on_check_changed.emit(this);
        
        adjust();
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
        setChecked(!checked());
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


    RadioBox::RadioBox(const std::string& name, Widget* parent) : CheckBox(name, parent)
    {
    }

    void RadioBox::onMouseLeftClick(const Vec2& pos)
    {
        if (!checked())
        {
            setChecked(!checked());
        }
    }

    void RadioBox::setChecked(bool checked)
    {
        if (checked == _checked)
        {
            return;
        }

        _checked = checked;
        on_selected.emit(this);

        adjust();
    }
    
    bool RadioBox::onLoad(XmlNode* node)
    {
        if (!CheckBox::onLoad(node))
        {
            return false;
        }


        adjust();
        return true;
    }

    /////////////////////////////////////////////////////////////////

    RadioGroupImpl::RadioGroupImpl(Group* group) : _group(group)
    {
    }

    RadioGroupImpl::~RadioGroupImpl()
    {
    }

    int RadioGroupImpl::addItem(const std::string& text) 
    {
        auto index = (int)itemCount();
        auto ctrl = _group->createChild<RadioBox>(fmt::format("__radio_{}__", index));
        ctrl->setText(text);
        ctrl->setData("__item_index__", index);
        ctrl->on_selected.connect(this, &RadioGroupImpl::onItemSelect);

        _items.push_back(ctrl);
        return index;
    }

    void RadioGroupImpl::removeItem(int index) 
    {
        Widget* widget = nullptr;

        for (auto& item : _group->children()) {
            auto item_index = item->getData<int>("__item_index__");
            if (item_index == index) {
                widget = item.get();
                _group->removeChild(item->name());
                break;
            }
        }

        if (widget) 
        {
            for (auto it = _items.begin(); it != _items.end(); it++) {
                if (widget == *it) {
                    _items.erase(it);
                    break;
                }
            }
        }
    }

    size_t RadioGroupImpl::itemCount() 
    {
        return _items.size(); 
    }

    int RadioGroupImpl::getSelectIndex() 
    {
        for (auto& item : _group->children()) {
            auto cb = dynamic_cast<CheckBox*>(item.get());
            if (cb && cb->checked()) {
                auto item_index = item->getData<int>("__item_index__");
                return item_index;
            }
        }
        return -1;
    }

    void RadioGroupImpl::setSelectItem(int index) 
    {
        for (auto& item : _group->children()) {
            auto cb = dynamic_cast<CheckBox*>(item.get());
            if (cb) {
                if (cb->checked()) { 
                    cb->setChecked(false); 
                }

                auto item_index = item->getData<int>("__item_index__");
                if (item_index == index) {
                    cb->setChecked(true);
                }
            }
        }
    }

    void RadioGroupImpl::onItemSelect(RadioBox* cb) 
    {
        if (!cb || !cb->checked()) {
            return;
        }

        auto select = cb->checked();
        auto index = cb->getData<int>("__item_index__");

        for (auto& item : _group->children()) {
            auto cb = dynamic_cast<CheckBox*>(item.get());
            if (cb) {
                auto item_index = item->getData<int>("__item_index__");
                if (item_index != index) {
                    cb->setChecked(false);
                }
            }
        }

        on_item_select.emit(index);
    }

    /////////////////////////////////////////////////////////////////

    RadioHLayGroup::RadioHLayGroup(const std::string& name, Widget* parent) : HorizonalLayout(name, parent)
    {
        _radioGroup = std::make_unique<RadioGroupImpl>(this);
        _radioGroup->on_item_select.connect([this](int v) { on_item_select.emit(v); });
    }

    int RadioHLayGroup::addItem(const std::string& text)
    {
        return _radioGroup->addItem(text);
    }

    void RadioHLayGroup::removeItem(int index)
    {
        _radioGroup->removeItem(index);
    }

    int RadioHLayGroup::getSelectIndex()
    {
        return _radioGroup->getSelectIndex();
    }

    void RadioHLayGroup::setSelectItem(int index)
    {
        return _radioGroup->setSelectItem(index);
    }

    bool RadioHLayGroup::onLoad(XmlNode* node)
    {
        if (!HorizonalLayout::onLoad(node))
        {
            return false;
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////

    RadioVLayGroup::RadioVLayGroup(const std::string& name, Widget* parent) : VerticalLayout(name, parent)
    {
        _radioGroup = std::make_unique<RadioGroupImpl>(this);
        _radioGroup->on_item_select.connect([this](int v) { on_item_select.emit(v); });
    }

    int RadioVLayGroup::addItem(const std::string& text)
    {
        return _radioGroup->addItem(text);
    }

    void RadioVLayGroup::removeItem(int index)
    {
        _radioGroup->removeItem(index);
    }

    int RadioVLayGroup::getSelectIndex()
    {
        return _radioGroup->getSelectIndex();
    }

    void RadioVLayGroup::setSelectItem(int index)
    {
        return _radioGroup->setSelectItem(index);
    }

    bool RadioVLayGroup::onLoad(XmlNode* node)
    {
        if (!VerticalLayout::onLoad(node))
        {
            return false;
        }

        return true;
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

    bool ProgressBar::onLoad(XmlNode* node)
    {
        if (!Group::onLoad(node))
        {
            return false;
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////

    SliderBlock::SliderBlock(const std::string& name, Widget* parent) : Button(name, parent)
    {
        setMovable(true);
    }

    void SliderBlock::onMouseLeftDown(const Vec2& pos)
    {
        if (!parent()) return;

        auto parent_wgt = dynamic_cast<SliderBar*>(parent());
        if (parent_wgt)
        {
            parent_wgt->onMouseLeftDown(pos);
        }
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

    bool SliderBlock::onLoad(XmlNode* node)
    {
        if (!Button::onLoad(node))
        {
            return false;
        }

        return true;
    }

    SliderBar::SliderBar(const std::string& name, Widget* parent) 
        : Group(name, parent)
    {
        _slider = createChild<SliderBlock>("_slider_");

        setMovable(true);

        setBgColor(Color::Pale);

        setSize(DefaultSize);

        adjustSliderSize();

        setValue(0);
    }

    void SliderBar::setMaxValue(float maxValue)
    {
        _maxValue = maxValue;
        setValue(_value);
    }

    void SliderBar::rawSetMaxValue(float maxval)
    {
        if (maxval < _value)
        {
            _maxValue = _value;
        }
        else 
        {
            _maxValue = maxval;
        }

        _maxValue = _maxValue == 0 ? 100 : _maxValue;

        rawSetValue(_value);
    }

    void SliderBar::setDirection(Coordinate dir) 
    { 
        _direction = dir; 

        setValue(_value);

        adjustSliderSize();
    }

    void SliderBar::setSize(const Vec2& sz)
    {
        Group::setSize(sz);
        adjustSliderSize();
    }

    void SliderBar::rawSetSize(const Vec2& sz)
    {
        Group::rawSetSize(sz);
        adjustSliderSize();
    }

    void SliderBar::rawSetValue(float value)
    {
        _value = std::clamp(value, 0.0f, _maxValue);
    }

    void SliderBar::setValue(float value) 
    { 
        rawSetValue(value);

        auto sliderPos = _slider->pos();
        float progress = std::clamp(_value / _maxValue, 0.0f, 1.0f);

        if (_direction == Coordinate::Horizonal)
        {
            sliderPos.x = progress * (size().x - _slider->size().x);
            sliderPos.y = (size().y - _slider->size().y) / 2.0f;
        }
        else
        {
            sliderPos.x = (size().x - _slider->size().x) / 2.0f;
            sliderPos.y = progress * (size().y - _slider->size().y);
        }

        _slider->setPos(sliderPos);

        on_value_changed.emit(this);
    }

    void SliderBar::adjustSliderSize()
    {
        float progress = std::clamp(_value/_maxValue, 0.0f, 1.0f);

        auto sliderPos = _slider->pos();
        Vec2 sliderSize;

        if(_direction == Coordinate::Horizonal)
        {
            sliderSize.x = size().y * 2;
            sliderSize.y = size().y;

            sliderPos.x = progress * (size().x - sliderSize.x);
            sliderPos.y = (size().y - sliderSize.y) / 2.0f;
        }
        else 
        {
            sliderSize.x = size().x;
            sliderSize.y = size().x * 2;

            sliderPos.x = (size().x - sliderSize.x) / 2.0f;
            sliderPos.y = progress * (size().y - sliderSize.y);
        }

        _slider->setPos(sliderPos);
        _slider->setSize(sliderSize);
    }

    void SliderBar::onMouseLeftDown(const Vec2& pos)
    {
        _beginValue = _value;
        _beginPos = pos;
    }

    void SliderBar::onSliderBlockDrag(const Vec2& pos, const Vec2& offset)
    {
        _maxValue = _maxValue==0 ? 100 : _maxValue;
        _maxValue = std::max(_maxValue, _value);

        Vec2 totalOffset = pos - _beginPos;
        auto realSize = size() - _slider->size();

        float poffset = (_direction == Coordinate::Horizonal) ? totalOffset.x / realSize.x : totalOffset.y / realSize.y;
        float value = _beginValue + poffset * _maxValue;

        setValue(value);
    }

    bool SliderBar::onLoad(XmlNode* node)
    {
        if (!Group::onLoad(node))
        {
            return false;
        }

        auto getCoord = [](const std::string& str) {
            if (str == "horizonal") return Coordinate::Horizonal;
            if (str == "vertical") return Coordinate::Vertical;
            return Coordinate::Horizonal;
        };

        auto direction = getCoord(node->Attribute("coordinate"));
        setDirection(direction);

        auto maxValue = node->FloatAttribute("max_value", 100.0f);
        setMaxValue(maxValue);

        auto value = node->FloatAttribute("value");
        setValue(value);

        adjustSliderSize();
        return true;
    }

    // ------------------------------------------------------------------------------------------

    ListBox::ListBox(const std::string& name, Widget* parent) : ExpandGroup(name, parent)
    {
        _radioGroup = std::make_unique<RadioGroupImpl>(this);
        _radioGroup->on_item_select.connect([this](int v) { on_item_select.emit(v); });
    }

    ListBox::~ListBox()
    {
    }

    void ListBox::setItemHeight(float height)
    {
        _itemHeight = height;
        
        const auto& items = _radioGroup->items();
        for (auto& item : items) 
        {
            auto& sz = item->size();
            item->setSize({sz.x, height});
        }
    }

    size_t ListBox::itemCount()
    {
        return _radioGroup->itemCount();
    }

    int ListBox::addItem(const std::string& text)
    {
        auto index = _radioGroup->addItem(text);
        auto name = fmt::format("__radio_{}__", index);
        auto ctrl = getChild<RadioBox>(name);
        if (ctrl) 
        {
            ctrl->setSize({size().x, _itemHeight});
            ctrl->setPos({ 0, _itemHeight * index });
        }
        return index;
    }

    void ListBox::removeItem(int index)
    {
        _radioGroup->removeItem(index);

        auto& items = _radioGroup->items();
        for (auto& item : items) {
            if (item) {
                item->setSize({ size().x, _itemHeight });
                item->setPos({ 0, _itemHeight * _radioGroup->itemCount() });
            }
        }
    }

    int ListBox::getSelectIndex()
    {
        return _radioGroup->getSelectIndex();
    }

    void ListBox::setSelectItem(int index)
    {
        return _radioGroup->setSelectItem(index);
    }

    bool ListBox::onLoad(XmlNode* node)
    {
        if (!ExpandGroup::onLoad(node))
        {
            return false;
        }
        return true;
    }

}