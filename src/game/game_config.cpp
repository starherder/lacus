#include "game_config.h"
#include "engine/wrapper.h"
#include "nlohmann/json.hpp"

namespace game
{



	bool GameConfig::load(const fs::path& filepath)
	{
		bool res = _jsonConfig.load(filepath);
		if(!res) {
			spdlog::error("GameConfig:load {} failed.", filepath.string());
			return false;
		}

		auto& json = _jsonConfig.getJson();
		dying_ticks = json.value("dying_ticks", 1000);

		if(json.contains("motion"))
		{
			auto& motionJs = json["motion"];
			motion.walk = motionJs.value("tween_walk_mode", "");
			motion.swim = motionJs.value("tween_swim_mode", "");
		}

		if (json.contains("float_text")) {
			auto& floattxtJs = json["float_text"];
			float_text.font_name = floattxtJs.value("font_name", "fonts/msyh.ttf");
			float_text.font_size = floattxtJs.value("font_size", 15.0f);
			float_text.color_inc_hp.fromHexString(floattxtJs.value("color_inc_hp", "#00FF00FF"));
			float_text.color_dec_hp.fromHexString(floattxtJs.value("color_dec_hp", "#FF0000FF"));
			float_text.tween_mode = floattxtJs.value("tween_mode", "cubicIn");
			float_text.float_ticks = floattxtJs.value("float_ticks", 1000);
		}

		return true;
	}

}