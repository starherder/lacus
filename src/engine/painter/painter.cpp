#include "painter.h"
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
		constexpr float kMaxMiterScale = 4.0f;

		Vertex makeVertex(const Vec2& pos, const Color& color, const Vec2& uv = { 0.0f, 0.0f })
		{
			Vertex vertex{};
			vertex.position = ToPoint(pos);
			vertex.color = color;
			vertex.tex_coord = ToPoint(uv);
			return vertex;
		}

		Vec2 rotatePoint(const Vec2& point, const Vec2& center, float radian)
		{
			Vec2 delta = point - center;
			const float cosA = std::cos(radian);
			const float sinA = std::sin(radian);
			return Vec2{ delta.x * cosA - delta.y * sinA, delta.x * sinA + delta.y * cosA } + center;
		}

		float lengthSquared(const Vec2& value)
		{
			return value.x * value.x + value.y * value.y;
		}

		Vec2 safeNormal(const Vec2& value)
		{
			const float lenSqr = lengthSquared(value);
			if (lenSqr <= kEpsilon)
			{
				return { 0.0f, 0.0f };
			}
			return value * (1.0f / std::sqrt(lenSqr));
		}

		Vec2 uvFromBounds(const Vec2& point, const Rect& uv, const Rect& bounds)
		{
			const float u = bounds.w != 0.0f ? (point.x - bounds.x) / bounds.w : 0.0f;
			const float v = bounds.h != 0.0f ? (point.y - bounds.y) / bounds.h : 0.0f;
			return { uv.x + uv.w * u, uv.y + uv.h * v };
		}
	}

	Painter::Painter(Application& app) : IPainter(app)
	{
	}

	Painter::~Painter()
	{
	}

	void Painter::init()
	{
	}

	void Painter::quit()
	{
		clearDrawList();
		_clipStack.clear();
	}

	void Painter::preFrame()
	{
		clearDrawList();
		_clipStack.clear();
		_application.renderer().setClipRect(defaultClipRect());
	}

	void Painter::postFrame()
	{
		flushDrawList();
		_clipStack.clear();
		_application.renderer().setClipRect(defaultClipRect());
	}

	void Painter::setClearColor(const Color& color)
	{
		_application.renderer().setDrawColor(color);
	}

	void Painter::pushClipRect(const Rect& rect)
	{
		Rect clip = rect;
		if (!_clipStack.empty())
		{
			clip = _clipStack.back().intersect(rect);
		}
		_clipStack.push_back(clip);
	}

	void Painter::popClipRect()
	{
		if (_clipStack.empty())
		{
			return;
		}

		_clipStack.pop_back();
	}

	void Painter::fillRect(const Color& color, const Rect& rect, float round)
	{
		Vec2 tl = rect.pos();
		Vec2 br = rect.pos() + rect.size();

		if (round <= 0.0f)
		{
			primRect(color, tl, br);
			return;
		}

		pathRect(tl, br, round);
		pathFillConvex(color);
	}

	void Painter::drawRect(const Color& color, const Rect& rect, float round, float thickness)
	{
		Vec2 tl = rect.pos();
		Vec2 br = rect.pos() + rect.size();

		pathRect(tl, br, round);
		pathStroke(color, true, thickness);
	}

	void Painter::drawCircle(const Color& color, const Vec2& center, float radius, int segments, float thickness)
	{
		if (radius <= 0.0f)
		{
			return;
		}

		pathArcTo(center, radius, 0.0f, PI_2, segments);
		pathStroke(color, true, thickness);
	}

	void Painter::fillCircle(const Color& color, const Vec2& center, float radius, int segments)
	{
		if (radius <= 0.0f)
		{
			return;
		}

		pathArcTo(center, radius, 0.0f, PI_2, segments);
		pathFillConvex(color);
	}

	void Painter::drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness)
	{
		pathLineTo(p1);
		pathLineTo(p2);
		pathLineTo(p3);
		pathStroke(color, true, thickness);
	}

	void Painter::fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3)
	{
		pathLineTo(p1);
		pathLineTo(p2);
		pathLineTo(p3);
		pathFillConvex(color);
	}

	void Painter::drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float thickness)
	{
		pathLineTo(beginPos);
		pathLineTo(endPos);
		pathStroke(color, false, thickness);
	}

	void Painter::drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness)
	{
		if (!points || point_count <= 0)
		{
			return;
		}

		for (int i = 0; i < point_count; ++i)
		{
			pathLineTo(points[i]);
		}

		pathStroke(color, closed, thickness);
	}

	void Painter::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color, float wrap_line)
	{
		fail_return(font);

		flushDrawList();
		_application.renderer().setClipRect(currentClipRect());
		_application.renderer().drawText(text, font, pos, color);
	}

	void Painter::drawTexTile(TexTile* pTexTile, const Rect& dst, float radian, float round, const Color& color)
	{
		fail_return(pTexTile);
		fail_return(pTexTile->texture());

		if (std::abs(radian) <= kEpsilon)
		{
			drawTexture(pTexTile->texture(), pTexTile->rect(), dst, round, color);
			return;
		}

		Texture* texture = pTexTile->texture();
		const Vec2 textureSize = texture->size();
		if (textureSize.x <= 0.0f || textureSize.y <= 0.0f)
		{
			return;
		}

		const Vec2 dstPos = dst.pos();
		const Vec2 dstSize = dst.size();
		const Vec2 center = dstPos + dstSize * pTexTile->center();

		Vec2 points[4] = {
			rotatePoint(dstPos, center, radian),
			rotatePoint({ dstPos.x + dstSize.x, dstPos.y }, center, radian),
			rotatePoint(dstPos + dstSize, center, radian),
			rotatePoint({ dstPos.x, dstPos.y + dstSize.y }, center, radian)
		};

		const Rect src = pTexTile->rect();
		const Vec2 uvMin = src.pos() / textureSize;
		const Vec2 uvMax = (src.pos() + src.size()) / textureSize;
		Vec2 uvs[4] = {
			{ uvMin.x, uvMin.y },
			{ uvMax.x, uvMin.y },
			{ uvMax.x, uvMax.y },
			{ uvMin.x, uvMax.y }
		};

		primQuadUV(texture, points, uvs, color);
	}

	void Painter::drawTexture(Texture* pTexture, const Rect& src, const Rect& dst, float round, const Color& color)
	{
		fail_return(pTexture);

		const Vec2 textureSize = pTexture->size();
		if (textureSize.x <= 0.0f || textureSize.y <= 0.0f)
		{
			return;
		}

		const Rect uv = { src.pos() / textureSize, src.size() / textureSize };
		drawTextureUV(pTexture, uv, dst, round, color);
	}

	void Painter::drawTextureUV(Texture* pTexture, const Rect& uv, const Rect& dst, float round, const Color& color)
	{
		fail_return(pTexture);

		if (round <= 0.0f)
		{
			primRectUV(pTexture, dst, uv, color);
			return;
		}

		pathRect(dst.pos(), dst.pos() + dst.size(), round);
		pathFillConvex(pTexture, color, uv, dst);
	}

	void Painter::drawGeometry(Texture* texture, const Vertex* vertices, int num_vertices,
		const int* indices, int num_indices, const Vec2& pos, float scale)
	{
		if (!vertices || num_vertices <= 0)
		{
			return;
		}

		const bool hasIndices = indices && num_indices > 0;
		const int writeIdxCount = hasIndices ? num_indices : num_vertices;
		PrimReservation reservation = primReserve(texture, writeIdxCount, num_vertices);

		for (int i = 0; i < num_vertices; ++i)
		{
			Vertex vertex = vertices[i];
			vertex.position.x = pos.x + vertices[i].position.x * scale;
			vertex.position.y = pos.y + vertices[i].position.y * scale;
			_vtxBuffer[reservation.vtxOffset + i] = vertex;
		}

		for (int i = 0; i < writeIdxCount; ++i)
		{
			_idxBuffer[reservation.idxOffset + i] = reservation.localVtxOffset + (hasIndices ? indices[i] : i);
		}
	}

	void Painter::setAntiAlaised(bool anti_aliased)
	{
		_antiAliased = anti_aliased;
	}

	void Painter::clearDrawList()
	{
		_cmdBuffer.clear();
		_vtxBuffer.clear();
		_idxBuffer.clear();
		_path.clear();
	}

	void Painter::flushDrawList()
	{
		if (_cmdBuffer.empty())
		{
			return;
		}

		auto& renderer = _application.renderer();
		for (const DrawCmd& cmd : _cmdBuffer)
		{
			if (cmd.idxCount <= 0 || cmd.vtxCount <= 0)
			{
				continue;
			}

			renderer.setClipRect(cmd.clipRect);
			renderer.drawGeometry(cmd.texture,
				_vtxBuffer.data() + cmd.vtxOffset, cmd.vtxCount,
				_idxBuffer.data() + cmd.idxOffset, cmd.idxCount);
		}

		clearDrawList();
		renderer.setClipRect(currentClipRect());
	}

	Rect Painter::defaultClipRect() const
	{
		const Vec2i size = _application.window().getSize();
		return { 0.0f, 0.0f, (float)size.x, (float)size.y };
	}

	Rect Painter::currentClipRect() const
	{
		return _clipStack.empty() ? defaultClipRect() : _clipStack.back();
	}

	Painter::DrawCmd& Painter::requireDrawCmd(Texture* texture)
	{
		const Rect clipRect = currentClipRect();
		if (!_cmdBuffer.empty())
		{
			DrawCmd& cmd = _cmdBuffer.back();
			if (cmd.texture == texture && cmd.clipRect == clipRect)
			{
				return cmd;
			}
		}

		DrawCmd& cmd = _cmdBuffer.emplace_back();
		cmd.texture = texture;
		cmd.clipRect = clipRect;
		cmd.idxOffset = (int)_idxBuffer.size();
		cmd.vtxOffset = (int)_vtxBuffer.size();
		return cmd;
	}

	Painter::PrimReservation Painter::primReserve(Texture* texture, int idxCount, int vtxCount)
	{
		assert(idxCount >= 0);
		assert(vtxCount >= 0);

		DrawCmd& cmd = requireDrawCmd(texture);
		PrimReservation reservation;
		reservation.idxOffset = (int)_idxBuffer.size();
		reservation.vtxOffset = (int)_vtxBuffer.size();
		reservation.localVtxOffset = reservation.vtxOffset - cmd.vtxOffset;

		cmd.idxCount += idxCount;
		cmd.vtxCount += vtxCount;

		_idxBuffer.resize(_idxBuffer.size() + idxCount);
		_vtxBuffer.resize(_vtxBuffer.size() + vtxCount);
		return reservation;
	}

	void Painter::primRect(const Color& color, const Vec2& tl, const Vec2& br)
	{
		const Rect dst = { tl, br - tl };
		const Rect uv = { 0.0f, 0.0f, 0.0f, 0.0f };
		primRectUV(nullptr, dst, uv, color);
	}

	void Painter::primRectUV(Texture* texture, const Rect& dst, const Rect& uv, const Color& color)
	{
		const Vec2 dstMin = dst.pos();
		const Vec2 dstMax = dst.pos() + dst.size();
		const Vec2 uvMin = uv.pos();
		const Vec2 uvMax = uv.pos() + uv.size();

		Vec2 points[4] = {
			{ dstMin.x, dstMin.y },
			{ dstMax.x, dstMin.y },
			{ dstMax.x, dstMax.y },
			{ dstMin.x, dstMax.y }
		};
		Vec2 uvs[4] = {
			{ uvMin.x, uvMin.y },
			{ uvMax.x, uvMin.y },
			{ uvMax.x, uvMax.y },
			{ uvMin.x, uvMax.y }
		};

		primQuadUV(texture, points, uvs, color);
	}

	void Painter::primQuadUV(Texture* texture, const Vec2* points, const Vec2* uvs, const Color& color)
	{
		PrimReservation reservation = primReserve(texture, 6, 4);

		for (int i = 0; i < 4; ++i)
		{
			_vtxBuffer[reservation.vtxOffset + i] = makeVertex(points[i], color, uvs[i]);
		}

		int* idx = _idxBuffer.data() + reservation.idxOffset;
		const int vtx = reservation.localVtxOffset;
		idx[0] = vtx + 0;
		idx[1] = vtx + 2;
		idx[2] = vtx + 1;
		idx[3] = vtx + 0;
		idx[4] = vtx + 3;
		idx[5] = vtx + 2;
	}

	void Painter::pathLineTo(const Vec2& pos)
	{
		_path.push_back(pos);
	}

	void Painter::pathArcTo(const Vec2& centre, float radius, float min, float max, int segments)
	{
		if (radius <= 0.0f)
		{
			return;
		}

		if (segments <= 0)
		{
			segments = std::clamp((int)(radius * 0.5f), 12, 64);
		}
		else
		{
			segments = std::max(segments, 3);
		}

		const float step = (max - min) / (float)segments;
		for (int i = 0; i < segments; ++i)
		{
			const float angle = min + step * (float)i;
			_path.push_back({ centre.x + std::cos(angle) * radius, centre.y + std::sin(angle) * radius });
		}
	}

	void Painter::pathRect(const Vec2& tl, const Vec2& br, float rounding)
	{
		const float minX = std::min(tl.x, br.x);
		const float minY = std::min(tl.y, br.y);
		const float maxX = std::max(tl.x, br.x);
		const float maxY = std::max(tl.y, br.y);
		const float width = maxX - minX;
		const float height = maxY - minY;
		const float round = std::min({ rounding, width * 0.5f, height * 0.5f });

		if (round <= 0.0f)
		{
			pathLineTo({ minX, minY });
			pathLineTo({ maxX, minY });
			pathLineTo({ maxX, maxY });
			pathLineTo({ minX, maxY });
			return;
		}

		const int segments = std::clamp((int)(round * 0.35f), 3, 12);
		pathArcTo({ maxX - round, minY + round }, round, -PI_HALF, 0.0f, segments);
		pathArcTo({ maxX - round, maxY - round }, round, 0.0f, PI_HALF, segments);
		pathArcTo({ minX + round, maxY - round }, round, PI_HALF, PI, segments);
		pathArcTo({ minX + round, minY + round }, round, PI, PI + PI_HALF, segments);
	}

	void Painter::pathStroke(const Color& color, bool closed, float thickness)
	{
		const int pointsCount = (int)_path.size();
		if (pointsCount < 2 || thickness <= 0.0f)
		{
			_path.clear();
			return;
		}

		if (!closed && pointsCount == 2 && lengthSquared(_path[1] - _path[0]) <= kEpsilon)
		{
			_path.clear();
			return;
		}

		const int segmentCount = closed ? pointsCount : pointsCount - 1;
		const float halfThickness = std::max(thickness * 0.5f, 0.5f);

		std::vector<Vec2> normals(segmentCount);
		for (int i = 0; i < segmentCount; ++i)
		{
			const int next = (i + 1) == pointsCount ? 0 : i + 1;
			const Vec2 dir = safeNormal(_path[next] - _path[i]);
			normals[i] = { -dir.y, dir.x };
		}

		PrimReservation reservation = primReserve(nullptr, segmentCount * 6, pointsCount * 2);
		for (int i = 0; i < pointsCount; ++i)
		{
			Vec2 normal;
			if (!closed && i == 0)
			{
				normal = normals[0];
			}
			else if (!closed && i == pointsCount - 1)
			{
				normal = normals[segmentCount - 1];
			}
			else
			{
				const int prevNormal = (i == 0) ? segmentCount - 1 : i - 1;
				const int nextNormal = (i == segmentCount) ? 0 : i;
				normal = safeNormal(normals[prevNormal] + normals[nextNormal]);

				if (lengthSquared(normal) <= kEpsilon)
				{
					normal = normals[nextNormal];
				}

				const float dot = std::abs(normal.x * normals[nextNormal].x + normal.y * normals[nextNormal].y);
				const float miterScale = std::min(1.0f / std::max(dot, 0.25f), kMaxMiterScale);
				normal *= miterScale;
			}

			const Vec2 offset = normal * halfThickness;
			_vtxBuffer[reservation.vtxOffset + i * 2 + 0] = makeVertex(_path[i] + offset, color);
			_vtxBuffer[reservation.vtxOffset + i * 2 + 1] = makeVertex(_path[i] - offset, color);
		}

		for (int i = 0; i < segmentCount; ++i)
		{
			const int next = (i + 1) == pointsCount ? 0 : i + 1;
			const int vtx = reservation.localVtxOffset;
			int* idx = _idxBuffer.data() + reservation.idxOffset + i * 6;
			idx[0] = vtx + i * 2 + 0;
			idx[1] = vtx + next * 2 + 1;
			idx[2] = vtx + next * 2 + 0;
			idx[3] = vtx + i * 2 + 0;
			idx[4] = vtx + i * 2 + 1;
			idx[5] = vtx + next * 2 + 1;
		}

		_path.clear();
	}

	void Painter::pathFillConvex(const Color& color)
	{
		const int pointsCount = (int)_path.size();
		if (pointsCount < 3)
		{
			_path.clear();
			return;
		}

		PrimReservation reservation = primReserve(nullptr, (pointsCount - 2) * 3, pointsCount);
		for (int i = 0; i < pointsCount; ++i)
		{
			_vtxBuffer[reservation.vtxOffset + i] = makeVertex(_path[i], color);
		}

		for (int i = 2; i < pointsCount; ++i)
		{
			int* idx = _idxBuffer.data() + reservation.idxOffset + (i - 2) * 3;
			idx[0] = reservation.localVtxOffset;
			idx[1] = reservation.localVtxOffset + i - 1;
			idx[2] = reservation.localVtxOffset + i;
		}

		_path.clear();
	}

	void Painter::pathFillConvex(Texture* texture, const Color& color, const Rect& uv, const Rect& bounds)
	{
		const int pointsCount = (int)_path.size();
		if (pointsCount < 3)
		{
			_path.clear();
			return;
		}

		PrimReservation reservation = primReserve(texture, (pointsCount - 2) * 3, pointsCount);
		for (int i = 0; i < pointsCount; ++i)
		{
			_vtxBuffer[reservation.vtxOffset + i] = makeVertex(_path[i], color, uvFromBounds(_path[i], uv, bounds));
		}

		for (int i = 2; i < pointsCount; ++i)
		{
			int* idx = _idxBuffer.data() + reservation.idxOffset + (i - 2) * 3;
			idx[0] = reservation.localVtxOffset;
			idx[1] = reservation.localVtxOffset + i - 1;
			idx[2] = reservation.localVtxOffset + i;
		}

		_path.clear();
	}
}
