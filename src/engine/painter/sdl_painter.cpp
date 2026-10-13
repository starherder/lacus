#include "sdl_painter.h"
#include "engine/application.h"
#include "engine/texture.h"
#include "engine/font.h"

#include <cmath>

#define fail_return(p) if(!p) {return;}

namespace engine
{
	namespace
	{
		constexpr float kEpsilon = 0.000001f;

		Vertex makeVertex(const Vec2& pos, const Color& color, const Vec2& uv = { 0.0f, 0.0f })
		{
			Vertex vertex{};
			vertex.position = ToPoint(pos);
			vertex.color = color;
			vertex.tex_coord = ToPoint(uv);
			return vertex;
		}

		std::vector<Vec2> makeCirclePoints(const Vec2& center, float radius, int segments, bool closed)
		{
			std::vector<Vec2> points;
			if (radius <= 0.0f)
			{
				return points;
			}

			segments = segments <= 0 ? std::clamp((int)(radius * 0.5f), 12, 64) : std::max(segments, 3);
			points.reserve(segments + (closed ? 1 : 0));

			for (int i = 0; i < segments; ++i)
			{
				const float angle = ((float)i / (float)segments) * PI_2;
				points.push_back({ center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius });
			}

			if (closed)
			{
				points.push_back(points.front());
			}

			return points;
		}
	}

	SdlPainter::SdlPainter(Application& app) : IPainter(app)
	{
	}

	SdlPainter::~SdlPainter()
	{
	}

	void SdlPainter::init()
	{
	}

	void SdlPainter::quit()
	{
		_clipStack.clear();
	}

	void SdlPainter::setClearColor(const Color& color)
	{
		_application.renderer().setDrawColor(color);
	}

	void SdlPainter::pushClipRect(const Rect& rect)
	{
		Rect clip = rect;
		if (!_clipStack.empty())
		{
			clip = _clipStack.back().intersect(rect);
		}

		_clipStack.push_back(clip);
		applyClipRect();
	}

	void SdlPainter::popClipRect()
	{
		if (_clipStack.empty())
		{
			return;
		}

		_clipStack.pop_back();
		applyClipRect();
	}

	void SdlPainter::fillRect(const Color& color, const Rect& rect, float round)
	{
		_application.renderer().setDrawColor(color);
		_application.renderer().drawFillRect(rect);
	}

	void SdlPainter::drawRect(const Color& color, const Rect& rect, float round, float thickness)
	{
		_application.renderer().setDrawColor(color);
		_application.renderer().drawRect(rect);
	}

	void SdlPainter::drawCircle(const Color& color, const Vec2& center, float radius, int segments, float thickness)
	{
		auto points = makeCirclePoints(center, radius, segments, true);
		drawLines(color, points.data(), (int)points.size(), false, thickness);
	}

	void SdlPainter::fillCircle(const Color& color, const Vec2& center, float radius, int segments)
	{
		auto points = makeCirclePoints(center, radius, segments, false);
		if (points.size() < 3)
		{
			return;
		}

		std::vector<Vertex> vertices;
		std::vector<int> indices;
		vertices.reserve(points.size() + 1);
		indices.reserve(points.size() * 3);

		vertices.push_back(makeVertex(center, color));
		for (const Vec2& point : points)
		{
			vertices.push_back(makeVertex(point, color));
		}

		for (int i = 0; i < (int)points.size(); ++i)
		{
			indices.push_back(0);
			indices.push_back(i + 1);
			indices.push_back((i + 1) % (int)points.size() + 1);
		}

		_application.renderer().drawGeometry(nullptr, vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size());
	}

	void SdlPainter::drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness)
	{
		Vec2 points[] = { p1, p2, p3, p1 };
		drawLines(color, points, 4, false, thickness);
	}

	void SdlPainter::fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3)
	{
		Vertex vertices[] = {
			makeVertex(p1, color),
			makeVertex(p2, color),
			makeVertex(p3, color)
		};
		int indices[] = { 0, 1, 2 };
		_application.renderer().drawGeometry(nullptr, vertices, 3, indices, 3);
	}

	void SdlPainter::drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float thickness)
	{
		_application.renderer().setDrawColor(color);
		_application.renderer().drawLine(beginPos, endPos);
	}

	void SdlPainter::drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness)
	{
		if (!points || point_count <= 1)
		{
			return;
		}

		_application.renderer().setDrawColor(color);
		if (!closed)
		{
			_application.renderer().drawlines(points, point_count);
			return;
		}

		std::vector<Vec2> closedPoints(points, points + point_count);
		closedPoints.push_back(points[0]);
		_application.renderer().drawlines(closedPoints.data(), (int)closedPoints.size());
	}

	void SdlPainter::drawTexTile(TexTile* pTexTile, const Rect& dst, float radian, float round, const Color& color)
	{
		fail_return(pTexTile);
		fail_return(pTexTile->texture());

		if (std::abs(radian) <= kEpsilon)
		{
			drawTexture(pTexTile->texture(), pTexTile->rect(), dst, round, color);
			return;
		}

		const Vec2 center = dst.size() * pTexTile->center();
		_application.renderer().drawTextureRotated(pTexTile->texture(), pTexTile->rect(), dst, glm::degrees(radian), center, SDL_FLIP_NONE);
	}

	void SdlPainter::drawTexture(Texture* pTexture, const Rect& src, const Rect& dst, float round, const Color& color)
	{
		fail_return(pTexture);
		_application.renderer().drawTexture(pTexture, src, dst);
	}

	void SdlPainter::drawTextureUV(Texture* pTexture, const Rect& uv, const Rect& dst, float round, const Color& color)
	{
		fail_return(pTexture);

		const Vec2 size = pTexture->size();
		Rect src = { uv.x * size.x, uv.y * size.y, uv.w * size.x, uv.h * size.y };
		drawTexture(pTexture, src, dst, round, color);
	}

	void SdlPainter::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color, float wrap_line)
	{
		fail_return(font);
		_application.renderer().drawText(text, font, pos, color);
	}

	void SdlPainter::drawGeometry(Texture* texture, const Vertex* vertices, int num_vertices,
		const int* indices, int num_indices, const Vec2& pos, float scale)
	{
		if (!vertices || num_vertices <= 0)
		{
			return;
		}

		if (pos == Vec2{ 0.0f, 0.0f } && scale == 1.0f)
		{
			_application.renderer().drawGeometry(texture, vertices, num_vertices, indices, num_indices);
			return;
		}

		std::vector<Vertex> transformed(vertices, vertices + num_vertices);
		for (Vertex& vertex : transformed)
		{
			vertex.position.x = pos.x + vertex.position.x * scale;
			vertex.position.y = pos.y + vertex.position.y * scale;
		}

		_application.renderer().drawGeometry(texture, transformed.data(), (int)transformed.size(), indices, num_indices);
	}

	Rect SdlPainter::defaultClipRect() const
	{
		const Vec2i size = _application.window().getSize();
		return { 0.0f, 0.0f, (float)size.x, (float)size.y };
	}

	Rect SdlPainter::currentClipRect() const
	{
		return _clipStack.empty() ? defaultClipRect() : _clipStack.back();
	}

	void SdlPainter::applyClipRect() const
	{
		_application.renderer().setClipRect(currentClipRect());
	}
}
