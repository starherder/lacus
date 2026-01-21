#include "im_painter.h"
#include "engine/application.h"
#include "engine/texture.h"
#include "engine/font.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"


namespace engine
{
	static ImColor toImColor(const Color& color)
	{
		return ImColor{ color.r, color.g, color.b, color.a };
	}

	ImPainter::ImPainter(Application& app) : IPainter(app)
	{
	}

	ImPainter::~ImPainter()
	{
	}

	void ImPainter::init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		auto window = _application.window().getSdlWindow();
		auto renderer = _application.renderer().getSdlRenderer();

		// 初始化 ImGui 的 SDL3 和 SDL_Renderer3 后端
		ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
		ImGui_ImplSDLRenderer3_Init(renderer);

		initDefaultFont("fonts/msyh.ttf", 20);
	}

	void ImPainter::quit()
	{
		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();

		ImGui::DestroyContext();
	}

	void ImPainter::preFrame()
	{
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}

	void ImPainter::postFrame()
	{
		ImGui::Render();
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _application.renderer().getSdlRenderer());
	}

	void ImPainter::setClearColor(const Color& color)
	{
		_application.renderer().setDrawColor(color);
	}

	void ImPainter::pushClipRect(const Rect& rect)
	{
		ImVec2 pmin{ rect.x, rect.y }, pmax{rect.x+rect.w, rect.y+rect.h};
		ImGui::GetBackgroundDrawList()->PushClipRect(pmin, pmax);
	}
	
	void ImPainter::popClipRect()
	{
		ImGui::GetBackgroundDrawList()->PopClipRect();
	}

	void ImPainter::fillRect(const Color& color, const Rect& rect, float round)
	{
		ImVec2 pmin = {rect.x, rect.y};
		ImVec2 pmax = {rect.x+rect.w, rect.y+rect.h};
		ImGui::GetBackgroundDrawList()->AddRectFilled(pmin, pmax, toImColor(color), round, ImDrawFlags_RoundCornersAll);
	}

	void ImPainter::drawRect(const Color& color, const Rect& rect, float round, float thickness)
	{
		ImVec2 pmin = { rect.x, rect.y };
		ImVec2 pmax = { rect.x + rect.w, rect.y + rect.h };
		ImGui::GetBackgroundDrawList()->AddRect(pmin, pmax, toImColor(color), round, ImDrawFlags_RoundCornersAll, thickness);
	}

	void ImPainter::drawTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3, float thickness)
	{
		ImVec2 v1{p1.x, p1.y}, v2{p2.x, p2.y}, v3{p3.x, p3.y};
		ImGui::GetBackgroundDrawList()->AddTriangle(v1, v2, v3, toImColor(color), thickness);
	}

	void ImPainter::fillTriangle(const Color& color, const Vec2& p1, const Vec2& p2, const Vec2& p3)
	{
		ImVec2 v1{ p1.x, p1.y }, v2{ p2.x, p2.y }, v3{ p3.x, p3.y };
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(v1, v2, v3, toImColor(color));
	}

	void ImPainter::drawLine(const Color& color, const Vec2& beginPos, const Vec2& endPos, float thickness)
	{
		ImVec2 p1{beginPos.x, beginPos.y}, p2{endPos.x, endPos.y};
		ImGui::GetBackgroundDrawList()->AddLine(p1, p2, toImColor(color), thickness);
	}

	void ImPainter::drawLines(const Color& color, const Vec2* points, int point_count, bool closed, float thickness)
	{
		auto flag = closed ? ImDrawFlags_Closed : ImDrawFlags_None;
		ImGui::GetBackgroundDrawList()->AddPolyline((ImVec2*)points, point_count, toImColor(color), flag, thickness);
	}

	void ImPainter::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color, float wrap_width)
	{
		assert(font);
		ImGui::GetBackgroundDrawList()->AddText(font->imFont, (float)font->size, {pos.x, pos.y}, toImColor(color), text.c_str(), (const char*)0, wrap_width);
	}

	void ImPainter::drawTexture(Texture* pTexture, const Rect& src, const Rect& dst, float round, const Color& color)
	{
		assert(pTexture);

		Vec2 srcSize = pTexture->size();
		Rect uv = { src.pos()/srcSize, src.size()/srcSize };

		Vec2 lt = dst.pos();
		Vec2 rb = dst.pos() + dst.size();

		ImVec2 dst_min = { lt.x, lt.y };
		ImVec2 dst_max = { rb.x, rb.y };

		lt = uv.pos();
		rb = uv.pos() + uv.size();

		ImVec2 uv_min = { lt.x, lt.y };
		ImVec2 uv_max = { rb.x, rb.y };

		ImGui::GetBackgroundDrawList()->AddImageRounded(
			(void*)pTexture->texture(),
			dst_min, dst_max, uv_min, uv_max,
			toImColor(color), round, ImDrawFlags_RoundCornersAll);
	}

	void ImPainter::drawTextureUV(Texture* pTexture, const Rect& uv, const Rect& dst, float round, const Color& color)
	{
		assert(pTexture);

		Vec2 lt = dst.pos();
		Vec2 rb = dst.pos() + dst.size();

		ImVec2 dst_min = {lt.x, lt.y};
		ImVec2 dst_max = {rb.x, rb.y};

		lt = uv.pos();
		rb = uv.pos() + uv.size();

		ImVec2 uv_min = { lt.x, lt.y };
		ImVec2 uv_max = { rb.x, rb.y };

		ImGui::GetBackgroundDrawList()->AddImageRounded(
			(void*)pTexture->texture(), 
			dst_min, dst_max, uv_min, uv_max, 
			toImColor(color), round, ImDrawFlags_RoundCornersAll);
	}

	void ImPainter::drawCircle(const Color& color, const Vec2& center, float radius, int segments, float thickness)
	{
		ImGui::GetBackgroundDrawList()->AddCircle({center.x, center.y}, radius, toImColor(color), segments, thickness);
	}

	void ImPainter::fillCircle(const Color& color, const Vec2& center, float radius, int segments)
	{
		ImGui::GetBackgroundDrawList()->AddCircleFilled({center.x, center.y}, radius, toImColor(color), segments);
	}

	void ImPainter::initDefaultFont(const std::string& fontName, int fontSize)
	{
		auto fontPath = _application.resPath() / fontName;
		if (!std::filesystem::exists(fontPath))
		{
			SPDLOG_ERROR("font file({}) NOT found.", fontPath.string());
			return;
		}

		auto& io = ImGui::GetIO();
		auto fonts = ImGui::GetIO().Fonts;
		auto font = fonts->AddFontFromFileTTF(fontPath.string().c_str(), (float)fontSize, nullptr, fonts->GetGlyphRangesChineseFull());
		if (!font)
		{
			SPDLOG_ERROR("load font {} failed. use default.", fontPath.string());
			io.Fonts->AddFontDefault();
		}
	}

#if 1
	void ImPainter::drawGeometry(Texture* texture, const Vertex* vertices, int vertices_count, const int* indices, int indices_count, const Vec2& pos, float scale)
	{
		bool has_indices = (indices != nullptr && indices_count > 0);
		indices_count = has_indices ? indices_count : vertices_count;

		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		if (texture) 
		{
			drawList->PushTextureID((void*)texture->texture());
		}

		drawList->PrimReserve(indices_count, vertices_count);

		for (int i = 0; i < vertices_count; i++)
		{
			auto pt = ImVec2{ pos.x + vertices[i].position.x * scale, pos.y + vertices[i].position.y * scale };
			auto uv = ImVec2{ vertices[i].tex_coord.x, vertices[i].tex_coord.y };
			auto cl = ImColor{ vertices[i].color.r, vertices[i].color.g, vertices[i].color.b, vertices[i].color.a };

			if (has_indices)
			{
				drawList->PrimWriteVtx(pt, uv, cl);
			}
			else
			{
				drawList->PrimVtx(pt, uv, cl);
			}
		}

		if (has_indices)
		{
			for (int i = 0; i < indices_count; i++)
			{
				drawList->PrimWriteIdx(indices[i]);
			}
		}

		if (texture)
		{
			drawList->PopTextureID();
		}
	}

#else
	void ImPainter::drawGeometry(Texture* texture, const Vertex* vertices, int vertices_count, const int* indices, int indices_count, const Vec2& pos, float scale)
	{
		auto draw_list = ImGui::GetBackgroundDrawList();

		bool has_indices = (indices != nullptr && indices_count > 0);
		indices_count = has_indices ? indices_count : vertices_count ;

		if (texture)
		{
			draw_list->PushTextureID((void*)texture->texture());
		}

		// 1. 为所需的顶点和索引数量预留空间
		draw_list->PrimReserve(indices_count, vertices_count);

		// 2. 获取当前绘制列表的写入指针
		ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
		ImDrawVert* vtx_write = draw_list->_VtxWritePtr;

		// 3. 记录当前顶点缓冲区的起始偏移量
		ImDrawIdx vtx_offset = draw_list->_VtxCurrentIdx;

		// 4. 复制并转换顶点数据到 ImDrawList 的顶点缓冲区
		for (int i = 0; i < vertices_count; i++)
		{
			vtx_write[i].pos = ImVec2(pos.x + vertices[i].position.x * scale, pos.y + vertices[i].position.y * scale);
			vtx_write[i].uv = ImVec2(vertices[i].tex_coord.x, vertices[i].tex_coord.y);
			vtx_write[i].col = ImColor{ vertices[i].color.r, vertices[i].color.g, vertices[i].color.b, vertices[i].color.a };
		}

		if (!has_indices)
		{
			for (int i = 0; i < vertices_count; i++)
			{
				idx_write[i] = vtx_offset + i;
			}
		}
		else
		{
			// 5. 复制索引数据，并加上顶点偏移量
			for (int i = 0; i < indices_count; i++)
			{
				idx_write[i] = vtx_offset + indices[i];
			}
		}

		// 6. 更新绘制列表的内部指针和索引计数
		draw_list->_VtxWritePtr += vertices_count;
		draw_list->_IdxWritePtr += indices_count;
		draw_list->_VtxCurrentIdx += vertices_count;

		if (texture)
		{
			draw_list->PopTextureID();
		}
	}
#endif
}