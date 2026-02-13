#include "gfx_painter.h"
#include "application.h"
#include "texture.h"
#include "font.h"

#include "SDL3_gfx/SDL3_gfxPrimitives.h"

#define fail_return(p) if(!p) {return;}

namespace engine
{
	GfxPainter::GfxPainter(Application& app) : IPainter(app)
	{
	}

	GfxPainter::~GfxPainter()
	{
	}

	void GfxPainter::init()
	{
	}

	void GfxPainter::quit()
	{
	}

	void GfxPainter::preFrame()
	{
	}

	void GfxPainter::postFrame()
	{
	}

	void GfxPainter::setClearColor(const Color& color)
	{
		_application.renderer().setDrawColor(color);
	}

	void GfxPainter::pushClipRect(const Rect& rect)
	{
		_clipStack.push(rect);

		_application.renderer().setClipRect(rect);
	}

	void GfxPainter::popClipRect()
	{
		auto rect = _clipStack.top();
		_application.renderer().setClipRect(rect);

		_clipStack.pop();
	}

	void GfxPainter::fillRect(const Color& color, const Rect& rect, float round)
	{
		auto cint = color.toUint32();

		auto sdlRenderer = _application.renderer().getSdlRenderer();

		Vec2 trPoint = { rect.pos().x + rect.size().x, rect.pos().y};
		Vec2 lbPoint = { rect.pos().x, rect.pos().y + rect.size().y};

		if (round > 0)
		{
			roundedBoxRGBA(sdlRenderer, trPoint.x, trPoint.y, lbPoint.x, lbPoint.y, round, color.r, color.g, color.b, color.a);
		}
		else
		{
			boxRGBA(sdlRenderer, trPoint.x, trPoint.y, lbPoint.x, lbPoint.y, color.r, color.g, color.b, color.a);
		}
	}

	void GfxPainter::drawRect(const Color& color, const Rect& rect, float round, float thickness)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();

		Vec2 trPoint = { rect.pos().x + rect.size().x, rect.pos().y };
		Vec2 lbPoint = { rect.pos().x, rect.pos().y + rect.size().y };

		if (round > 0)
		{
			roundedRectangleRGBA(sdlRenderer, trPoint.x, trPoint.y, lbPoint.x, lbPoint.y, round, color.r, color.g, color.b, color.a);
		}
		else
		{
			rectangleRGBA(sdlRenderer, trPoint.x, trPoint.y, lbPoint.x, lbPoint.y, color.r, color.g, color.b, color.a);
		}
	}

	void GfxPainter::drawCircle(const Color& color, const Vec2& center, float radius, int segment, float thickness)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();

		if (_antiAliased) 
		{
			aacircleRGBA(sdlRenderer, center.x, center.y, radius, color.r, color.g, color.b, color.a);
		}
		else 
		{
			circleRGBA(sdlRenderer, center.x, center.y, radius, color.r, color.g, color.b, color.a);
		}
	}

	void GfxPainter::fillCircle(const Color& color, const Vec2& center, float radius, int segment)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		filledCircleRGBA(sdlRenderer, center.x, center.y, radius, color.r, color.g, color.b, color.a);
	}

	void GfxPainter::drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();

		if (_antiAliased) 
		{
			aatrigonRGBA(sdlRenderer, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color.r, color.g, color.b, color.a);
		}
		else 
		{
			trigonRGBA(sdlRenderer, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color.r, color.g, color.b, color.a);
		}
	}

	void GfxPainter::fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		filledTrigonRGBA(sdlRenderer, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color.r, color.g, color.b, color.a);
	}

	void GfxPainter::drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float lineWidth)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();

		if (lineWidth > 0.0f) 
		{
			thickLineRGBA(sdlRenderer, beginPos.x, beginPos.y, endPos.x, endPos.y, lineWidth, color.r, color.g, color.b, color.a);
		}
		else
		{
			if (_antiAliased)
			{
				aalineRGBA(sdlRenderer, beginPos.x, beginPos.y, endPos.x, endPos.y, color.r, color.g, color.b, color.a);
			}
			else
			{
				lineRGBA(sdlRenderer, beginPos.x, beginPos.y, endPos.x, endPos.y, color.r, color.g, color.b, color.a);
			}
		}
	}

	void GfxPainter::drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness)
	{
		// TODO: 
	}

	void GfxPainter::drawTexTile(TexTile* pTexTile, const Rect& dst, float radian, float round, const Color& color)
	{
		drawTexture(pTexTile->texture(), pTexTile->rect(), dst, round, color);
	}

	void GfxPainter::drawTexture(Texture* pTexture, const Rect& src, const Rect& dst, float round, const Color& color)
	{
		fail_return(pTexture);

		_application.renderer().drawTexture(pTexture, src, dst);
	}

	void GfxPainter::drawTextureUV(Texture* pTexture, const Rect& uv, const Rect& dst, float round, const Color& color)
	{
		fail_return(pTexture);

		auto sz = pTexture->size();
		Rect src = { uv.x * sz.x, uv.y * sz.y, uv.w * sz.x, uv.h * sz.y };
		_application.renderer().drawTexture(pTexture, src, dst);
	}

	void GfxPainter::drawGeometry(Texture* texture, const Vertex* vertices, int num_vertices,
								const int* indices, int num_indices, const Vec2& pos, float scale)
	{
		fail_return(texture);

		_application.renderer().drawGeometry(texture, vertices, num_vertices, indices, num_indices);
	}

	void GfxPainter::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color, float wrap_line)
	{
		_application.renderer().drawText(text, font, pos, color);
	}

	void GfxPainter::setAntiAlaised(bool anti_aliased)
	{
		_antiAliased = anti_aliased;
	}

}