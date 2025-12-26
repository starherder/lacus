#include "game_camera.h"
#include <namespaceapi.h>
#include "imform/imform_manager.h"


namespace game
{

    void GameCamera::init(Application* app)
    {
        spdlog::info("GameCamera::init");

        assert(app && "app is null");
        _application = app;

        app->eventDispatcher().onKeyDown.connect(this, &GameCamera::onKeyDown);
        app->eventDispatcher().onKeyUp.connect(this, &GameCamera::onKeyUp);
        app->eventDispatcher().onMouseLeftDrag.connect(this, &GameCamera::onMouseLeftDrag);
        app->eventDispatcher().onMouseWheel.connect(this, &GameCamera::onMouseWheel);
    }

    bool GameCamera::checkInputOK()
    {
        // 如果有调试用的ImGui窗口，要当心当前是不是在调试窗口上
        return imgui::ImFormManager::inst().isAnyWindowHovered() == false;
    }

    void GameCamera::onKeyDown(KeyCode keyCode)
    {
        spdlog::info("GameCamera::onKeyDown, keyCode = {}", (int)keyCode);

        switch (keyCode)
        {
        case SDLK_LEFT: {
            moveCamera({ -1, 0 });
        }break;
        case SDLK_RIGHT: {
            moveCamera({ 1, 0 });
        }break;
        case SDLK_UP: {
            moveCamera({ 0, -1 });
        }break;
        case SDLK_DOWN: {
            moveCamera({ 0, 1 });
        }break;
        case SDLK_SPACE: {
            moveHome();
        }break;
        default: {
        }break;
        }
    }

    void GameCamera::onKeyUp(KeyCode keyCode)
    {
        spdlog::info("GameCamera::onKeyDown, onKeyUp = {}", (int)keyCode);

        switch (keyCode)
        {
        case SDLK_LEFT: 
        case SDLK_RIGHT: {
            _vec.x = 0; 
        }break;
        case SDLK_UP:
        case SDLK_DOWN: {
            _vec.y = 0; 
        }break;
        default: {
        }break;
        }
    }

    void GameCamera::onMouseLeftDrag(const Vec2& pos, const Vec2& delta)
    {
        if(!checkInputOK()) return;

        spdlog::info("GameCamera::onMouseLeftDrag, pos = ({}, {}), delta = ({}, {})", 
            pos.x, pos.y, delta.x, delta.y);

        move(-delta);
    }
    
    void GameCamera::onMouseWheel(const Vec2& pos, float dir)
    {
        if(!checkInputOK()) return;

        spdlog::info("GameCamera::onMouseWheel, pos = ({}, {}), dir = {}", 
            pos.x, pos.y, dir);
    }

    void GameCamera::update(float deltaTime)
    {
        if (_vec.length() < 1e-8)
        {
            return;
        }

        move(_vec * deltaTime);
    }

    void GameCamera::moveCamera(const Vec2& dir)
    {
        _vec += dir;
        _vec = glm::normalize(_vec);
        _vec *= _speed;

        if (_application->eventDispatcher().isShiftKeyDown())
        {
            _vec *= 10;
        }

        auto& pos = getPos();
        spdlog::info("move camera ({}, {}), pos = ({}, {}), vec = ({}, {})", 
            dir.x, dir.y, pos.x, pos.y, _vec.x, _vec.y);
    }

    void GameCamera::stopCamera()
    {
        spdlog::info("stop camera.");
        _vec = { 0, 0 };
    }

    void GameCamera::moveHome()
    {
        setPos({ 0, 0 });
    }

}