#include "painter.h"
#include "engine/application.h"
#include "engine/texture.h"
#include "engine/font.h"

#define fail_return(p) if(!p) {return;}

namespace engine
{
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
	}

	void Painter::preFrame()
	{
	}

	void Painter::postFrame()
	{
	}

	void Painter::setClearColor(const Color& color)
	{
		_application.renderer().setDrawColor(color);
	}

	void Painter::pushClipRect(const Rect& rect)
	{
		_clipStack.push(rect);

		_application.renderer().setClipRect(rect);
	}

	void Painter::popClipRect()
	{
		auto rect = _clipStack.top();
		_application.renderer().setClipRect(rect);

		_clipStack.pop();
	}

	void Painter::fillRect(const Color& color, const Rect& rect, float round)
	{
		Vec2 tl = { rect.x, rect.y };
		Vec2 br = { rect.x + rect.w, rect.y + rect.h };

		if (round <= 0)
		{
			primRect(color, tl, br);

			drawPath();
		}
		else
		{
			pathRect(tl, br, round);

			strokeConvexPolyFilled(color);
			drawPath();
		}
	}

	void Painter::drawRect(const Color& color, const Rect& rect, float round, float thickness)
	{
		Vec2 beginPos = { rect.x, rect.y };
		Vec2 endPos = { rect.x + rect.w, rect.y + rect.h };

		pathRect(beginPos, endPos, round);

		strokePolyline(color, true, thickness);
		drawPath();
	}

	void Painter::drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness)
	{
		pathLineTo(p1);
		pathLineTo(p2);
		pathLineTo(p3);

		strokePolyline(color, true, thickness);
		drawPath();
	}

	void Painter::fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3)
	{
		pathLineTo(p1);
		pathLineTo(p2);
		pathLineTo(p3);

		strokeConvexPolyFilled(color);
		drawPath();
	}

	void Painter::drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float thickness)
	{
		pathLineTo(beginPos);
		pathLineTo(endPos);

		strokePolyline(color, false, thickness);
		drawPath();
	}

	void Painter::drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness)
	{
		for (int i = 0; i < point_count; i++) {
			pathLineTo(points[i]);
		}

		strokePolyline(color, closed, thickness);
		drawPath();
	}

	void Painter::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color, float wrap_line)
	{
		_application.renderer().drawText(text, font, pos, color);
	}

	void Painter::drawTexTile(TexTile* pTexTile, const Rect& dst, float radian, float round, const Color& color)
	{
		drawTexture(pTexTile->texture(), pTexTile->rect(), dst, round, color);
	}

	void Painter::drawTexture(Texture* pTexture, const Rect& src, const Rect& dst, float round, const Color& c)
	{
		fail_return(pTexture);

		_application.renderer().drawTexture(pTexture, src, dst);
		_vPaths.resize(0);
	}

	void Painter::drawTextureUV(Texture* pTexture, const Rect& uv, const Rect& dst, float round, const Color& c)
	{
		fail_return(pTexture);

		auto size = pTexture->size();
		Rect srcrect = { uv.x * size.x, uv.y * size.y, uv.w * size.x, uv.h * size.y };

		_application.renderer().drawTexture(pTexture, srcrect, dst);
		_vPaths.resize(0);
	}

	void Painter::drawPath()
	{
		_application.renderer().drawGeometry(nullptr,
			_geometryVertices.data(), (int)_geometryVertices.size(),
			_geometryIndices.data(), (int)_geometryIndices.size());

		_vPaths.clear();
		_geometryVertices.clear();
		_geometryIndices.clear();
	}

	void Painter::drawCircle(const Color& color, const Vec2& center, float radius, int segments, float thickness)
	{
		pathArcTo(center, radius - 0.5f, 0, PI_2, segments);

		strokePolyline(color, false, thickness);

		drawPath();
	}

	void Painter::fillCircle(const Color& color, const Vec2& center, float radius, int segments)
	{
		auto oldAntiAlaise = isAntiAlaised();
		setAntiAlaised(false);

		pathArcTo(center, radius, 0, PI_2, segments);

		strokeConvexPolyFilled(color);

		drawPath();

		setAntiAlaised(oldAntiAlaise);
	}


	void Painter::setAntiAlaised(bool anti_aliased)
	{
		_antiAliased = anti_aliased;
	}

	void Painter::pathLineTo(const Vec2& pos)
	{
		Vec2 p;
		p.x = pos.x;
		p.y = pos.y;

		_vPaths.push_back(p);
	}

	void Painter::pathArcTo(const Vec2& centre, float radius, float min, float max, int segments)
	{
		for (int i = 0; i <= segments; i++) {
			const float a = min + ((float)i / (float)segments) * (max - min);
			_vPaths.push_back(Vec2(centre.x - sinf(a) * radius, centre.y + cosf(a) * radius));
		}
	}

	void Painter::pathArcToFast(const Vec2& centre, float radius, int a_min_of_12, int a_max_of_12)
	{
		static Vec2 circle[13];
		static bool has_build = false;

		/* 构建圆 */
		if (has_build == false) {
			for (int i = 0; i <= 12; i++) {
				float radian = float(i) / 12 * 6.28318f;
				circle[i].x = -sinf(radian);
				circle[i].y = -cosf(radian);
			}
			has_build = true;
		}

		for (int i = a_min_of_12; i <= a_max_of_12; i++) {
			_vPaths.push_back(Vec2(circle[i].x * radius + centre.x, circle[i].y * radius + centre.y));
		}
	}

	void Painter::pathRect(const Vec2& tl, const Vec2& br, float rounding)
	{
		if (rounding <= 0.0f) {
			pathLineTo(tl);
			pathLineTo({ br.x, tl.y });
			pathLineTo(br);
			pathLineTo({ tl.x, br.y });
		}
		else {
			pathArcToFast({ tl.x + rounding, tl.y + rounding }, rounding, 0, 3);
			pathArcToFast({ tl.x + rounding, br.y + rounding }, rounding, 3, 6);
			pathArcToFast({ br.x - rounding, br.y + rounding }, rounding, 6, 9);
			pathArcToFast({ br.x - rounding, tl.y + rounding }, rounding, 9, 12);
		}
	}

	void Painter::strokePolyline(const Color& color, bool closed, float thickness)
	{
		if (_vPaths.size() < 2) return;

		int points_count = (int)_vPaths.size();
		int count = (closed == false) ? points_count - 1 : points_count;

		const bool thick_line = thickness > 1.0f;

		/* 反走样 */
		const float AA_SIZE = 1.0f;
		//const float alpha = 0.0f;

		const int idx_count = thick_line ? count * 18 : count * 12;
		const int vtx_count = thick_line ? points_count * 4 : points_count * 3;

		_geometryVertices.resize(vtx_count);
		_geometryIndices.resize(idx_count);

		/* 临时缓冲区 */
		Vec2* temp_normals = (Vec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(Vec2));
		Vec2* temp_points = temp_normals + points_count;

		/* 计算直线的法线 */
		for (int i1 = 0; i1 < count; i1++)
		{
			const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
			Vec2 diff = _vPaths[i2] - _vPaths[i1];
			diff = SafeNormal(diff);
			
			temp_normals[i1].x = diff.y;
			temp_normals[i1].y = -diff.x;
		}

		if (closed == false)
		{
			temp_normals[points_count - 1] = temp_normals[points_count - 2];
		}

		if (thick_line == false)
		{
			if (closed == false) {
				temp_points[0] = _vPaths[0] + temp_normals[0] * AA_SIZE;
				temp_points[1] = _vPaths[0] - temp_normals[0] * AA_SIZE;
				temp_points[(points_count - 1) * 2 + 0] = _vPaths[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
				temp_points[(points_count - 1) * 2 + 1] = _vPaths[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
			}
			unsigned int idx1 = 0;

			for (int i1 = 0; i1 < count; i1++) {
				const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				unsigned int idx2 = (i1 + 1) == points_count ? 0 : idx1 + 3;

				/* 平均法向量，即两直线交点的法向量 */
				Vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
				float dmr2 = dm.x * dm.x + dm.y * dm.y;
				if (dmr2 > 0.000001f) {
					float scale = 1.0f / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm = dm * scale;
				}
				dm = dm * AA_SIZE;

				temp_points[i2 * 2 + 0] = _vPaths[i2] + dm;
				temp_points[i2 * 2 + 1] = _vPaths[i2] - dm;

				/* 索引数据 */
				_geometryIndices[i1 * 12 + 0] = idx2 + 0;
				_geometryIndices[i1 * 12 + 1] = idx1 + 0;
				_geometryIndices[i1 * 12 + 2] = idx1 + 2;

				_geometryIndices[i1 * 12 + 3] = idx1 + 2;
				_geometryIndices[i1 * 12 + 4] = idx2 + 2;
				_geometryIndices[i1 * 12 + 5] = idx2 + 0;

				_geometryIndices[i1 * 12 + 6] = idx2 + 1;
				_geometryIndices[i1 * 12 + 7] = idx1 + 1;
				_geometryIndices[i1 * 12 + 8] = idx1 + 0;

				_geometryIndices[i1 * 12 + 9] = idx1 + 0;
				_geometryIndices[i1 * 12 + 10] = idx2 + 0;
				_geometryIndices[i1 * 12 + 11] = idx2 + 1;

				idx1 = idx2;
			}

			/* 顶点数据 */
			for (int i = 0; i < points_count; i++) {
				_geometryVertices[i * 3 + 0].position = ToPoint(_vPaths[i]);
				_geometryVertices[i * 3 + 1].position = ToPoint(temp_points[i * 2 + 0]);
				_geometryVertices[i * 3 + 2].position = ToPoint(temp_points[i * 2 + 1]);

				_geometryVertices[i * 3 + 0].color = color;
				_geometryVertices[i * 3 + 1].color = color;
				_geometryVertices[i * 3 + 2].color = color;

				//_geometryVertices[i * 3 + 1].color.w = alpha;
				//_geometryVertices[i * 3 + 2].color.w = alpha;
			}
		}
		else
		{
			const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
			if (closed == false)
			{
				temp_points[0] = _vPaths[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
				temp_points[1] = _vPaths[0] + temp_normals[0] * (half_inner_thickness);
				temp_points[2] = _vPaths[0] - temp_normals[0] * (half_inner_thickness);
				temp_points[3] = _vPaths[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
				temp_points[(points_count - 1) * 4 + 0] = _vPaths[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
				temp_points[(points_count - 1) * 4 + 1] = _vPaths[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness);
				temp_points[(points_count - 1) * 4 + 2] = _vPaths[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness);
				temp_points[(points_count - 1) * 4 + 3] = _vPaths[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
			}

			unsigned int idx1 = 0;

			for (int i1 = 0; i1 < count; i1++)
			{
				const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				unsigned int idx2 = (i1 + 1) == points_count ? 0 : idx1 + 4;

				/* 平均法向量，即两直线交点的法向量 */
				Vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
				float dmr2 = glm::distance({ 0, 0 }, dm);
				if (dmr2 > 0.000001f) {
					float scale = 1.0f / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm = dm * scale;
				}

				Vec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
				Vec2 dm_in = dm * half_inner_thickness;

				/* 索引数据 */
				temp_points[i2 * 4 + 0] = _vPaths[i2] + dm_out;
				temp_points[i2 * 4 + 1] = _vPaths[i2] + dm_in;
				temp_points[i2 * 4 + 2] = _vPaths[i2] - dm_in;
				temp_points[i2 * 4 + 3] = _vPaths[i2] - dm_out;

				_geometryIndices[i1 * 18 + 0] = idx2 + 1;
				_geometryIndices[i1 * 18 + 1] = idx1 + 1;
				_geometryIndices[i1 * 18 + 2] = idx1 + 2;

				_geometryIndices[i1 * 18 + 3] = idx1 + 2;
				_geometryIndices[i1 * 18 + 4] = idx2 + 2;
				_geometryIndices[i1 * 18 + 5] = idx2 + 1;

				_geometryIndices[i1 * 18 + 6] = idx2 + 1;
				_geometryIndices[i1 * 18 + 7] = idx1 + 1;
				_geometryIndices[i1 * 18 + 8] = idx1 + 0;

				_geometryIndices[i1 * 18 + 9] = idx1 + 0;
				_geometryIndices[i1 * 18 + 10] = idx2 + 0;
				_geometryIndices[i1 * 18 + 11] = idx2 + 1;

				_geometryIndices[i1 * 18 + 12] = idx2 + 2;
				_geometryIndices[i1 * 18 + 13] = idx1 + 2;
				_geometryIndices[i1 * 18 + 14] = idx1 + 3;

				_geometryIndices[i1 * 18 + 15] = idx1 + 3;
				_geometryIndices[i1 * 18 + 16] = idx2 + 3;
				_geometryIndices[i1 * 18 + 17] = idx2 + 2;

				idx1 = idx2;
			}

			/* 顶点数据 */
			for (int i = 0; i < points_count; i++)
			{
				_geometryVertices[i * 4 + 0].position = ToPoint(temp_points[i * 4 + 0]); _geometryVertices[i * 4 + 0].color = color;
				_geometryVertices[i * 4 + 1].position = ToPoint(temp_points[i * 4 + 1]); _geometryVertices[i * 4 + 1].color = color;
				_geometryVertices[i * 4 + 2].position = ToPoint(temp_points[i * 4 + 2]); _geometryVertices[i * 4 + 2].color = color;
				_geometryVertices[i * 4 + 3].position = ToPoint(temp_points[i * 4 + 3]); _geometryVertices[i * 4 + 3].color = color;

				//_geometryVertices[i * 4 + 0].color.w = alpha;
				//_geometryVertices[i * 4 + 3].color.w = alpha;
			}
		}
	}

	void Painter::strokeConvexPolyFilled(const Color& color)
	{
		int points_count = (int)_vPaths.size();
		int idx_count;
		int vtx_count;

		if (_antiAliased)
		{
			const float AA_SIZE = 1.0f;
			//const float alpha = 0.0f;

			idx_count = (points_count - 2) * 3 + points_count * 6;
			vtx_count = (points_count * 2);

			_geometryVertices.resize(vtx_count);
			_geometryIndices.resize(idx_count);

			unsigned int vtx_inner_idx = 0;
			unsigned int vtx_outer_idx = 1;

			for (int i = 2; i < points_count; i++)
			{
				_geometryIndices[(i - 2) * 3 + 0] = vtx_inner_idx;
				_geometryIndices[(i - 2) * 3 + 1] = vtx_inner_idx + ((i - 1) << 1);
				_geometryIndices[(i - 2) * 3 + 2] = vtx_inner_idx + (i << 1);
			}

			/* 计算法线 */
			Vec2* temp_normals = (Vec2*)alloca(points_count * sizeof(Vec2));
			for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
				const Vec2& p0 = _vPaths[i0];
				const Vec2& p1 = _vPaths[i1];
				Vec2 diff = p1 - p0;
				diff = SafeNormal(diff);

				temp_normals[i0].x = diff.y;
				temp_normals[i0].y = -diff.x;
			}

			for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
				/* 平均法向量，即两直线交点的法向量 */
				Vec2 dm = (temp_normals[i0] + temp_normals[i1]) * 0.5f;
				float dmr2 = glm::distance({ 0,0 }, dm);

				if (dmr2 > 0.000001f) {
					float scale = 1.0f / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm = dm * scale;
				}
				dm = dm * AA_SIZE * 0.5f;

				/* 顶点数据 */
				_geometryVertices[i1 * 2 + 0].position = ToPoint(_vPaths[i1] - dm);
				_geometryVertices[i1 * 2 + 1].position = ToPoint(_vPaths[i1] + dm);

				_geometryVertices[i1 * 2 + 0].color = color;
				_geometryVertices[i1 * 2 + 1].color = color;
				//_geometryVertices[i1 * 2 + 1].color.w = alpha;

				/* 索引数据 */
				_geometryIndices[i1 * 6 + 0 + (points_count - 2) * 3] = vtx_inner_idx + (i1 << 1);
				_geometryIndices[i1 * 6 + 1 + (points_count - 2) * 3] = vtx_inner_idx + (i0 << 1);
				_geometryIndices[i1 * 6 + 2 + (points_count - 2) * 3] = vtx_outer_idx + (i0 << 1);
				_geometryIndices[i1 * 6 + 3 + (points_count - 2) * 3] = vtx_outer_idx + (i0 << 1);
				_geometryIndices[i1 * 6 + 4 + (points_count - 2) * 3] = vtx_outer_idx + (i1 << 1);
				_geometryIndices[i1 * 6 + 5 + (points_count - 2) * 3] = vtx_inner_idx + (i1 << 1);
			}
		}
		else
		{
			idx_count = (points_count - 2) * 3;
			vtx_count = points_count;

			_geometryVertices.resize(vtx_count);
			_geometryIndices.resize(idx_count);

			for (int i = 0; i < vtx_count; i++) {
				_geometryVertices[i].position = ToPoint(_vPaths[i]);
				_geometryVertices[i].color = color;
			}

			for (int i = 2; i < points_count; i++) {
				_geometryIndices[(i - 2) * 3 + 0] = 0;
				_geometryIndices[(i - 2) * 3 + 1] = i - 1;
				_geometryIndices[(i - 2) * 3 + 2] = i;
			}
		}
	}

	void Painter::primRect(const Color& color, const Vec2& tl, const Vec2& br)
	{
		_geometryVertices.resize(4);
		_geometryIndices.resize(6);

		_geometryVertices[0].position = { tl.x, tl.y };
		_geometryVertices[1].position = { br.x, tl.y };
		_geometryVertices[2].position = { br.x, br.y };
		_geometryVertices[3].position = { tl.x, br.y };

		_geometryVertices[0].color = color;
		_geometryVertices[1].color = color;
		_geometryVertices[2].color = color;
		_geometryVertices[3].color = color;

		_geometryIndices[0] = 0;
		_geometryIndices[1] = 2;
		_geometryIndices[2] = 1;
		_geometryIndices[3] = 0;
		_geometryIndices[4] = 3;
		_geometryIndices[5] = 2;
	}
}