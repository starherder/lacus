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
            compParticle.particle->SetPos(compTransform.position);
            compParticle.particle->Update(delta);
        }
    }

    auto ent_view = _context.registry().view<CompSkyEffect>();
    for (auto ent : ent_view)
    {
        auto& skyEffect = _context.registry().get<CompSkyEffect>(ent);

        auto delta = _context.frameTicker().deltaTicks();
        skyEffect.tween.step(delta);
    }
}

void RenderSystem::draw()
{
    drawObjects();

    drawParticles();

    drawSkyEffect();

    drawFightText();

    if (_context.debugMode())
    {
        drawSceneDebug();

        drawMotionDebug();
    }
}

void RenderSystem::drawSkyEffect()
{
    auto sz = _context.applicaton().window().getSize();

    auto ent_view = _context.registry().view<CompSkyEffect>();
    for (auto ent : ent_view)
    {
        auto& skyEffect = _context.registry().get<CompSkyEffect>(ent);
        _context.painter().fillRect(skyEffect.color, { {0,0}, sz });
        return;
    }
}

void RenderSystem::drawFightText()
{
    auto ent_view = _context.registry().view<CompTransform, CompFightText>();
    for (auto& ent : ent_view)
    {
        auto& compTrans = ent_view.get<CompTransform>(ent);
        auto& compFt = ent_view.get<CompFightText>(ent);

        auto pos = _context.camera().projectPoint(compTrans.position);
        _context.painter().drawText(compFt.text, compFt.font, pos, compFt.color);
    }
}

void RenderSystem::drawObjects()
{ 
    auto& painter = _context.painter();
    auto& camera = _context.camera();

    // draw objects
    auto ent_view = _context.registry().view<CompNameId, CompTransform, CompDisplay>();
    for (auto& ent : ent_view)
    {
        auto& nameid = ent_view.get<CompNameId>(ent);
        auto& transform = ent_view.get<CompTransform>(ent);
        auto& display = ent_view.get<CompDisplay>(ent);

        auto dstrect = Rect{ transform.position - transform.size / 2.0f, transform.size };
        if(transform.coord_mode == CoordMode::WorldSpace)
        {
            dstrect = camera.projectRect(dstrect);
        }

        int alpha = 255;
        float corner = _context.gameConfig().display.chess_corner;

        auto pdead = _context.registry().try_get<CompDead>(ent);
        if (pdead)
        {
            auto total = _context.gameConfig().dying_ticks;
            auto half = total / 2.0f;
            if (pdead->ticks > half)
            {
                alpha = (int)(256 * (1.0f - ((pdead->ticks - half) / half)));
                alpha = std::clamp(alpha, 0, 255);
            }
        }

        if (display.texture != nullptr)
        {
            auto srcrect = display.tex_rect;
            painter.drawTexture(display.texture, srcrect, dstrect, corner, { 255, 255, 255, alpha });
        }
        else
        {
            auto ground_color = display.ground_color;
            auto border_color = display.border_color;
            auto font_color = display.font_color;

            if(pdead)
            {
                ground_color = _context.gameConfig().display.chess_dead_ground_color; ground_color.a = alpha;
                border_color = _context.gameConfig().display.chess_dead_border_color; border_color.a = alpha;
                font_color = _context.gameConfig().display.chess_dead_font_color; font_color.a = alpha;
            }

            painter.fillRect(ground_color, dstrect, corner);
            painter.drawRect(border_color, dstrect, corner);
            painter.drawText(nameid.name.c_str(), display.font, dstrect.pos() + Vec2{ 10,10 }, font_color);
        }

        auto pFight = _context.registry().try_get<CompFightProp>(ent);
        if(pFight)
        {
            if (pFight->hpm > 0.0f) 
            {
                float ratio = pFight->hp/pFight->hpm;

                Rect bgrect = {dstrect.x+5, dstrect.y+dstrect.w-10, dstrect.w-10, 5.0f};
                Rect ftrect = {bgrect.x, bgrect.y, bgrect.w*ratio, bgrect.h};

                Color bk_color = Color::Light; bk_color.a = alpha;
                Color font_color = Color::Red; font_color.a = alpha;

                painter.fillRect(bk_color, bgrect);
                painter.fillRect(font_color, ftrect);
            }
        }

        auto selectComp = _context.registry().try_get<CompSelection>(ent);
        if (selectComp)
        {
            painter.fillRect(selectComp->ground_color, dstrect, corner);
            painter.drawRect(selectComp->border_color, dstrect, corner, selectComp->border_size);
        }
    }
}

void RenderSystem::drawParticles()
{
    auto& camera = _context.camera();

    // draw particles
    auto parEnts = _context.registry().view<CompTransform, CompBindParticle>();
    for (auto& ent : parEnts)
    {
        auto& compTransform = parEnts.get<CompTransform>(ent);
        auto& compParticle = parEnts.get<CompBindParticle>(ent);

        Camera* pcamera = nullptr;
        if(compTransform.coord_mode == CoordMode::WorldSpace)
        {
            pcamera = &camera;
        }

        auto& particle = compParticle.particle;
        if (particle)
        {
            particle->Draw(pcamera);
        }
    }
}

void RenderSystem::drawMotionDebug()
{ 
    auto& painter = _context.painter();
    auto& camera = _context.camera();

    auto ent_view = _context.registry().view<CompTransform, CompMotion>();
    for (auto& ent : ent_view)
    {
        auto& transform = ent_view.get<CompTransform>(ent);
        auto& motion = ent_view.get<CompMotion>(ent);
   
        if (motion.state == MotionState::Moving && motion.path.size() > 0)
        {
            auto lstPos = camera.projectPoint(transform.position);

            for (auto it = motion.path_iterator; it != motion.path.rend(); ++it)
            {
                auto grid_center = _context.scene().getGridCenterPos(*it);
                auto grid_pos = camera.projectPoint(grid_center);
                painter.drawLine(Color::Red, lstPos, grid_pos);

                auto rect = Rect{ grid_center - Vec2{10,10}, Vec2{20,20} };
                rect = camera.projectRect(rect);
                painter.drawRect(Color::Red, rect);

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

    auto& renderer = _context.renderer();
    auto& painter = _context.painter();

    // -------------- show collision info ------------------
    auto& _collisionDebugRects = _context.scene().getCollisionDebugRects();

    static std::vector<Rect> rects;
    rects.clear();
    rects.reserve(_collisionDebugRects.size());
    rects.insert(rects.begin(), _collisionDebugRects.begin(), _collisionDebugRects.end());
    _context.camera().projectRects(rects.data(), (int)rects.size());

    renderer.setDrawColor(Color{ 255, 0, 0, 100 });
    renderer.drawFillRects(rects.data(), (int)rects.size());

    // -------------- show grids ------------------
    auto mapSize = _context.scene().mapSize(); 
    auto tileSize = _context.scene().tileSize();
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
            auto& objs = _context.scene().getObjectsInGrid({ x, y });
            auto pos = _context.scene().getGridCenterPos({ x, y });

            pos = _context.camera().projectPoint(pos);
            renderer.drawDebugTextFormat(pos, "%d", (int)objs.size());
        }
    }
}



}

