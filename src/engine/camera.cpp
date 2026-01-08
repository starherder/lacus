#include "camera.h"
#include "tweeny/tweeny.h"

namespace engine {

    tweeny::tween<float, float> g_cameraTween;

    Vec2 Camera::worldToScreen(const Vec2& pos) const 
    { 
        return pos - _pos; 
    }

    Vec2 Camera::screenToWorld(const Vec2& pos) const 
    { 
        return pos + _pos; 
    }

    void Camera::update(float delta) 
    {
        if (_shaking)
        {
            g_cameraTween.step(delta);
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

    void Camera::projectVertexies(std::vector<Vertex>& verts) const 
    {
        for (auto& vert : verts) {
            auto pos = worldToScreen({vert.position.x, vert.position.y});
            vert.position.x = pos.x;
            vert.position.y = pos.y;
        }
    }

    Rect Camera::projectRect(const Rect& rect) const
    {
        // auto sz = rect.size() * _scale;
        auto sz = rect.size();
        auto pos = worldToScreen(rect.pos());
        return {pos, sz};
    }
    
	void Camera::projectRects(Rect* rect, size_t count) const
    {
        for(auto i=0; i<count; ++i) {
            rect[i] = projectRect(rect[i]);
        }
    }

    void Camera::shake(float seconds, int frequency, int ampl)
    {
        if (seconds <= 0 || frequency <= 0 || ampl <= 0)
        {
            spdlog::info("camera::shake: param <= 0");
            return;
        }

        int count = (int)(frequency * seconds);

        int ms = (int)(seconds * 1000) / count;

        g_cameraTween = tweeny::from(_pos.x, _pos.y);

        for (int i = 0; i < count; i++)
        {
            Vec2 pos = _pos + Vec2{ utility::rand_minus1_1(), utility::rand_minus1_1() } * (float)ampl;
            g_cameraTween.to(pos.x, pos.y).via("linear").during(ms);
        }

        g_cameraTween.onStep([this](auto& t, auto x, auto y)
        {
            if (t.isFinished())
            {
                _shaking = false;
                return true;
            }

            _pos = { x, y };
            return false;
        });

        _shaking = true;
    }
}