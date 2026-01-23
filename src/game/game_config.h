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
			int font_size = 20;
			Color color_inc = Color::Green;
			Color color_dec = Color::Red;
			std::string tween_mode = "cubicIn";
			int float_ticks = 1000;
		} float_text;

		struct Display {
			float chess_corner = 5.0f;
			Color chess_dead_ground_color = Color::Light;
			Color chess_dead_border_color = Color::Gray;
			Color chess_dead_font_color = Color::Dark;

			Color ground_color_drag_ok = Color::LightGreen;
			Color border_color_drag_ok = Color::Green;
			Color ground_color_drag_error = Color::LightRed;
			Color border_color_drag_error = Color::Red;

			Color border_color_camp_officer = Color::Red;
			Color border_color_camp_ganster = Color::Green;
			Color border_color_camp_rebel = Color::Pink;
			Color border_color_camp_civilian = Color::White;
		}display;

		int dying_ticks = 1000;

		int dead_drop_range = 130;

		struct Scenes {
			std::string first_scene;
		}scenes;

		bool load(const fs::path& filepath);
		
	private:
		JsonConfig _jsonConfig;
	};




}