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
    Top,
    Bottom,
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
        ~##WidgetType##_Initializer() {}                                    \
    }g_##WidgetType##_Initializer;







}