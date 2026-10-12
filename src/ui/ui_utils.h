#pragma once

#include <memory>
#include "engine/application.h"
#include "utility/dynamic_struct.h"
#include "tinyxml2/tinyxml2.h"

namespace ui 
{

using namespace engine;

template <typename... Args>
using signal = utility::sigslot::Signal<Args...>;

using Properties = utility::DynamicStruct<std::string>;

using XmlNode = tinyxml2::XMLElement;

struct WidgetStatus {
    Color ground_color;
    Color text_color;
    Color border_color;

    TexTile* texture = nullptr;
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
    Top,
    Bottom,
};

enum class Coordinate {
    Horizontal,
    Vertical,
};

struct UiMetrics
{
    Vec2 designSize = {1280.0f, 720.0f};
    Vec2 windowSize = {1280.0f, 720.0f};
    Rect safeArea = {0.0f, 0.0f, 1280.0f, 720.0f};
    Vec2 uiSize = {1280.0f, 720.0f};

    float scale = 1.0f;
    float displayScale = 1.0f;
    bool portrait = false;
    unsigned int version = 0;

    void update(const Vec2& window, const Rect& safe, float displayScale_);

    Vec2 screenToUiPoint(const Vec2& point) const;
    Vec2 screenToUiDelta(const Vec2& delta) const;
    Rect screenToUiRect(const Rect& rect) const;

    Vec2 uiToScreenPoint(const Vec2& point) const;
    Vec2 uiToScreenDelta(const Vec2& delta) const;
    Rect uiToScreenRect(const Rect& rect) const;
    float uiToScreenScalar(float value) const;
};

class WidgetUtils
{
public:
    static const WidgetStatus normalStatus;
    static const WidgetStatus hoveredStatus;
    static const WidgetStatus pressedStatus;
    static const WidgetStatus disabledStatus;
    static const WidgetStatus selectedStatus;
    static const WidgetStatus selectHoverStatus;

    static Coordinate getCoord(const std::string& str);

    static Align getAlign(const std::string& str);

    static void parseColorIfValid(Color& color, const char* hex);

    static float parseMetric(const char* str, float defaultValue = 0.0f);
    static Vec2 parseVec2(const char* str, const Vec2& defaultValue = {0.0f, 0.0f});
};


class Widget;
using WidgetPtr = std::shared_ptr<Widget>;

class WidgetCreator
{
public:
    virtual const char* typeName() = 0;
    virtual WidgetPtr create(const std::string& name, Widget* widget) = 0;
};

using WidgetCreatorPtr = std::shared_ptr<WidgetCreator>;


#define DeclareWidgetType(WidgetType, TypeName)                             \
    struct WidgetType##_Creator : public WidgetCreator {                    \
        const char* typeName() override { return TypeName; }                \
        WidgetPtr create(const std::string& name, Widget* widget) override  \
        { return std::make_shared<WidgetType>(name, widget); }              \
    };                                                                      \
    struct WidgetType##_Initializer{                                        \
        WidgetType##_Initializer() {                                        \
            ui::GuiManager::inst()                                          \
                .addWidgetCreator<WidgetType##_Creator>();                  \
        }                                                                   \
        ~WidgetType##_Initializer() = default;                                    \
    }g_##WidgetType##_Initializer;







}
