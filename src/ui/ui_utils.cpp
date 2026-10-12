#include "ui_utils.h"

namespace ui {

void UiMetrics::update(const Vec2& window, const Rect& safe, float displayScale_)
{
    windowSize = window;
    safeArea = safe;
    displayScale = displayScale_ > 0.0f ? displayScale_ : 1.0f;

    if (safeArea.w <= 0.0f || safeArea.h <= 0.0f)
    {
        safeArea = Rect{{0.0f, 0.0f}, windowSize};
    }

    float sx = designSize.x > 0.0f ? safeArea.w / designSize.x : 1.0f;
    float sy = designSize.y > 0.0f ? safeArea.h / designSize.y : 1.0f;
    float fitScale = std::min(sx, sy);
    scale = std::max(1.0f, fitScale);

    uiSize = safeArea.size() / scale;
    portrait = uiSize.y > uiSize.x;
    version++;
}

Vec2 UiMetrics::screenToUiPoint(const Vec2& point) const
{
    return (point - safeArea.pos()) / scale;
}

Vec2 UiMetrics::screenToUiDelta(const Vec2& delta) const
{
    return delta / scale;
}

Rect UiMetrics::screenToUiRect(const Rect& rect) const
{
    return {screenToUiPoint(rect.pos()), screenToUiDelta(rect.size())};
}

Vec2 UiMetrics::uiToScreenPoint(const Vec2& point) const
{
    return safeArea.pos() + point * scale;
}

Vec2 UiMetrics::uiToScreenDelta(const Vec2& delta) const
{
    return delta * scale;
}

Rect UiMetrics::uiToScreenRect(const Rect& rect) const
{
    return {uiToScreenPoint(rect.pos()), uiToScreenDelta(rect.size())};
}

float UiMetrics::uiToScreenScalar(float value) const
{
    return value * scale;
}


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

static std::string trimMetricToken(const char* str)
{
    if (!str)
    {
        return "";
    }

    std::string token = str;
    auto begin = token.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos)
    {
        return "";
    }

    auto end = token.find_last_not_of(" \t\r\n");
    token = token.substr(begin, end - begin + 1);

    if (token.ends_with("dp") || token.ends_with("px"))
    {
        token.resize(token.size() - 2);
    }

    return token;
}

float WidgetUtils::parseMetric(const char* str, float defaultValue)
{
    auto token = trimMetricToken(str);
    if (token.empty())
    {
        return defaultValue;
    }

    try
    {
        return std::stof(token);
    }
    catch (const std::exception&)
    {
        return defaultValue;
    }
}

Vec2 WidgetUtils::parseVec2(const char* str, const Vec2& defaultValue)
{
    if (!str)
    {
        return defaultValue;
    }

    auto arr = utility::StringUtil::split(str, ',');
    if (arr.size() != 2)
    {
        return defaultValue;
    }

    std::string x{arr[0]};
    std::string y{arr[1]};
    return {parseMetric(x.c_str(), defaultValue.x), parseMetric(y.c_str(), defaultValue.y)};
}

}
