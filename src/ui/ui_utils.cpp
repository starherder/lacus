#pragma once

#include "ui_utils.h"

namespace ui {


const WidgetStatus WigetUtils::normalStatus = WidgetStatus{
                                        .groud_color = ColorUtils::Light,
                                        .text_color = ColorUtils::Black,
                                        .border_color = ColorUtils::Gray,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::hoveredStatus = WidgetStatus{
                                        .groud_color = ColorUtils::White,
                                        .text_color = ColorUtils::Green,
                                        .border_color = ColorUtils::Green,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::pressedStatus = WidgetStatus{
                                        .groud_color = ColorUtils::Gray,
                                        .text_color = ColorUtils::DarkGreen,
                                        .border_color = ColorUtils::DarkGreen,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::disabledStatus = WidgetStatus{
                                        .groud_color = ColorUtils::DarkGreen,
                                        .text_color = ColorUtils::Black,
                                        .border_color = ColorUtils::Gray,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::selectedStatus = WidgetStatus{
                                        .groud_color = ColorUtils::Light,
                                        .text_color = ColorUtils::DarkYellow,
                                        .border_color = ColorUtils::Yellow,
                                        .texture = nullptr
};

}