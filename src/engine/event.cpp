#include "event.h"

#include "magic_enum/magic_enum.h"

namespace engine {

bool EventDispatcher::isShiftKeyDown()
{
	const bool* state = SDL_GetKeyboardState(nullptr);
	return state[(int)SDL_SCANCODE_LSHIFT] || state[(int)SDL_SCANCODE_RSHIFT];
}

bool EventDispatcher::isCtrlKeyDown()
{
	const bool* state = SDL_GetKeyboardState(nullptr);
	return state[(int)SDL_SCANCODE_LCTRL] || state[(int)SDL_SCANCODE_RCTRL];
}

bool EventDispatcher::isEventFilter(const Event& e)
{
	if (_eventFilter)
	{
		return _eventFilter(e);
	}
	return false;
}

void EventDispatcher::setEventFilterFunc(EventFilterFunc func)
{
	_eventFilter = func;
}

void EventDispatcher::emitPointerFromMouse(const Vec2& pos, const Vec2& delta, PointerPhase phase, bool pressed)
{
	PointerEvent pointer;
	pointer.id = 0;
	pointer.pos = pos;
	pointer.delta = delta;
	pointer.phase = phase;
	pointer.primary = true;
	pointer.pressed = pressed;
	pointer.touch = false;
	onPointerEvent.emit(pointer);
}

void EventDispatcher::emitPointerFromTouch(const Event& e, PointerPhase phase)
{
	SDL_Window* window = SDL_GetWindowFromID(e.tfinger.windowID);
	int width = 0;
	int height = 0;
	if (window)
	{
		SDL_GetWindowSize(window, &width, &height);
	}

	Vec2 windowSize{ (float)width, (float)height };
	Vec2 pos{ e.tfinger.x * windowSize.x, e.tfinger.y * windowSize.y };
	Vec2 delta{ e.tfinger.dx * windowSize.x, e.tfinger.dy * windowSize.y };
	_mousePos = pos;

	if (phase == PointerPhase::Down && !_hasPrimaryFinger)
	{
		_primaryFinger = e.tfinger.fingerID;
		_hasPrimaryFinger = true;
	}

	bool primary = !_hasPrimaryFinger || e.tfinger.fingerID == _primaryFinger;

	PointerEvent pointer;
	pointer.id = (std::int64_t)e.tfinger.fingerID;
	pointer.pos = pos;
	pointer.delta = delta;
	pointer.phase = phase;
	pointer.primary = primary;
	pointer.pressed = phase == PointerPhase::Down || phase == PointerPhase::Move;
	pointer.touch = true;
	onPointerEvent.emit(pointer);

	if ((phase == PointerPhase::Up || phase == PointerPhase::Cancel) && primary)
	{
		_hasPrimaryFinger = false;
		_primaryFinger = 0;
	}
}

void EventDispatcher::run()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		onSdlEvent.emit(e);

		if (isEventFilter(e))
		{
			continue;
		}

		switch(e.type)
		{
		case SDL_EVENT_WINDOW_RESIZED:
		{
			Vec2 size{e.window.data1, e.window.data2};
			onWindowResized(size);
			onWindowEvent(WindowEventType::Resized, size);
		}break;
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
		{
			Vec2 size{e.window.data1, e.window.data2};
			onWindowEvent(WindowEventType::PixelSizeChanged, size);
		}break;
		case SDL_EVENT_WINDOW_MAXIMIZED:
		{
			Vec2 size{e.window.data1, e.window.data2};
			onWindowEvent(WindowEventType::Maximized, size);
		}break;
		case SDL_EVENT_WINDOW_RESTORED:
		{
			Vec2 size{e.window.data1, e.window.data2};
			onWindowEvent(WindowEventType::Restored, size);
		}break;
		case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
		{
			Vec2 size{e.window.data1, e.window.data2};
			onWindowEnterFullscreen(size);
			onWindowEvent(WindowEventType::EnterFullscreen, size);
		}break;
		case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
		{
			Vec2 size{e.window.data1, e.window.data2};
			onWindowLeaveFullscreen(size);
			onWindowEvent(WindowEventType::LeaveFullscreen, size);
		}break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			if (e.button.which == SDL_TOUCH_MOUSEID)
			{
				break;
			}
			if (e.button.button == SDL_BUTTON_LEFT) 
			{
				emitPointerFromMouse({ e.button.x, e.button.y }, {0.0f, 0.0f}, PointerPhase::Down, true);
				onMouseLeftDown({ e.button.x, e.button.y });
			}
			if (e.button.button == SDL_BUTTON_RIGHT)
			{
				onMouseRightDown({ e.button.x, e.button.y });
			}
		}break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			if (e.button.which == SDL_TOUCH_MOUSEID)
			{
				break;
			}
			if (e.button.button == SDL_BUTTON_LEFT) 
			{
				emitPointerFromMouse({ e.button.x, e.button.y }, {0.0f, 0.0f}, PointerPhase::Up, false);
				onMouseLeftUp({e.button.x, e.button.y});
				onMouseLeftClicked.emit({e.button.x, e.button.y});

				if (_isDragging)
				{
					_isDragging = false;
					onMouseLeftDragFinish.emit({ e.button.x, e.button.y });
				}
			}
			if (e.button.button == SDL_BUTTON_RIGHT)
			{
				onMouseRightUp({ e.button.x, e.button.y });
				onMouseRightClicked.emit({e.button.x, e.button.y});
			}
		}break;
		case SDL_EVENT_MOUSE_MOTION:
		{
			if (e.motion.which == SDL_TOUCH_MOUSEID)
			{
				break;
			}
			_mousePos = { e.motion.x, e.motion.y };

			emitPointerFromMouse({ e.motion.x, e.motion.y }, { e.motion.xrel, e.motion.yrel }, PointerPhase::Move, (e.motion.state & SDL_BUTTON_LMASK) != 0);

			onMouseMotion.emit({e.motion.x, e.motion.y}, {e.motion.xrel, e.motion.yrel});
			if(e.motion.state & SDL_BUTTON_LMASK) 
			{
				onMouseLeftDrag.emit({e.motion.x, e.motion.y}, {e.motion.xrel, e.motion.yrel});

				if (!_isDragging) 
				{
					_isDragging = true;
					onMouseLeftDragStart.emit({ e.motion.x, e.motion.y });
				}
			}
			if (e.motion.state & SDL_BUTTON_RMASK) 
			{
				onMouseRightDrag.emit({ e.motion.x, e.motion.y }, { e.motion.xrel, e.motion.yrel });
			}
		}break;
		case SDL_EVENT_MOUSE_WHEEL:
		{
			float x, y; SDL_GetMouseState(&x, &y);
			float dir = e.wheel.y;
			onMouseWheel.emit({x, y}, dir);
		}break;
		case SDL_EVENT_FINGER_DOWN:
		{
			emitPointerFromTouch(e, PointerPhase::Down);
		}break;
		case SDL_EVENT_FINGER_MOTION:
		{
			emitPointerFromTouch(e, PointerPhase::Move);
		}break;
		case SDL_EVENT_FINGER_UP:
		{
			emitPointerFromTouch(e, PointerPhase::Up);
		}break;
		case SDL_EVENT_FINGER_CANCELED:
		{
			emitPointerFromTouch(e, PointerPhase::Cancel);
		}break;
		case SDL_EVENT_KEY_DOWN:
		{
			onKeyDown.emit(e.key.key);
		}break;
		case SDL_EVENT_KEY_UP:
		{
			onKeyUp.emit(e.key.key);
		}break;
		case SDL_EVENT_QUIT:
		{
			onQuit.emit();
		}break;
		default:
		{
			break;
		}
		}
	}
}

}
