#include "system_render.h"

namespace game 
{


void RenderSystem::draw()
{
    Painter& painter = _context.painter();
    GameCamera& camera = _context.camera();

    auto ent_view = _context.registry().view<CompNameId, CompTransform, CompDisplay>();
    for (auto& ent : ent_view)
    {
        auto& nameid = ent_view.get<CompNameId>(ent);
        auto& transform = ent_view.get<CompTransform>(ent);
        auto& display = ent_view.get<CompDisplay>(ent);

        auto dstrect = Rect{ transform.position - transform.size / 2.0f, transform.size };
        dstrect = camera.projectRect(dstrect);

        if (display.texture != nullptr)
        {
            auto srcrect = display.tex_rect;
            painter.drawTexture(display.texture, srcrect, dstrect);
        }
        else
        {
            painter.fillRect(display.ground_color, dstrect);

            painter.drawRect(display.border_color, dstrect);

            painter.drawText(nameid.name.c_str(), display.font, dstrect.pos() + Vec2{ 10,10 }, display.font_color);
        }

        if(display.particle)
        {
            display.particle->Draw();
        }
    }

    if (_context.debugMode())
    {
        drawMotionDebug();
    }
}

void RenderSystem::drawMotionDebug()
{
    Renderer& renderer = _context.renderer();
    GameCamera& camera = _context.camera();

    auto ent_view = _context.registry().view<CompTransform, CompMotion>();
    for (auto& ent : ent_view)
    {
        auto& transform = ent_view.get<CompTransform>(ent);
        auto& motion = ent_view.get<CompMotion>(ent);
   
        if (motion.state == MotionState::Moving && motion.path.size() > 0)
        {
            renderer.setDrawColor(Color::Red);
            auto lstPos = camera.projectPoint(transform.position);

            for (auto it = motion.path_iterator; it != motion.path.rend(); ++it)
            {
                auto grid_center = _context.currentScene().getGridCenterPos(*it);
                auto grid_pos = camera.projectPoint(grid_center);

                renderer.drawLine(lstPos, grid_pos);

                auto rect = Rect{ grid_center - Vec2{10,10}, Vec2{20,20} };
                rect = camera.projectRect(rect);

                renderer.drawRect(rect);
                lstPos = grid_pos;
            }
        }
    }
}



}

