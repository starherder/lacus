#pragma once

#include "wrapper.h"
#include "tweeny/tweeny.h"

namespace engine {

	class Camera 
	{
	public:
        Camera(Camera&&) = delete;
        Camera(const Camera&) = delete;

		Camera() = default;
        Camera(const Vec2& pos, const Vec2& size);

		void move(const Vec2& dis) { setPos(_pos + dis); }

		void setLimitArea(bool limit, const Rect& area);

		const Vec2& getPos() const { return _pos; }
		void setPos(const Vec2& pos);

		const Vec2& getSize() const { return _size; }
		void setSize(const Vec2& size);

		const Vec2& getViewSize() const { return _size; }
		void setViewSize(const Vec2& size) { setSize(size); }

		const Vec2& getScreenSize() const { return _screenSize; }
		void setScreenSize(const Vec2& size);

		float getViewScale() const { return _viewScale; }
		const Vec2& getViewOffset() const { return _viewOffset; }

		Vec2 worldToScreen(const Vec2& pos) const;
		Vec2 screenToWorld(const Vec2& pos) const;

        void update(float delta);

		virtual void onUpdate(float delta) {}

        virtual bool handleEvent(const Event& event);

		Vec2 projectPoint(const Vec2& point) const;
		void projectVertices(std::vector<Vertex>& verts) const;

		Rect projectRect(const Rect& rect) const;
		void projectRects(Rect* rect, size_t count) const;

		void shake(int duration, int frequency, int ampl);

	private:
		void updateViewTransform();

	private:
		Vec2 _pos = {0, 0};
		Vec2 _size = {1280, 1024};
		Vec2 _screenSize = {1280, 1024};
		float _viewScale = 1.0f;
		Vec2 _viewOffset = {0, 0};

		bool _limitInArea = false;
		Rect _limitArea;

		bool _shaking = false;

		tweeny::tween<float, float> _cameraTween;

		// 先不管旋转了
		//float _rotate;
	};


}