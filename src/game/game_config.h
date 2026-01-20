#pragma once

#include "engine/config.h"

namespace game
{
	using namespace engine;

	class GameConfig 
	{
	public:
		struct Motion {
			std::string walk = "cubicIn";
			std::string swim = "linear";
		} motion;

		struct FloatText {
			std::string font_name = "fonts/msyh.ttf";
			float font_size = 15.0f;
			Color color_inc_hp = Color::Green;
			Color color_dec_hp = Color::Red;
			std::string tween_mode = "cubicIn";
			int float_ticks = 1000;
		} float_text;

		struct Display {
			float chess_corner = 5.0f;
			Color chess_dead_ground_color = Color::Light;
			Color chess_dead_border_color = Color::Gray;
			Color chess_dead_font_color = Color::Dark;
		}display;

		int dying_ticks = 1000;

		struct Scenes {
			std::string first_scene;
		}scenes;

		bool load(const fs::path& filepath);
		
	private:
		JsonConfig _jsonConfig;
	};




}