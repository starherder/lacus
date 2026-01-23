#pragma once

#include "engine/wrapper.h"

namespace engine
{
	class Application;
	class Texture;
	class Font;

	class IPainter
	{
	public:
		IPainter() = delete;
		IPainter(IPainter&&) = delete;
		IPainter(const IPainter&) = delete;

		IPainter(Application& app) : _application(app) {  }
		virtual ~IPainter() {}

		virtual void init() = 0;
		virtual void quit() = 0;
		 
		virtual void preFrame() {}
		virtual void postFrame() {}
		 
		virtual void setClearColor(const Color& color) = 0;
		 
		virtual void pushClipRect(const Rect& rect) = 0;
		virtual void popClipRect() = 0;
		 
		virtual void fillRect(const Color& color, const Rect& rect, float round = 0) = 0;
		virtual void drawRect(const Color& color, const Rect& rect, float round = 0, float thickness = 1) = 0;
		 
		virtual void drawCircle(const Color& color, const Vec2& center, float radius, int segments = 12, float thickness = 1.0f) = 0;
		virtual void fillCircle(const Color& color, const Vec2& center, float radius, int segments = 12) = 0;
		 
		virtual void drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness = 1.0f) = 0;
		virtual void fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3) = 0;
		 
		virtual void drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float thickness = 1) = 0;
		virtual void drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness = 1.0f) = 0;
		 
		virtual void drawTexture(Texture* pTexture, const Rect& src, const Rect& dst, float round = 0.0f, const Color& color = Color::White) = 0;
		virtual void drawTextureUV(Texture* pTexture, const Rect& uv, const Rect& dst, float round = 0.0f, const Color& color = Color::White) = 0;
		 
		virtual void drawText(const std::string& text, Font* font, const Vec2& pos, 
		 					const Color& color = Color{ 255,255,255,255 }, float wrap_line = 0.0f) = 0;
		 
		virtual void drawGeometry(Texture* texture, const Vertex* vertices, int num_vertices, 
							const int* indices, int num_indices, const Vec2& pos = {0,0}, float scale=1.0f) = 0;

		virtual void drawLightning(const Color& color, const Vec2& beginPos, const Vec2& endPos,
							float displace, float min_displace = 3, float thickness = 1) {}
		virtual void makeLightningData(std::vector<Vec2>& data, const Vec2& beginPos, const Vec2& endPos, float displace, float min_displace = 3) {}
		virtual void drawLightningData(const Color& color, const std::vector<Vec2>& data, float thickness = 1.0f) {}

	protected:
		Application& _application;

	};




}