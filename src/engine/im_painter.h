#pragma once

#include "engine/wrapper.h"
#include "utility/i_singleton.h"
#include "i_painter.h"

namespace engine
{
	class Application;
	class Texture;
	class Font;

	class ImPainter : public IPainter
	{
	public:
		ImPainter() = delete;
		ImPainter(ImPainter&&) = delete;
		ImPainter(const ImPainter&) = delete;

		ImPainter(Application& app);
		~ImPainter();

		void init() override;
		void quit() override;

		void preFrame() override;
		void postFrame() override;

		void setClearColor(const Color& color) override;

		void pushClipRect(const Rect& rect) override;
		void popClipRect() override;

		void fillRect(const Color& color, const Rect& rect, float round = 0) override;
		void drawRect(const Color& color, const Rect& rect, float round = 0, float thickness = 1) override;

		void drawCircle(const Color& color, const Vec2& center, float radius, int segments = 12, float thickness = 1.0f) override;
		void fillCircle(const Color& color, const Vec2& center, float radius, int segments = 12) override;

		void drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness = 1.0f) override;
		void fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3) override;

		void drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float thickness = 1) override;
		void drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness = 1.0f) override;

		void drawTexTile(TexTile* pTexTile, const Rect& dst, float round = 0.0f, const Color& color = Color::White) override;

		void drawTexture(Texture* pTexture, const Rect& src, const Rect& dst, float round = 0.0f, const Color& color = Color::White) override;
		void drawTextureUV(Texture* pTexture, const Rect& uv, const Rect& dst, float round = 0.0f, const Color& color = Color::White) override;

		void drawText(const std::string& text, Font* font, const Vec2& pos, 
			const Color& color = Color{ 255,255,255,255 }, float wrap_line = 0.0f) override;

		void drawGeometry(Texture* texture, const Vertex* vertices, int num_vertices, 
			const int* indices, int num_indices, const Vec2& pos = {0,0}, float scale=1.0f) override;

		void drawLightning(const Color& color, const Vec2& beginPos, const Vec2& endPos, 
			float displace, float min_displace=3, float thickness = 1) override;

		void makeLightningData(std::vector<Vec2>& data, const Vec2& beginPos, const Vec2& endPos, float displace, float min_displace = 3) override;
		void drawLightningData(const Color& color, const Vec2& pos, const std::vector<Vec2>& data, float thickness = 1.0f) override;

	private:
		void initDefaultFont(const std::string& fontName, int fontSize);

	};




}