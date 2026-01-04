

#pragma once

#include "wrapper.h"
#include "utility/i_singleton.h"


namespace engine 
{
	class Application;
	class Texture;
	class Font;

	// TODO: something wrong in anti-alaised mode, like fillCirle
	// use imgui-draw instead after

	class Painter 
	{
	public:
		Painter() = delete;
		Painter(Painter&&) = delete;
		Painter(const Painter&) = delete;

		Painter(Application& app);
		~Painter();

		void SetClearColor(const Color& color);

		void FillRect(const Color& color, Rect rect, float round = 0);
		void DrawRect(const Color& color, Rect rect, float round = 0, float lineWidth = 1);

		void DrawCircle(const Color& color, const Vec2& center, float radius, int segments = 12, float thickness = 1.0f);
		void FillCircle(const Color& color, const Vec2& center, float radius, int segments = 12);

		void DrawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness = 1.0f);
		void FillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3);

		void DrawLine(const Color& color, Vec2 beginPos, Vec2 endPos, float lineWidth = 1);
		void DrawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness = 1.0f);

		void drawTexture(Texture* pTexture, const Rect& uv, const Rect& dst);

		void drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color = Color{ 255,255,255,255 });

		void SetClipRect(const Rect& rect);
		Rect GetClipRect();

		bool IsAntiAlaised() const { return m_bAntiAliased; }
		void SetAntiAlaised(bool anti_aliased = true);

	private:
		void PathLineTo(const Vec2& pos);

		void PathArcTo(const Vec2& centre, float radius, float min, float max, int segments = 10);

		void PathArcToFast(const Vec2& centre, float radius, int a_min_of_12, int a_max_of_12);

		void PathRect(const Vec2& tl, const Vec2& br, float rounding = 0.0f);

		void PrimRect(const Color& color, const Vec2& tl, const Vec2& br);

		void StrokePolyline(const Color& color, bool closed, float thickness);

		void StrokeConvexPolyFilled(const Color& color);

		void DrawPath();

	private:
		Application& _application;

		SDL_Window* m_pWindow = nullptr;

		Rect m_ClipRect;

		std::vector<Vec2> m_vPaths;

		std::vector<Vertex> m_GeometryVertices;
		std::vector<int> m_vGeometryIndices;

		bool m_bAntiAliased = false;
	};
	



}