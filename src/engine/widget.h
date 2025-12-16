#pragma once

#include "entt/entt.hpp"
#include "render.h"
#include "utility/i_singleton.h"

namespace engine
{
    enum WidgetState
    {
        WidgetState_Normal,
        WidgetState_Hover,
        WidgetState_Press,
        WidgetState_Disable,
    };

    struct WidgetBase
    {
        entt::entity parent = entt::null;

        entt::entity self = entt::null;

        std::string name = "";

        WidgetState state = WidgetState_Normal;

        bool visible = true;

        std::unordered_map<std::string, entt::entity> children;

        WidgetBase(entt::entity parent, entt::entity self, const std::string& name)
            : parent(parent), self(self), name(name)
        {}
    };

    struct WidgetNormal
    {
        WidgetState state = WidgetState_Normal;

        FRect back_rect = {100, 100, 250, 100};
        int border_size = 1;

        Color back_color = {0, 100, 0, 100};
        Color text_color = {0, 255, 0, 255};
        Color border_color = {0, 200, 0, 200};

        SDL_Texture* bg_texture = nullptr;
        std::string text = "";
    };

    struct WidgetHover : public WidgetNormal
    {
        WidgetHover()
        {
            state = WidgetState_Hover;

            border_size = 2;

            back_color = {0, 100, 0, 200};
            text_color = {100, 255, 100, 255};
            border_color = {0, 200, 0, 200};
        }
        ~WidgetHover() = default;
    };

    struct WidgetPress : public WidgetNormal
    {
        WidgetPress()
        {
            state = WidgetState_Press;

            int border_size = 2;

            back_color = {0, 80, 0, 250};
            text_color = {0, 100, 0, 255};
            border_color = {0, 100, 0, 250};
        }
        ~WidgetPress() = default;
    };

    struct WidgetDisable : public WidgetNormal
    {
        WidgetDisable()
        {
            state = WidgetState_Disable;

            int border_size = 1;

            back_color = {100, 100, 100, 250};
            text_color = {10, 10, 10, 255};
            border_color = {10, 10, 10, 250};
        }
        ~WidgetDisable() = default;
    };



    class WidgetManager : public utility::ISingleton<WidgetManager>
    {
    public:
        void init(Renderer& renderer)
        {
            _renderer = &renderer;
        }

        entt::entity create_label(entt::entity parent, const std::string& name)
        {
            if(_registry.valid(parent))
            {
                auto& widget_base = _registry.get<WidgetBase>(parent);
                if(widget_base.children.find(name)!=widget_base.children.end())
                {
                    return widget_base.children[name];
                }
            }

            auto widget = _registry.create();
            _registry.emplace<WidgetBase>(widget, parent, widget, name);
            _registry.emplace<WidgetNormal>(widget);
            return widget;
        }

        void draw()
        {
            auto ents = _registry.view<WidgetBase>();
            for(auto widget : ents)
            {
                auto& base = _registry.get<WidgetBase>(widget);
                if(base.visible)
                {
                    if(base.state == WidgetState_Normal)
                    {
                        if(auto* state = _registry.try_get<WidgetNormal>(widget))
                        {
                            //_renderer->setRenderClipRect(&state.back_rect);

                            Color old_color = _renderer->getDrawColor();

                            // background
                            _renderer->setDrawColor(state->back_color);
                            _renderer->drawFillRect(state->back_rect);

                            // border
                            _renderer->setDrawColor(state->border_color);
                            //_renderer->setRenderLineWidth(state.border_size);
                            _renderer->drawRect(state->back_rect);
                            
                            // text
                            _renderer->setDrawColor(state->text_color);
                            // _renderer->renderText(state->text, state->back_rect.x + 10, state->back_rect.y + 10);
                            _renderer->drawDebugText({state->back_rect.x + 10, state->back_rect.y + 10}, state->text.c_str());
                            
                            _renderer->setDrawColor(old_color);
                        }
                    }
                }
            }
        }

    private:
        entt::registry _registry;

        Renderer* _renderer = nullptr;
    };
}