#pragma once

#include "wrapper.h"
#include "utility/signalslot.h"

namespace engine {


class EventDispatcher
{
	template <typename... _ArgTypes>
	using signal = utility::sigslot::Signal<_ArgTypes...>;

public:
	signal<const Vec2&> onMouseLeftDown;
	signal<const Vec2&> onMouseLeftUp;

	signal<const Vec2&> onMouseRightDown;
	signal<const Vec2&> onMouseRightUp;

	signal<const Vec2&> onMouseLeftClicked;
	signal<const Vec2&> onMouseRightClicked;

	signal<const Vec2&, const Vec2&> onMouseLeftDrag;
	signal<const Vec2&, const Vec2&> onMouseRightDrag;

	signal<const Vec2&, const Vec2&> onMouseMotion;
	signal<const Vec2&, float> onMouseWheel;

	signal<int> onKeyDown;
	signal<int> onKeyUp;

	signal<> onQuit;
	signal<const Event&> onSdlEvent;

public:
	EventDispatcher() = default;
	~EventDispatcher() = default;
	EventDispatcher(EventDispatcher&&) = delete;
	EventDispatcher(const EventDispatcher&) = delete;

	void run();

};

}