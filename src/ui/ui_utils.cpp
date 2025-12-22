#pragma once

#include "ui_utils.h"

namespace ui {


const WidgetStatus WigetUtils::normalStatus = WidgetStatus{
                                        .groud_color = Color::Light,
                                        .text_color = Color::Black,
                                        .border_color = Color::Gray,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::hoveredStatus = WidgetStatus{
                                        .groud_color = Color::White,
                                        .text_color = Color::Green,
                                        .border_color = Color::Green,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::pressedStatus = WidgetStatus{
                                        .groud_color = Color::Gray,
                                        .text_color = Color::DarkGreen,
                                        .border_color = Color::DarkGreen,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::disabledStatus = WidgetStatus{
                                        .groud_color = Color::DarkGreen,
                                        .text_color = Color::Black,
                                        .border_color = Color::Gray,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::selectedStatus = WidgetStatus{
                                        .groud_color = Color::Light,
                                        .text_color = Color::DarkYellow,
                                        .border_color = Color::Yellow,
                                        .texture = nullptr
};

}