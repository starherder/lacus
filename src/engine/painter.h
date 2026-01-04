#pragma once

#include "wrapper.h"
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
		void drawRect(const Color& color, Rect rect, float round = 0);

		void drawCircle(const Color& color, const Vec2& center, float radius);
		void fillCircle(const Color& color, const Vec2& center, float radius);

		void drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3);
		void fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3);

		void drawPixel(const Color& color, Vec2 pos);

		void drawLine(const Color& color, Vec2 beginPos, Vec2 endPos, float lineWidth = 1);

		// startAngle, endAngle: angle in degreen, 0 degress is down, increasing cuonter clockwise.
		void drawArc(const Color& color, const Vec2& center, float radius, int startAngle, int endAngle);
		void drawPie(const Color& color, const Vec2& center, float radius, int startAngle, int endAngle);
		void fillPie(const Color& color, const Vec2& center, float radius, int startAngle, int endAngle);

		// radius: horizonal & vertical radius
		void drawEllipse(const Color& color, const Vec2& center, const Vec2& radius);
		void fillEllipse(const Color& color, const Vec2& center, const Vec2& radius);

		void drawTexture(Texture* pTexture, const Rect& uv, const Rect& dst);

		void drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color = Color{ 255,255,255,255 });

		void setClipRect(const Rect& rect);
		Rect getClipRect();

		bool isAntiAlaised() const { return _antiAliased; }
		void setAntiAlaised(bool anti_aliased = true);

	private:
		Application& _application;

		std::vector<Vertex> _geometryVertices;

		std::vector<int> _geometryIndices;

		bool _antiAliased = false;

		Rect _clipRect;

	};
	



}