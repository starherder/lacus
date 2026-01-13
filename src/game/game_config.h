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

		bool load(const fs::path& filepath);
		
	private:
		JsonConfig _jsonConfig;
	};




}