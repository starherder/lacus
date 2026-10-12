#pragma once

#include "wrapper.h"
#include "utility/signalslot.h"
#include <cstdint>

namespace engine 
{

enum class PointerPhase
{
	Down,
	Up,
	Move,
	Cancel,
};

struct PointerEvent
{
	std::int64_t id = 0;
	Vec2 pos = {0.0f, 0.0f};
	Vec2 delta = {0.0f, 0.0f};
	PointerPhase phase = PointerPhase::Move;
	bool primary = true;
	bool pressed = false;
	bool touch = false;
};

class EventDispatcher
{
public:
	signals::Signal<const Vec2&> onWindowResized;
	signals::Signal<WindowEventType, const Vec2&> onWindowEvent;
	signals::Signal<const Vec2&> onWindowEnterFullscreen;
	signals::Signal<const Vec2&> onWindowLeaveFullscreen;

	signals::Signal<const Vec2&> onMouseLeftDown;
	signals::Signal<const Vec2&> onMouseLeftUp;

	signals::Signal<const Vec2&> onMouseRightDown;
	signals::Signal<const Vec2&> onMouseRightUp;

	signals::Signal<const Vec2&> onMouseLeftClicked;
	signals::Signal<const Vec2&> onMouseRightClicked;

	signals::Signal<const Vec2&> onMouseLeftDragStart;
	signals::Signal<const Vec2&> onMouseLeftDragFinish;

	signals::Signal<const Vec2&, const Vec2&> onMouseLeftDrag;
	signals::Signal<const Vec2&, const Vec2&> onMouseRightDrag;

	signals::Signal<const Vec2&, const Vec2&> onMouseMotion;
	signals::Signal<const Vec2&, float> onMouseWheel;

	signals::Signal<const PointerEvent&> onPointerEvent;

	signals::Signal<KeyCode> onKeyDown;
	signals::Signal<KeyCode> onKeyUp;

	signals::Signal<> onQuit;
	signals::Signal<const Event&> onSdlEvent;

	using EventFilterFunc = std::function<bool(const Event&)>;

public:
	EventDispatcher() = default;
	~EventDispatcher() = default;
	EventDispatcher(EventDispatcher&&) = delete;
	EventDispatcher(const EventDispatcher&) = delete;

	void run();
	
	bool isShiftKeyDown();

	bool isCtrlKeyDown();

	auto& logicDispatcher() { return _logicDispatcher;  }

	const Vec2& mousePos() { return _mousePos; }

	bool isEventFilter(const Event& e);

	void setEventFilterFunc(EventFilterFunc func);

private:
	void emitPointerFromMouse(const Vec2& pos, const Vec2& delta, PointerPhase phase, bool pressed);
	void emitPointerFromTouch(const Event& e, PointerPhase phase);

	entt::dispatcher _logicDispatcher;

	Vec2 _mousePos;

	bool _isDragging = false;
	bool _hasPrimaryFinger = false;
	SDL_FingerID _primaryFinger = 0;

	EventFilterFunc _eventFilter;
};

}
