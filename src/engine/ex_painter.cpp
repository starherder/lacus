#include "ex_Painter.h"
#include "application.h"
#include "texture.h"
#include "font.h"

#include "SDL3_gfx/SDL3_gfxPrimitives.h"

#define fail_return(p) if(!p) {return;}

namespace engine
{
	ExPainter::ExPainter(Application& app) : _application(app)
	{
	}

	ExPainter::~ExPainter()
	{
	}

	void ExPainter::setClearColor(const Color& color)
	{
		_application.renderer().setDrawColor(color);
	}

	void ExPainter::drawPixel(const Color& color, Vec2 pos)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		pixelRGBA(sdlRenderer, pos.x, pos.y, color.r, color.g, color.b, color.a);
	}

	void ExPainter::drawArc(const Color& color, const Vec2& pos, float radius, int start, int end)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		arcRGBA(sdlRenderer, pos.x, pos.y, radius, start, end, color.r, color.g, color.b, color.a);
	}

	void ExPainter::drawPie(const Color& color, const Vec2& center, float radius, int startAngle, int endAngle)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		pieRGBA(sdlRenderer, center.x, center.y, radius, startAngle, endAngle, color.r, color.g, color.b, color.a);
	}

	void ExPainter::fillPie(const Color& color, const Vec2& center, float radius, int startAngle, int endAngle)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		filledPieRGBA(sdlRenderer, center.x, center.y, radius, startAngle, endAngle, color.r, color.g, color.b, color.a);
	}

	// radius: horizonal & vertical radius
	void ExPainter::drawEllipse(const Color& color, const Vec2& center, const Vec2& radius)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		if (_antiAliased) 
		{
			aaellipseRGBA(sdlRenderer, center.x, center.y, radius.x, radius.y, color.r, color.g, color.b, color.a);
		}
		else
		{
			ellipseRGBA(sdlRenderer, center.x, center.y, radius.x, radius.y, color.r, color.g, color.b, color.a);
		}
	}

	void ExPainter::fillEllipse(const Color& color, const Vec2& center, const Vec2& radius)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();	
		filledEllipseRGBA(sdlRenderer, center.x, center.y, radius.x, radius.y, color.r, color.g, color.b, color.a);
	}

	void ExPainter::fillRect(const Color& color, Rect rect, float round)
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

	void ExPainter::drawRect(const Color& color, Rect rect, float round)
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

	void ExPainter::drawCircle(const Color& color, const Vec2& center, float radius)
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

	void ExPainter::fillCircle(const Color& color, const Vec2& center, float radius)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		filledCircleRGBA(sdlRenderer, center.x, center.y, radius, color.r, color.g, color.b, color.a);
	}

	void ExPainter::drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3)
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

	void ExPainter::fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3)
	{
		auto sdlRenderer = _application.renderer().getSdlRenderer();
		filledTrigonRGBA(sdlRenderer, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color.r, color.g, color.b, color.a);
	}

	void ExPainter::drawLine(const Color& color, Vec2 beginPos, Vec2 endPos, float lineWidth)
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

	void ExPainter::drawTexture(Texture* pTexture, const Rect& uv, const Rect& dst)
	{
		fail_return(pTexture);

		auto size = pTexture->size();
		Rect srcrect = { uv.x * size.x, uv.y * size.y, uv.w * size.x, uv.h * size.y };
		_application.renderer().drawTexture(pTexture, srcrect, dst);

	}

	void ExPainter::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color)
	{
		_application.renderer().drawText(text, font, pos, color);
	}

	void ExPainter::setClipRect(const Rect& rect)
	{
		if (rect == _clipRect)
		{
			return;
		}

		_clipRect = rect;
		_application.renderer().setClipRect(rect);
	}

	Rect ExPainter::getClipRect()
	{
		return _clipRect;
	}

	void ExPainter::setAntiAlaised(bool anti_aliased)
	{
		_antiAliased = anti_aliased;
	}

}