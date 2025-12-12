#include "camera.h"


namespace engine {

    
    Camera::Camera(const Vec2i& size)
    {
        _rect.w = size.x;
        _rect.h = size.y;
    }

	void Camera::Move(const Vec2i& dis)
	{
		_rect.x += dis.x;
		_rect.y += dis.y;
	}
	
	Vec2i Camera::SceneLTCorner()
	{
		return { _rect.x, _rect.y };
	}

	Vec2i Camera::SceneRBCorner()
	{
		return { _rect.x + _rect.w, _rect.y + _rect.h};
	}

    Vec2i Camera::GetPos() const
    {
        return {_rect.x, _rect.y};
    }

	void Camera::SetPos(const Vec2i& pos)
	{
		_rect.x = pos.x;
		_rect.y = pos.y;
	}

    Vec2i Camera::GetSize() const 
    {
        return {_rect.w, _rect.h};
    }

	void Camera::SetSize(const Vec2i& size)
	{
		_rect.w = size.x;
		_rect.h = size.y;
	}

    const Vec2f& Camera::GetScale() const
    {
        return _scale;
    }

    void Camera::SetScale(const Vec2f& scale)
    {
        _scale = scale;
    }

	Vec2i Camera::WorldToScreen(const Vec2i& pos) const
	{
		return { pos.x - _rect.x, pos.y - _rect.y };
	}

	Vec2i Camera::ScreenToWorld(const Vec2i& pos) const
	{
		return { pos.x + _rect.x, pos.y + _rect.y };
	}

}