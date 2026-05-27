#include "ui_utils.h"

namespace ui {


const WidgetStatus WidgetUtils::normalStatus = WidgetStatus{
                                        .ground_color = Color::Light,
                                        .text_color = Color::Black,
                                        .border_color = Color::Gray,
                                        .texture = nullptr
};

const WidgetStatus WidgetUtils::hoveredStatus = WidgetStatus{
                                        .ground_color = Color::White,
                                        .text_color = Color::Gray,
                                        .border_color = Color::DarkGreen,
                                        .texture = nullptr
};

const WidgetStatus WidgetUtils::pressedStatus = WidgetStatus{
                                        .ground_color = Color::Gray,
                                        .text_color = Color::Black,
                                        .border_color = Color::DarkGreen,
                                        .texture = nullptr
};

const WidgetStatus WidgetUtils::disabledStatus = WidgetStatus{
                                        .ground_color = Color::Dark,
                                        .text_color = Color::Black,
                                        .border_color = Color::Gray,
                                        .texture = nullptr
};

const WidgetStatus WidgetUtils::selectedStatus = WidgetStatus{
                                        .ground_color = Color::Gray,
                                        .text_color = Color::Yellow,
                                        .border_color = Color::Yellow,
                                        .texture = nullptr
};

const WidgetStatus WidgetUtils::selectHoverStatus = WidgetStatus{
                                        .ground_color = Color::Gray,
                                        .text_color = Color::Yellow,
                                        .border_color = Color::Green,
                                        .texture = nullptr
};


Coordinate WidgetUtils::getCoord(const std::string& str)
{
    if (str == "horizonal") return Coordinate::Horizontal;
    if (str == "vertical") return Coordinate::Vertical;
    return Coordinate::Horizontal;
}

Align WidgetUtils::getAlign(const std::string& str) 
{
    if (str == "left") return Align::Left;
    else if (str == "right") return Align::Right;
    else if (str == "center") return Align::Center;
    else if (str == "top") return Align::Top;
    else if (str == "bottom") return Align::Bottom;

    return Align::Center;
}

void WidgetUtils::parseColorIfValid(Color& color, const char* hex)
{
    auto c = Color::parseHexString(hex);
    if (c.isValid()) color = c;
}

}