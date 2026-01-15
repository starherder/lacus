#include "im_painter.h"
#include "engine/application.h"
#include "engine/texture.h"
#include "engine/font.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"


namespace engine
{
	ImPainter::ImPainter(Application& app) : _application(app)
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

	ImColor ImPainter::toImColor(const Color& color)
	{
		return ImColor{color.r, color.g, color.b, color.a};
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

	void ImPainter::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color)
	{
		assert(font);

#ifdef USE_IMGUI_AS_RENDER_ENGINE		
		ImGui::GetBackgroundDrawList()->AddText(font->imFont, font->size, {pos.x, pos.y}, toImColor(color), text.c_str());
#else
		ImGui::GetBackgroundDrawList()->AddText({ pos.x, pos.y }, toImColor(color), text.c_str());
#endif
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
			spdlog::error("font file({}) NOT found.", fontPath.string());
			return;
		}

		auto& io = ImGui::GetIO();
		auto fonts = ImGui::GetIO().Fonts;
		auto font = fonts->AddFontFromFileTTF(fontPath.string().c_str(), fontSize, nullptr, fonts->GetGlyphRangesChineseFull());
		if (!font)
		{
			spdlog::error("load font {} failed. use default.", fontPath.string());
			io.Fonts->AddFontDefault();
		}
	}

}