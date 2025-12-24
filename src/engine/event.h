#pragma once

#include "wrapper.h"
#include "utility/signalslot.h"

namespace engine 
{

class EventDispatcher
{
public:
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

	signals::Signal<int> onKeyDown;
	signals::Signal<int> onKeyUp;

	signals::Signal<> onQuit;
	signals::Signal<const Event&> onSdlEvent;

public:
	EventDispatcher() = default;
	~EventDispatcher() = default;
	EventDispatcher(EventDispatcher&&) = delete;
	EventDispatcher(const EventDispatcher&) = delete;

	void run();

};

}