#pragma once

#include "engine/wrapper.h"
#include "utility/i_singleton.h"
#include "imgui/imgui.h"

namespace engine
{
	class Application;
	class Texture;
	class Font;

	class ImPainter
	{
	public:
		ImPainter() = delete;
		ImPainter(ImPainter&&) = delete;
		ImPainter(const ImPainter&) = delete;

		ImPainter(Application& app);
		~ImPainter();

		void init();
		void quit();

		void preFrame();
		void postFrame();

		// ImU32表示颜色时，顺序是AGBR
		ImColor toImColor(const Color& color);

		void setClearColor(const Color& color);

		void pushClipRect(const Rect& rect);
		void popClipRect();

		void fillRect(const Color& color, const Rect& rect, float round = 0);
		void drawRect(const Color& color, const Rect& rect, float round = 0, float thickness = 1);

		void drawCircle(const Color& color, const Vec2& center, float radius, int segments = 12, float thickness = 1.0f);
		void fillCircle(const Color& color, const Vec2& center, float radius, int segments = 12);

		void drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness = 1.0f);
		void fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3);

		void drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float thickness = 1);
		void drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness = 1.0f);

		void drawTexture(Texture* pTexture, const Rect& uv, const Rect& dst);

		void drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color = Color{ 255,255,255,255 });

	private:
		void initDefaultFont(const std::string& fontName, int fontSize);

	private:
		Application& _application;

	};




}