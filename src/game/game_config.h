#pragma once

#include "engine/config.h"

namespace game
{
	using namespace engine;

	class GameConfig final
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

			Color chess_gangster_ground_color = Color::LightGreen;
			Color chess_officer_ground_color = Color::LightRed;
			Color chess_rebel_ground_color = Color::LightYellow;
			Color chess_civil_ground_color = Color::Light;

			Color chess_font_color = Color::Blue;

			Color chess_border_color = Color::Dark;
			float chess_border_size = 2.0f;
		}display;

		struct Selection {
			Color border_color = Color::Yellow;
			Color ground_color = { 255,255,255,128 };
			float border_size = 10.0f;
			int period = 500;
		} selection;

		struct Script {
			std::string entry = "lua/main.lua";
		} script;

		int dying_ticks = 1000;

		struct Pickup {
			int dead_drop_range = 130;
			int pick_range = 30;
			int pick_period = 1000;
		}pickup;

		struct Scenes {
			std::string first_scene;
		}scenes;

		bool load(const fs::path& filepath);
		
	private:
		JsonConfig _jsonConfig;
	};




}