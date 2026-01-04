#pragma once

#include "engine/wrapper.h"
#include "utility/i_singleton.h"


namespace engine
{
	class Application;
	class Texture;
	class Font;

	class Painter
	{
	public:
		Painter() = delete;
		Painter(Painter&&) = delete;
		Painter(const Painter&) = delete;

		Painter(Application& app);
		~Painter();

		void setClearColor(const Color& color);

		void fillRect(const Color& color, Rect rect, float round = 0);
		void drawRect(const Color& color, Rect rect, float round = 0, float lineWidth = 1);

		void drawCircle(const Color& color, const Vec2& center, float radius, int segments = 12, float thickness = 1.0f);
		void fillCircle(const Color& color, const Vec2& center, float radius, int segments = 12);

		void drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness = 1.0f);
		void fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3);

		void drawLine(const Color& color, Vec2 beginPos, Vec2 endPos, float lineWidth = 1);
		void drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness = 1.0f);

		void drawTexture(Texture* pTexture, const Rect& uv, const Rect& dst);

		void drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color = Color{ 255,255,255,255 });

		void setClipRect(const Rect& rect);
		Rect getClipRect();

		bool isAntiAlaised() const { return _antiAliased; }
		void setAntiAlaised(bool anti_aliased = true);

	private:
		void pathLineTo(const Vec2& pos);

		void pathArcTo(const Vec2& centre, float radius, float min, float max, int segments = 10);

		void pathArcToFast(const Vec2& centre, float radius, int a_min_of_12, int a_max_of_12);

		void pathRect(const Vec2& tl, const Vec2& br, float rounding = 0.0f);

		void primRect(const Color& color, const Vec2& tl, const Vec2& br);

		void strokePolyline(const Color& color, bool closed, float thickness);

		void strokeConvexPolyFilled(const Color& color);

		void drawPath();

	private:
		Application& _application;

		Rect _clipRect;

		std::vector<Vec2> _vPaths;

		std::vector<Vertex> _geometryVertices;
		std::vector<int> _geometryIndices;

		bool _antiAliased = false;
	};




}