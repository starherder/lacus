#include "event.h"


namespace engine {

void EventDispatcher::run()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		onSdlEvent.emit(e);

		switch(e.type)
		{
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			if (e.button.button == SDL_BUTTON_LEFT) {
				onMouseLeftDown({ e.button.x, e.button.y });
			}
			if (e.button.button == SDL_BUTTON_RIGHT) {
				onMouseRightDown({ e.button.x, e.button.y });
			}
		}break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			if (e.button.button == SDL_BUTTON_LEFT) {
				onMouseLeftUp({e.button.x, e.button.y});
				onMouseLeftClicked.emit({e.button.x, e.button.y});
			}
			if (e.button.button == SDL_BUTTON_RIGHT) {
				onMouseRightUp({ e.button.x, e.button.y });
				onMouseRightClicked.emit({e.button.x, e.button.y});
			}
		}break;
		case SDL_EVENT_MOUSE_MOTION:
		{
			onMouseMotion.emit({e.motion.x, e.motion.y}, {e.motion.xrel, e.motion.yrel});
			if(e.button.button == SDL_BUTTON_LEFT) {
				onMouseLeftDrag.emit({e.motion.x, e.motion.y}, {e.motion.xrel, e.motion.yrel});
			}
			if (e.button.button == SDL_BUTTON_RIGHT) {
				onMouseRightDrag.emit({ e.motion.x, e.motion.y }, { e.motion.xrel, e.motion.yrel });
			}
		}break;
		case SDL_EVENT_MOUSE_WHEEL:
		{
			float x, y; SDL_GetMouseState(&x, &y);
			float dir = e.wheel.y;
			onMouseWheel.emit({x, y}, dir);
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
			break;
		}
	}
}

}