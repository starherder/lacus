#include "game_config.h"
#include "engine/wrapper.h"
#include "nlohmann/json.hpp"

namespace game
{



	bool GameConfig::load(const fs::path& filepath)
	{
		bool res = _jsonConfig.load(filepath);
		if(!res) {
			LogError("GameConfig:load {} failed.", filepath.string());
			return false;
		}

		auto& json = _jsonConfig.getJson();
		dying_ticks = json.value("dying_ticks", 1000);

		if (json.contains("scene"))
		{
			auto& sceneJs = json["scene"];
			scenes.first_scene = sceneJs.value("first_scene", "");
		}

		if (json.contains("display"))
		{
			auto& displayJs = json["display"];
			display.chess_corner = displayJs.value("chess_corner", 5.0f);
			display.chess_dead_ground_color.fromHexString(displayJs.value("chess_dead_ground_color", "#808080FF"));
			display.chess_dead_border_color.fromHexString(displayJs.value("chess_dead_border_color", "#505050FF"));
			display.chess_dead_font_color.fromHexString(displayJs.value("chess_dead_font_color", "#000000FF"));

			display.ground_color_drag_ok.fromHexString(displayJs.value("ground_color_drag_ok", "#80FF8080"));
			display.border_color_drag_ok.fromHexString(displayJs.value("border_color_drag_ok", "#80FF80F0"));
			display.ground_color_drag_error.fromHexString(displayJs.value("ground_color_drag_error", "#FF808080"));
			display.border_color_drag_error.fromHexString(displayJs.value("border_color_drag_error", "#FF8080F0"));

			display.chess_gangster_ground_color.fromHexString(displayJs.value("chess_gangster_ground_color", "#88FF88FF"));
			display.chess_officer_ground_color.fromHexString(displayJs.value("chess_officer_ground_color", "#FF8888FF"));
			display.chess_rebel_ground_color.fromHexString(displayJs.value("chess_rebel_ground_color", "#FFFF88FF"));
			display.chess_civil_ground_color.fromHexString(displayJs.value("chess_civil_ground_color", "#888888FF"));

			display.chess_font_color.fromHexString(displayJs.value("chess_font_color", "#8888FFFF"));
			display.chess_border_color.fromHexString(displayJs.value("chess_border_color", "#555555FF"));
			display.chess_border_size = displayJs.value("chess_border_size", 2.0f);
		}

		if (json.contains("selection"))
		{
			auto& selectJs = json["selection"];
			selection.border_size = selectJs.value("border_size", 2.0f);
			selection.border_color.fromHexString(selectJs.value("border_color", "#FF0000FF"));
			selection.ground_color.fromHexString(selectJs.value("ground_color", "#FF0000FF"));
		}

		if(json.contains("motion"))
		{
			auto& motionJs = json["motion"];
			motion.walk = motionJs.value("tween_walk_mode", "");
			motion.swim = motionJs.value("tween_swim_mode", "");
		}

		if (json.contains("script"))
		{
			auto& scriptJs = json["script"];
			script.entry = scriptJs.value("entry", "lua/main.lua");
		}

		if (json.contains("float_text")) {
			auto& floattxtJs = json["float_text"];
			float_text.font_name = floattxtJs.value("font_name", "fonts/msyh.ttf");
			float_text.font_size = floattxtJs.value("font_size", 20);
			float_text.color_inc.fromHexString(floattxtJs.value("color_inc", "#00FF00FF"));
			float_text.color_dec.fromHexString(floattxtJs.value("color_dec", "#FF0000FF"));
			float_text.tween_mode = floattxtJs.value("tween_mode", "cubicIn");
			float_text.float_ticks = floattxtJs.value("float_ticks", 1000);
		}

		if (json.contains("pickup"))
		{
			auto& pickupJs = json["pickup"];
			pickup.pick_range = pickupJs.value("pickup", 30);
			pickup.pick_period = pickupJs.value("period", 1000);
			pickup.dead_drop_range = pickupJs.value("dead_drop_range", 130);
		}

		return true;
	}

}