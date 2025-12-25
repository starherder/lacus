#pragma once

#include "wrapper.h"

namespace engine {

	class Camera 
	{
	public:
        Camera(Camera&&) = delete;
        Camera(const Camera&) = delete;

		Camera() = default;
        Camera(const Vec2& pos, const Vec2& size) { _pos = pos; _size = size; }

		void move(const Vec2& dis) { _pos += dis; }

		const Vec2& getPos() const { return _pos; }
		void setPos(const Vec2& pos) { _pos = pos; }

		const Vec2& getSize() const { return _size; }
		void setSize(const Vec2& size) { _size = size; }

		Vec2 worldToScreen(const Vec2& pos) const;
		Vec2 screenToWorld(const Vec2& pos) const;

        virtual void update(float delta);
        virtual bool handleEvent(const Event& event);

		Vec2 projectPoint(const Vec2& point) const;
		void projectVertexies(std::vector<Vertex>& verts) const;

		Rect projectRect(const Rect& rect) const;
		void projectRects(Rect* rect, size_t count) const;

		//const Vec2& getScale() const { return _scale; }
		//void setScale(const Vec2& scale) { _scale = scale; }

        //float getRotate() const { return _rotate; }
		//void setRotate(float rotate) { _rotate = rotate; }

	private:
		Vec2 _pos = {0, 0};
		Vec2 _size = {1280, 1024};

		// 先不管缩放旋转了
		//Vec2 _scale;
		//float _rotate;
	};


}