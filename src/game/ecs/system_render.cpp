#include "system_render.h"

namespace game 
{


void RenderSystem::update(float delta)
{
    auto parEnts = _context.registry().view<CompTransform, CompBindParticle>();
    for (auto& ent : parEnts)
    {
        auto& compTransform = parEnts.get<CompTransform>(ent);
        auto& compParticle = parEnts.get<CompBindParticle>(ent);
        if (compParticle.particle)
        {
            compParticle.particle->Update(delta);
        }
    }
}

void RenderSystem::draw()
{
    Painter& painter = _context.painter();
    GameCamera& camera = _context.camera();

    // draw objects
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

        auto selectComp = _context.registry().try_get<CompSelection>(ent);
        if (selectComp && selectComp->selected)
        {
            painter.drawRect(selectComp->border_color, dstrect, 0, selectComp->border_size);
        }

        if(display.particle)
        {
            display.particle->Draw();
        }
    }

    // draw particles
    auto parEnts = _context.registry().view<CompTransform, CompBindParticle>();
    for (auto& ent : parEnts)
    {
        auto& compTransform = parEnts.get<CompTransform>(ent);
        auto& compParticle = parEnts.get<CompBindParticle>(ent);
        auto& particle = compParticle.particle;
        if (particle)
        {
            particle->SetPos(compTransform.position);
            particle->Draw();
        }
    }

    if (_context.debugMode())
    {
        drawSceneDebug();

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


void RenderSystem::drawSceneDebug()
{
    if (!_context.debugMode())
    {
        return;
    }

    // -------------- show collision info ------------------
    auto& _collisionDebugRects = _context.currentScene().getCollisionDebugRects();

    static std::vector<Rect> rects;
    rects.clear();
    rects.reserve(_collisionDebugRects.size());
    rects.insert(rects.begin(), _collisionDebugRects.begin(), _collisionDebugRects.end());
    _context.camera().projectRects(rects.data(), (int)rects.size());

    auto& renderer = _context.renderer();
    renderer.setDrawColor(Color{ 255, 0, 0, 100 });
    renderer.drawFillRects(rects.data(), (int)rects.size());

    // -------------- show grids ------------------
    auto mapSize = _context.currentScene().mapSize(); 
    auto tileSize = _context.currentScene().tileSize();
    for (int x = 0; x <= mapSize.x; ++x)
    {
        auto srcPos = _context.camera().projectPoint({ x * tileSize.x, 0 });
        auto dstPos = _context.camera().projectPoint({ x * tileSize.x, mapSize.y * tileSize.y });
        renderer.drawLine(srcPos, dstPos);
    }

    for (int y = 0; y <= mapSize.y; ++y)
    {
        auto srcPos = _context.camera().projectPoint({ 0, y * tileSize.y });
        auto dstPos = _context.camera().projectPoint({ tileSize.x * mapSize.x, y * tileSize.y });
        renderer.drawLine(srcPos, dstPos);
    }

    renderer.setDrawColor({ 0, 0, 255, 255 });

    for (int x = 0; x <= mapSize.x; ++x)
    {
        for (int y = 0; y <= mapSize.y; ++y)
        {
            auto& objs = _context.currentScene().getObjectsInGrid({ x, y });
            auto pos = _context.currentScene().getGridCenterPos({ x, y });

            pos = _context.camera().projectPoint(pos);
            renderer.drawDebugTextFormat(pos, "%d", (int)objs.size());
        }
    }
}



}

