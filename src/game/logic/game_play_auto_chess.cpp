#include "game_play_auto_chess.h"

#include "game/ecs/comm_event.h"
#include "game/scene/game_scene.h"
#include "magic_enum/magic_enum.h"
#include "game/game_config.h"


namespace game 
{

	GamePlayAutoChess::GamePlayAutoChess(GameContext& context) : GamePlay(context)
	{
		context.eventDispatcher().onMouseLeftDown.connect(this, &GamePlayAutoChess::onMouseLeftPressed, -1);
		context.eventDispatcher().onMouseLeftUp.connect(this, &GamePlayAutoChess::onMouseLeftRelease, -1);
		context.eventDispatcher().onMouseLeftDrag.connect(this, &GamePlayAutoChess::onMouseLeftDrag, -1);
		context.eventDispatcher().onMouseMotion.connect(this, &GamePlayAutoChess::onMouseMotion, -1);

		context.eventDispatcher().onKeyDown.connect(this, &GamePlayAutoChess::onKeyDown, -1);

		context.eventDispatcher().onMouseLeftDragStart.connect(this, &GamePlayAutoChess::onMouseLeftDragStart, -1);
		context.eventDispatcher().onMouseLeftDragFinish.connect(this, &GamePlayAutoChess::onMouseLeftDragFinish, -1);

		context.eventDispatcher().onMouseLeftClicked.connect(this, &GamePlayAutoChess::onMouseLeftClick, -1);
		context.eventDispatcher().onMouseRightClicked.connect(this, &GamePlayAutoChess::onMouseRightClick, -1);
	}

	void GamePlayAutoChess::update(float deltaTime)
	{
	}

	void GamePlayAutoChess::draw()
	{
	}

	void GamePlayAutoChess::onKeyDown(KeyCode key)
	{
	}

	void GamePlayAutoChess::onMouseLeftPressed(const Vec2& pos)
	{
		auto scenePos = _context.camera().screenToWorld(pos);
		auto selObj = _context.scene().findObjectAtPos(scenePos);
        if(selObj == _selectEntity)
        {
            return;
        }

		if (_selectEntity != entt::null)
		{
            onRoleUnselect();
		}

		_selectEntity = selObj;

        onRoleSelect();
	}

	void GamePlayAutoChess::onMouseLeftRelease(const Vec2& pos)
	{

	}

	void GamePlayAutoChess::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
	{
		if (dragSelectActorInProgress(pos))
		{
		    slotContext().setBreak(true);
		}
	}

	void GamePlayAutoChess::onMouseMotion(const Vec2& pos, const Vec2& offset)
	{
	}

	void GamePlayAutoChess::onMouseLeftClick(const Vec2& pos)
	{
	}

	void GamePlayAutoChess::onMouseRightClick(const Vec2& pos)
{
        if (!_context.registry().valid(_selectEntity)) {
            return;
        }

        auto pcomm = _context.registry().try_get<CompComm>(_selectEntity);
        if (!pcomm || pcomm->side != CampSide::Gangster) {
            return;
        }

        auto scenePos = _context.camera().screenToWorld(pos);
        moveSelectActor(scenePos);
	}

	void GamePlayAutoChess::onMouseLeftDragStart(const Vec2& pos)
	{
        if ( objectDragable() && dragSelectActor(pos))
        {
            slotContext().setBreak(true);
        }
	}

	void GamePlayAutoChess::onMouseLeftDragFinish(const Vec2& pos)
	{
        if (objectDragable() && dropSelectActor(pos))
        {
            slotContext().setBreak(true);
        }
	}

    void GamePlayAutoChess::onRoleSelect()
    {
        _context.dispatcher().trigger(EvtObjectSelection{ _selectEntity });
        
        if(_context.registry().try_get<CompAutoMotion>(_selectEntity))
        {
            _context.dispatcher().trigger(EvtRoleStopMotion{ _selectEntity });
        }

        auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
        if (bevComp && bevComp->bevtree)
        {
            bevComp->bevtree->stop();
        }
    }

    void GamePlayAutoChess::onRoleUnselect()
    {
        auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
        if (bevComp && bevComp->bevtree)
        {
            bevComp->bevtree->start();
        }

        _context.dispatcher().trigger(EvtObjectUnselect{ _selectEntity });
    }

    void GamePlayAutoChess::moveSelectActor(const Vec2& pos)
    {
        if (_context.registry().valid(_selectEntity) == false)
        {
            return;
        }

        auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
        if (bevComp && bevComp->bevtree)
        {
            bevComp->bevtree->stop();
        }

        if (_context.registry().try_get<CompMoveCfg>(_selectEntity))
        {
            auto gridPos = _context.scene().getGridFromPos(pos);
            _context.dispatcher().trigger(EvtMoveToGrid{ _selectEntity, gridPos, true });
        }
    }

    bool GamePlayAutoChess::dragSelectActor(const Vec2& pos)
    {
        if (!_context.registry().valid(_selectEntity) || !objectDragable())
        {
            return false;
        }

        auto pDead = _context.registry().try_get<CompDead>(_selectEntity);
        if (pDead)
        {
            return false;
        }

        auto pTrans = _context.registry().try_get<CompTransform>(_selectEntity);
        if (!pTrans)
        {
            return false;
        }

        auto dstpos = pTrans->position;
        _context.registry().emplace_or_replace<CompDragging>(_selectEntity, CompDragging{ dstpos, dstpos, pTrans->size });

        auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
        if (bevComp && bevComp->bevtree)
        {
            bevComp->bevtree->stop();
        }

        _context.scene().removeObjectFromScene(_selectEntity);

        return true;
    }

    bool GamePlayAutoChess::dragSelectActorInProgress(const Vec2& pos)
    {
        if (!_context.registry().valid(_selectEntity) || !objectDragable())
        {
            return false;
        }

        auto pDragging = _context.registry().try_get<CompDragging>(_selectEntity);
        if (!pDragging)
        {
            return false;
        }

        auto pTrans = _context.registry().try_get<CompTransform>(_selectEntity);
        if (!pTrans)
        {
            return false;
        }

        auto dstpos = _context.camera().screenToWorld(pos);
        if (!canDropToPos(dstpos))
        {
            pDragging->ground_color = _context.gameConfig().display.ground_color_drag_error;
            pDragging->border_color = _context.gameConfig().display.border_color_drag_error;
        }
        else
        {
            pDragging->ground_color = _context.gameConfig().display.ground_color_drag_ok;
            pDragging->border_color = _context.gameConfig().display.border_color_drag_ok;
        }

        pTrans->position = dstpos;
        pDragging->tip_pos = _context.scene().normalToGridPos(dstpos);

        return true;
    }

    bool GamePlayAutoChess::dropSelectActor(const Vec2& pos)
    {
        if (!_context.registry().valid(_selectEntity) || !objectDragable())
        {
            return false;
        }

        auto pDragging = _context.registry().try_get<CompDragging>(_selectEntity);
        auto pTrans = _context.registry().try_get<CompTransform>(_selectEntity);
        if (!pDragging || !pTrans)
        {
            return false;
        }

        auto dstpos = _context.camera().screenToWorld(pos);
        if (canDropToPos(dstpos))
        {
            dstpos = _context.scene().normalToGridPos(dstpos);
            _context.dispatcher().trigger(EvtRoleOnDrop{ _selectEntity, dstpos });
        }
        else
        {
            dstpos = pDragging->origin_pos;
        }

        _context.scene().addObjectToScene(_selectEntity);

        _context.scene().setObjectPos(_selectEntity, dstpos);

        _context.registry().remove<CompDragging>(_selectEntity);

        auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
        if (bevComp && bevComp->bevtree)
        {
            bevComp->bevtree->start();
        }

        EvtRoleCrossGrid e;
        e.actor = _selectEntity;
        e.cur_grid = _context.scene().getObjectGrid(_selectEntity);
        e.lst_grid = _context.scene().getObjectGrid(_selectEntity);
        _context.dispatcher().trigger(e);

        return true;
    }

    bool GamePlayAutoChess::canDropToPos(const Vec2& pos)
    {
        auto walktype = _context.scene().getGridWalkType(_context.scene().getGridFromPos(pos));
        return walktype != (int)tilemap::WalkType::Collision;
    }

}