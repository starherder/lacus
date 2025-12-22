#pragma once

#include <memory>
#include "engine/application.h"

namespace ui {

using namespace engine;

template <typename... _ArgTypes>
using signal = utility::sigslot::Signal<_ArgTypes...>;

namespace sigslot = utility::sigslot;

struct WidgetStatus {
    Color groud_color;
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
    Dragging,

    Max,
};


class WigetUtils
{
public:
    static const WidgetStatus normalStatus;
    static const WidgetStatus hoveredStatus;
    static const WidgetStatus pressedStatus;
    static const WidgetStatus disabledStatus;
    static const WidgetStatus selectedStatus;
};


}