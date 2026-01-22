#pragma once

#include "wrapper.h"
#include "utility/signalslot.h"

namespace engine 
{

class EventDispatcher
{
public:
	signals::Signal<const Vec2&> onWindowResized;

	signals::Signal<const Vec2&> onMouseLeftDown;
	signals::Signal<const Vec2&> onMouseLeftUp;

	signals::Signal<const Vec2&> onMouseRightDown;
	signals::Signal<const Vec2&> onMouseRightUp;

	signals::Signal<const Vec2&> onMouseLeftClicked;
	signals::Signal<const Vec2&> onMouseRightClicked;

	signals::Signal<const Vec2&, const Vec2&> onMouseLeftDrag;
	signals::Signal<const Vec2&, const Vec2&> onMouseRightDrag;

	signals::Signal<const Vec2&, const Vec2&> onMouseMotion;
	signals::Signal<const Vec2&, float> onMouseWheel;

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
	entt::dispatcher _logicDispatcher;
	Vec2 _mousePos;

	EventFilterFunc _eventFilter;
};

}