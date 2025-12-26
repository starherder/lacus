#pragma once

#include "engine/camera.h"
#include "engine/application.h"

namespace game 
{
using namespace engine;

class GameCamera : public Camera, public signals::SlotHandler
{
public:
    GameCamera() = default;
    ~GameCamera() = default;
    GameCamera(const Vec2& pos, const Vec2& size) 
        : Camera(pos, size) {}
    
    void init(Application* app);

    void update(float delta) override;

private:
    void onKeyDown(KeyCode keyCode);

    void onKeyUp(KeyCode keyCode);
    
    void onMouseLeftDrag(const Vec2& pos, const Vec2& delta);

    void onMouseWheel(const Vec2& pos, float dir);

    void moveCamera(const Vec2& dir);

    void stopCamera();

    void moveHome();

    bool checkInputOK();
    
private:
    Application* _application = nullptr;
    
    float _speed = 200.0f;
    Vec2 _vec = {0.0f, 0.0f};
};



}