#include "system_render.h"
#include "game/game_config.h"

namespace game 
{

    DeclareEcsSystem(RenderSystem, EcsPriority::Middle);

    Color RenderSystem::getBorderColor(CampSide side)
    {
        return _context.gameConfig().display.chess_border_color;
    }

    Color RenderSystem::getFontColor(CampSide side)
    {
        return _context.gameConfig().display.chess_font_color;
    }

    Color RenderSystem::getGroundColor(CampSide side)
    {
        switch (side)
        {
        case CampSide::Officer: return _context.gameConfig().display.chess_officer_ground_color;
        case CampSide::Rebel: return _context.gameConfig().display.chess_rebel_ground_color;
        case CampSide::Gangster: return _context.gameConfig().display.chess_gangster_ground_color;
        case CampSide::Civilian: return _context.gameConfig().display.chess_civil_ground_color;
        default: return Color::White;
        }

        return Color::White;
    }


    Color RenderSystem::getForeColor(CampSide side)
    {
        return Color::White;
    }

void RenderSystem::update(float delta)
{
    auto delta_tick = _context.frameTicker().deltaTicks();

    auto par_view = _context.registry().view<CompTransform, CompBindParticle>();
    for (auto& ent : par_view)
    {
        auto& compTransform = par_view.get<CompTransform>(ent);
        auto& compParticle = par_view.get<CompBindParticle>(ent);
        if (compParticle.particle)
        {
            compParticle.particle->SetPos(compTransform.position);
            compParticle.particle->Update(delta);
        }
    }

    auto sky_view = _context.registry().view<CompSkyEffect>();
    for (auto ent : sky_view)
    {
        auto& skyEffect = sky_view.get<CompSkyEffect>(ent);

        skyEffect.tween.step(delta);
    }

    auto sel_view = _context.registry().view<CompSelection>();
    for (auto ent : sel_view)
    {
        auto& select = sel_view.get<CompSelection>(ent);
        if(select.ticks >= _context.gameConfig().selection.period || select.ticks <= 0)
        {
            select.ticks = std::clamp(select.ticks, 0, 255);
            select.dir *= -1;
        }
        select.ticks += delta_tick * select.dir;
    }

}

void RenderSystem::draw()
{
    drawMarker();

    drawObjects();

    drawSkillEffect();

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

void RenderSystem::drawSkillEffect()
{
    auto& painter = _context.painter();
    auto& camera = _context.camera();
    
    auto lightning_view = _context.registry().view<CompLightningDisplay>();
    for(auto& ent : lightning_view)
    {
        auto& disp = lightning_view.get<CompLightningDisplay>(ent);
        painter.drawLightningData(disp.color, -_context.camera().getPos(), disp.data, disp.thickness);
    }

    auto projectile_view = _context.registry().view<CompTransform, CompProjectileDisplay>();
    for (auto& ent : projectile_view)
    {
        auto& trans = projectile_view.get<CompTransform>(ent);
        auto& disp = projectile_view.get<CompProjectileDisplay>(ent);

        Rect dstrect = { trans.position+disp.offset, disp.texture->rect().size()};
        dstrect = camera.projectRect(dstrect);

        painter.drawTexTile(disp.texture, dstrect, disp.orient, 0.0f, disp.color);
    }
}

void RenderSystem::drawMarker()
{
    auto& painter = _context.painter();
    auto& camera = _context.camera();

    // draw objects
    auto ent_view = _context.registry().view<CompNameId, CompTransform, CompMarkDisplay>();
    for (auto& ent : ent_view)
    {
        auto& nameid = ent_view.get<CompNameId>(ent);
        auto& transform = ent_view.get<CompTransform>(ent);
        auto& display = ent_view.get<CompMarkDisplay>(ent);

        float corner = _context.gameConfig().display.chess_corner;

        if (display.texture != nullptr)
        {
            Rect srcrect = display.tex_rect;
            Rect dstrect = Rect{ transform.position - transform.size / 2.0f, transform.size };
            if (transform.coord_mode == CoordMode::WorldSpace)
            {
                dstrect = camera.projectRect(dstrect);
            }
            painter.drawTexture(display.texture, srcrect, dstrect, corner, display.ground_color);
        }
        else
        {
            auto ground_color = display.ground_color;
            auto border_color = display.border_color;

            if (display.shape_type == ShapeType::Circle)
            {
                auto center = camera.projectPoint(transform.position);
                float radius = transform.size.x;

                painter.fillCircle(ground_color, center, radius, 50);
                painter.drawCircle(border_color, center, radius, 50);
            }
            else
            {
                auto rect = Rect{ transform.position - transform.size / 2.0f, transform.size };
                auto dstrect = camera.projectRect(rect);
                painter.fillRect(ground_color, dstrect, corner);
                painter.drawRect(border_color, dstrect, corner);
            }
        }
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
        auto pCompComm = _context.registry().try_get<CompComm>(ent);
        if (!pCompComm)
        {
            continue;
        }
        
        const auto& nameid = ent_view.get<CompNameId>(ent);
        const auto& transform = ent_view.get<CompTransform>(ent);
        const auto& display = ent_view.get<CompDisplay>(ent);
        if (!display.visible) 
        {
            continue;
        }

        float corner = _context.gameConfig().display.chess_corner;

        auto dstrect = Rect{ transform.position - transform.size / 2.0f, transform.size };
        if(transform.coord_mode == CoordMode::WorldSpace)
        {
            dstrect = camera.projectRect(dstrect);
        }

        int alpha = 255;
        auto pdead = _context.registry().try_get<CompDead>(ent);
        if(pdead)
        {
            auto total = _context.gameConfig().dying_ticks;
            auto half = total / 2.0f;
            if (pdead->ticks > half)
            {
                alpha = (int)(256 * (1.0f - ((pdead->ticks - half) / half)));
                alpha = std::clamp(alpha, 0, 255);
            }
        }

        // dragging tip
        auto dragComp = _context.registry().try_get<CompDragging>(ent);
        if (dragComp)
        {
            auto dstrect = Rect{ dragComp->tip_pos - dragComp->tip_size / 2.0f, dragComp->tip_size };
            if (transform.coord_mode == CoordMode::WorldSpace) {
                dstrect = camera.projectRect(dstrect);
            }

            painter.fillRect(dragComp->ground_color, dstrect, corner);
            painter.drawRect(dragComp->border_color, dstrect, corner, dragComp->border_size);
        }

        auto dead_ground_color = _context.gameConfig().display.chess_dead_ground_color;
        auto dead_border_color = _context.gameConfig().display.chess_dead_border_color;
        auto dead_font_color = _context.gameConfig().display.chess_dead_font_color;

        Color ground_color = pdead ? dead_ground_color : getGroundColor(pCompComm->side);
        Color border_color = pdead ? dead_border_color : getBorderColor(pCompComm->side);
        Color font_color = pdead ? dead_font_color : getFontColor(pCompComm->side);

        // draw back_ground
        {
            ground_color.a = alpha;
            border_color.a = alpha;

            if (display.ground_texture != nullptr)
            { 
                painter.drawTexTile(display.ground_texture, dstrect, 0.0f, corner, ground_color);
            }
            else
            {
                painter.fillRect(ground_color, dstrect, corner);

                // draw border
                {
                    painter.drawRect(border_color, dstrect, corner);
                }
            }
        }

        // draw fore_ground
        {
            auto fore_color = getForeColor(pCompComm->side);
            if (display.texture != nullptr)
            {
                painter.drawTexTile(display.texture, dstrect, 0.0f, corner, fore_color);
            }
            else
            {
                // draw text
                {
                    painter.drawText(nameid.name.c_str(), display.font, dstrect.pos() + Vec2{ 10,10 }, font_color);
                }
            }
        }


        // fight hp
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

        // selection
        auto pSelectComp = _context.registry().try_get<CompSelection>(ent);
        if (pSelectComp)
        {
            auto& selection = _context.gameConfig().selection;

            float alpha = (float)pSelectComp->ticks / (float)selection.period;

            auto border_color = selection.border_color;
            border_color.a = (int8_t)(alpha * 255);

            painter.fillRect(selection.ground_color, dstrect, corner);
            painter.drawRect(border_color, dstrect, corner, selection.border_size);
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

}


}

