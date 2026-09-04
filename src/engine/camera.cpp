#include "camera.h"

namespace engine {

    Camera::Camera(const Vec2& pos, const Vec2& size)
    { 
        _pos = pos; 
        _size = size;
        _screenSize = size;
        updateViewTransform();

        _limitInArea = false;
        _limitArea = Rect{_pos, _size};
    }

    void Camera::setPos(const Vec2& pos)
    {
        if(_limitInArea)
        {
            if(!_limitArea.contains(Rect{ pos, _size }))
            {
                return;
            }
        }

        _pos = pos;
    }

    void Camera::setSize(const Vec2& size)
    {
        if (_limitInArea)
        {
            if (!_limitArea.contains(Rect{ _pos, size }))
            {
                return;
            }
        }

        _size = size;
        updateViewTransform();
    }

    void Camera::setScreenSize(const Vec2& size)
    {
        _screenSize = size;
        updateViewTransform();
    }

    void Camera::updateViewTransform()
    {
        if (_size.x <= 0.0f || _size.y <= 0.0f || _screenSize.x <= 0.0f || _screenSize.y <= 0.0f)
        {
            _viewScale = 1.0f;
            _viewOffset = {0, 0};
            return;
        }

        auto scaleX = _screenSize.x / _size.x;
        auto scaleY = _screenSize.y / _size.y;
        _viewScale = std::min(scaleX, scaleY);

        auto displaySize = _size * _viewScale;
        _viewOffset = (_screenSize - displaySize) * 0.5f;
    }

    void Camera::setLimitArea(bool limit, const Rect& area)
    {
        _limitInArea = limit;
        _limitArea = area;

        setPos(_pos);
    }

    Vec2 Camera::worldToScreen(const Vec2& pos) const 
    { 
        return (pos - _pos) * _viewScale + _viewOffset; 
    }

    Vec2 Camera::screenToWorld(const Vec2& pos) const 
    { 
        return (pos - _viewOffset) / _viewScale + _pos; 
    }

    void Camera::update(float delta) 
    {
        if (_shaking)
        {
            _cameraTween.step((int)(delta*1000));
        }

        onUpdate(delta);
    }
    
    bool Camera::handleEvent(const Event& event) 
    { 
        return true; 
    }

	Vec2 Camera::projectPoint(const Vec2& point) const
    {
        return worldToScreen(point);
    }

    void Camera::projectVertices(std::vector<Vertex>& verts) const 
    {
        for (auto& vert : verts) 
        {
            auto pos = worldToScreen({vert.position.x, vert.position.y});
            vert.position.x = pos.x;
            vert.position.y = pos.y;
        }
    }

    Rect Camera::projectRect(const Rect& rect) const
    {
        auto sz = rect.size() * _viewScale;
        auto pos = worldToScreen(rect.pos());
        return {pos, sz};
    }
    
	void Camera::projectRects(Rect* rect, size_t count) const
    {
        for(auto i=0; i<count; ++i) {
            rect[i] = projectRect(rect[i]);
        }
    }

    void Camera::shake(int duration, int frequency, int ampl)
    {
        if (duration <= 0 || frequency <= 0 || ampl <= 0)
        {
            LogInfo("camera::shake: param <= 0");
            return;
        }

        float fduration = static_cast<float>(duration);
        int count = static_cast<int>(frequency * (fduration / 1000.0f));
        int period = static_cast<int>(fduration / count);

        _cameraTween = tweeny::from(_pos.x, _pos.y);

        for (int i = 0; i < count; i++)
        {
            Vec2 pos = _pos + Vec2{ utility::rand_minus1_1(), utility::rand_minus1_1() } * (float)ampl;
            _cameraTween.to(pos.x, pos.y).via("linear").during(period);
        }

        _cameraTween.onStep([this, origin_pos=_pos](auto& t, auto x, auto y)
        {
            if (t.isFinished())
            {
                _pos = origin_pos;
                _shaking = false;
                return true;
            }

            _pos = { x, y };
            return false;
        });

        _shaking = true;
    }
}