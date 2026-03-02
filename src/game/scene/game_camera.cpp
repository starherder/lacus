#include "game_camera.h"
#include <namespaceapi.h>
#include "imform/imform_manager.h"


namespace game
{

    void GameCamera::init(Application* app)
    {
        LogInfo("GameCamera::init");

        assert(app && "app is null");
        _application = app;

        setSize(_application->window().getSize());

        app->eventDispatcher().onWindowResized.connect(this, &GameCamera::onWindowResize);
        app->eventDispatcher().onKeyDown.connect(this, &GameCamera::onKeyDown);
        app->eventDispatcher().onKeyUp.connect(this, &GameCamera::onKeyUp);
        app->eventDispatcher().onMouseLeftDrag.connect(this, &GameCamera::onMouseLeftDrag);
        app->eventDispatcher().onMouseWheel.connect(this, &GameCamera::onMouseWheel);
    }

    bool GameCamera::checkInputOK()
    {
        return true;
    }

    void GameCamera::onKeyDown(KeyCode keyCode)
    {
        //LogInfo("GameCamera::onKeyDown, keyCode = {}", (int)keyCode);

        if (_moveByKey)
        {
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
    }

    void GameCamera::onKeyUp(KeyCode keyCode)
    {
        //LogInfo("GameCamera::onKeyDown, onKeyUp = {}", (int)keyCode);

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
        if (!_dragMode) return;

        if(!checkInputOK()) return;

        //LogInfo("GameCamera::onMouseLeftDrag, pos = ({}, {}), delta = ({}, {})", 
        //    pos.x, pos.y, delta.x, delta.y);

        move(-delta);
    }
    
    void GameCamera::onMouseWheel(const Vec2& pos, float dir)
    {
        if(!checkInputOK()) return;

        //LogInfo("GameCamera::onMouseWheel, pos = ({}, {}), dir = {}", 
        //    pos.x, pos.y, dir);
    }

    void GameCamera::onWindowResize(const Vec2& sz)
    {
        setSize(sz);
    }

    void GameCamera::onUpdate(float deltaTime)
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
        _vec = SafeNormal(_vec);
        _vec *= _speed;

        if (_application->eventDispatcher().isShiftKeyDown())
        {
            _vec *= 10;
        }
    }

    void GameCamera::stopCamera()
    {
        _vec = { 0, 0 };
    }

    void GameCamera::moveHome()
    {
        setPos({ 0, 0 });
    }

}