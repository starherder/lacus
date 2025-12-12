#pragma once

#include "wrapper.h"

namespace engine {

	class Camera 
	{
	public:
        Camera() = delete;
        Camera(Camera&&) = delete;
        Camera(const Camera&) = delete;

        Camera(const Vec2i& size);

		void Move(const Vec2i& dis);

		Vec2i SceneLTCorner();
		Vec2i SceneRBCorner();

		Vec2i GetPos() const;
		void SetPos(const Vec2i& pos);

		Vec2i GetSize() const;
		void SetSize(const Vec2i& size);

		const Rect& GetRect() const;
		void SetRect(const Rect& rect);

		Vec2i WorldToScreen(const Vec2i& pos) const;
		Vec2i ScreenToWorld(const Vec2i& pos) const;

		const Vec2f& GetScale() const;
		void SetScale(const Vec2f& scale);

        float GetRotate() const;
		void SetRotate(float rotate);

        virtual void Update(float delta) {}
        virtual bool HandleEvent(const Event& event) { return true; }

	private:
		Rect _rect = {0,0,0,0};

		// 先不管缩放旋转了
		Vec2f _scale;
		float _rotate;
	};


}