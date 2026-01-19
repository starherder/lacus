#pragma once

#include <memory>
#include "engine/application.h"
#include "utility/dynamic_struct.h"
#include "tinyxml2/tinyxml2.h"

namespace ui 
{

using namespace engine;

template <typename... _ArgTypes>
using signal = utility::sigslot::Signal<_ArgTypes...>;

using Properties = utility::DynamicStruct<std::string>;

using XmlNode = tinyxml2::XMLElement;

struct WidgetStatus {
    Color ground_color;
    Color text_color;
    Color border_color;

    Texture* texture;
    Rect tex_rect;
};

enum class WidgetState {
    Normal,
    Hover,
    Pressed,
    Disabled,
    Selected,
    SelectedHover,
    Dragging,

    Max,
};

enum class Align {
    Left,
    Center,
    Right,
};

enum class Coordinate {
    Horizonal,
    Vertical,
};

class WigetUtils
{
public:
    static const WidgetStatus normalStatus;
    static const WidgetStatus hoveredStatus;
    static const WidgetStatus pressedStatus;
    static const WidgetStatus disabledStatus;
    static const WidgetStatus selectedStatus;
    static const WidgetStatus selectHoverStatus;
};


}