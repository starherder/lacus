#pragma once

#include "engine/wrapper.h"
#include "utility/i_singleton.h"
#include "i_painter.h"
#include <stack>

namespace engine
{
	class Application;
	class Texture;
	class Font;

	class Painter : public IPainter
	{
	public:
		Painter() = delete;
		Painter(Painter&&) = delete;
		Painter(const Painter&) = delete;

		Painter(Application& app);
		~Painter();

		void init() override;
		void quit() override;

		void preFrame() override;
		void postFrame() override;

		void setClearColor(const Color& color) override;

		void pushClipRect(const Rect& rect) override;
		void popClipRect() override;

		void fillRect(const Color& color, const Rect& rect, float round = 0) override;
		void drawRect(const Color& color, const Rect& rect, float round = 0, float lineWidth = 1) override;

		void drawCircle(const Color& color, const Vec2& center, float radius, int segments = 12, float thickness = 1.0f) override;
		void fillCircle(const Color& color, const Vec2& center, float radius, int segments = 12) override;

		void drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness = 1.0f) override;
		void fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3) override;

		void drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float thickness = 1) override;
		void drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness = 1.0f) override;

		void drawTexture(Texture* pTexture, const Rect& src, const Rect& dst, float round = 0.0f, const Color& color = Color::White) override;
		void drawTextureUV(Texture* pTexture, const Rect& uv, const Rect& dst, float round = 0.0f, const Color& color = Color::White) override;

		void drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color = Color{ 255,255,255,255 }, float wrap_line=0.0f) override;

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
		std::stack<Rect> _clipStack;

		std::vector<Vec2> _vPaths;

		std::vector<Vertex> _geometryVertices;
		std::vector<int> _geometryIndices;

		bool _antiAliased = false;
	};




}