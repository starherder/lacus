#include "ui_utils.h"

namespace ui {


const WidgetStatus WigetUtils::normalStatus = WidgetStatus{
                                        .ground_color = Color::Light,
                                        .text_color = Color::Black,
                                        .border_color = Color::Gray,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::hoveredStatus = WidgetStatus{
                                        .ground_color = Color::Pale,
                                        .text_color = Color::Gray,
                                        .border_color = Color::DarkGreen,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::pressedStatus = WidgetStatus{
                                        .ground_color = Color::Gray,
                                        .text_color = Color::Black,
                                        .border_color = Color::DarkGreen,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::disabledStatus = WidgetStatus{
                                        .ground_color = Color::Dark,
                                        .text_color = Color::Black,
                                        .border_color = Color::Gray,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::selectedStatus = WidgetStatus{
                                        .ground_color = Color::Gray,
                                        .text_color = Color::Yellow,
                                        .border_color = Color::Yellow,
                                        .texture = nullptr
};

const WidgetStatus WigetUtils::selectHoverStatus = WidgetStatus{
                                        .ground_color = Color::Gray,
                                        .text_color = Color::Yellow,
                                        .border_color = Color::Green,
                                        .texture = nullptr
};

}