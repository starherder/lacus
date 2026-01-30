#pragma once

#include "engine/wrapper.h"
#include "engine/camera.h"

namespace samples {

using namespace engine;

class SampleCamera : public Camera
{
public:
    SampleCamera() = default;
    ~SampleCamera() = default;
    SampleCamera(const Vec2& pos, const Vec2& size) 
        : Camera(pos, size) {
    
    }

    bool isShiftKeyDown()
    {
        const bool* state = SDL_GetKeyboardState(nullptr);
        return state[(int)SDL_SCANCODE_LSHIFT] || state[(int)SDL_SCANCODE_RSHIFT];
    }

    void onUpdate(float delta) override
    {
        if(_vec.length() < 1e-8)
        {
            return;
        }

        move(_vec * delta);
    }

    void moveCamera(const Vec2& dir)
    {
        auto pos = getPos();
        //LogInfo("move camera ({}, {}), pos = ({}, {})", dir.x, dir.y, pos.x, pos.y);

        _vec = dir;
        _vec *= _speed;

        if (isShiftKeyDown())
        {
            _vec *= 3;
        }
    }

    void stopCamera()
    {
        //LogInfo("stop camera.");
        _vec = {0, 0};
    }

    void moveHome()
    {
        setPos({0, 0});
    }

    bool handleEvent(const Event& event) override
    {
        if(event.type == SDL_EVENT_KEY_DOWN)
        {
            switch(event.key.key)
            {
                case SDLK_LEFT: {
                    moveCamera({-1, 0});
                }break;
                case SDLK_RIGHT: {
                    moveCamera({1, 0});
                }break;
                case SDLK_UP: {
                    moveCamera({0, -1});
                }break;
                case SDLK_DOWN: {
                    moveCamera({0, 1});
                }break;
                case SDLK_SPACE: {
                    moveHome();
                }break;
                default:{
                }break;
            }
        }
        else if(event.type == SDL_EVENT_KEY_UP)
        {
            stopCamera();
        }

        return true;
    }

private:
    float _speed = 200.0f;
    Vec2f _vec = {0.0f, 0.0f};
};


}