#include "camera.h"


namespace engine {

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

}