#include "role_factory.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include "game/ecs/comm_comp.h"
#include "utility/translator.h"

namespace game 
{

	bool RoleFactory::load(GameContext& context, const fs::path& roles_cfg)
	{
		_context = &context;

		nlohmann::json json;

		std::ifstream ifile(roles_cfg.string());
		if (!ifile.is_open()) {
			spdlog::error("open file '{}' failed.", roles_cfg.string());
			return false;
		}

		try {
			ifile >> json;
			spdlog::info("load json from '{}' OK.", roles_cfg.string());
		}
		catch (const std::exception& e) {
			spdlog::error("load json form '{}' failed, err = '{}'", roles_cfg.string(), e.what());
			return false;
		}

		auto parentDir = roles_cfg.parent_path();

		for(auto& role : json) 
		{
			auto id = role.value("id", "");
			auto cfg = role.value("cfg", "");

			loadRoleCfg(id, parentDir/cfg);
		}

		return true;
	}
	
	bool RoleFactory::loadRoleCfg(const std::string& id, const fs::path& cfgfile)
	{
		auto jsonptr = std::make_shared<nlohmann::json>();

		std::ifstream ifile(cfgfile.string());
		if (!ifile.is_open()) {
			spdlog::error("open file '{}' failed.", cfgfile.string());
			return false;
		}

		try {
			ifile >> *jsonptr;
			spdlog::info("load json from '{}' OK.", cfgfile.string());
		}
		catch (const std::exception& e) {
			spdlog::error("load json form '{}' failed, err = '{}'", cfgfile.string(), e.what());
			return false;
		}

		_jsonCfgs[id] = jsonptr;
		return true;
	}

	entt::entity RoleFactory::createRole(const std::string& cfgid)
	{
		if (!_context) 
		{
			spdlog::error("roleFactory need Load first.");
			return entt::null;
		}

		auto jsonptr = _jsonCfgs[cfgid];
		if(!jsonptr)
		{
			spdlog::error("role ({}) cfg NOT found.", cfgid);
			return entt::null;
		}

		auto& json = *jsonptr;
		auto role = _context->registry().create();

		auto name = Trans(json.value("name", ""));

		_context->registry().emplace<CompNameId>(role, role, name, cfgid);

		if (json.contains("common"))
		{
			auto& cmmJs = json["common"];

			CompComm comm;
			comm.type = cmmJs.value("type", "");
			comm.desc = cmmJs.value("desc", "");

			auto campstr = cmmJs.value("camp", "gangster");
			if (campstr == "official") {
				comm.comp = CompComm::CampSide::Officer;
			}
			else if (campstr == "foreign") { 
				comm.comp = CompComm::CampSide::Foreign; 
			}
			else if (campstr == "rebel") {
				comm.comp = CompComm::CampSide::Rebel;
			}
			else if (campstr == "gangster") {
				comm.comp = CompComm::CampSide::Gangster;
			}
			else {
				comm.comp = CompComm::CampSide::Gangster;
				spdlog::error("camp ({}) NOT support", campstr);
			}

			_context->registry().emplace<CompComm>(role, comm);

			if (comm.type == "patrol_npc")
			{
				if (json.contains("patrol"))
				{
					auto& patrolJs = json["patrol"];

					CompNpcPatrol compPatrol;
					compPatrol.patrol_radius = patrolJs.value("raduis", 0.0f);

					_context->registry().emplace<CompNpcPatrol>(role, compPatrol);
				}
			}
		}

		if (json.contains("motion"))
		{
			auto& motionJs = json["motion"];
			
			CompMotion motion;
			motion.tween_mode = motionJs.value("tween", "");
			motion.speed = motionJs.value("speed", 0.0f);
			_context->registry().emplace<CompMotion>(role, motion);
		}

		if(json.contains("display")) 
		{
			auto& display = json["display"];

			CompTransform comtrans;
			comtrans.size = ToVec2(display.value("size", "64,64"));
			_context->registry().emplace<CompTransform>(role, comtrans);

			Color bg; bg.fromHexString(display.value("groud_color", "0,0,0,0"));
			Color bd; bd.fromHexString(display.value("border_color", "0,0,0,0"));
			Color fc; fc.fromHexString(display.value("font_color", "0,0,0,0"));

			auto texture = display.value("texture", "");
			auto tex_rect = ToRect(display.value("tex_rect", "0,0,0,0"));
			auto font_file = display.value("font_file", "fonts/msyh.ttf");
			auto font_size = display.value("font_size", 12);

			CompDisplay comdis;
			comdis.ground_color = bg;
			comdis.border_color = bd;
			comdis.font_color = fc;
			comdis.texture = _context->textureMgr().get(HashString(texture.c_str()));
			comdis.tex_rect = tex_rect;
			comdis.font = _context->fontMgr().get(HashString(font_file.c_str()), font_size);
			_context->registry().emplace<CompDisplay>(role, comdis);
		}

		if (json.contains("properties")) 
		{
			utility::DynamicStruct<std::string> ds;
			auto& props = json["properties"];
			for(auto& [k, v] : props.items())
			{
				auto varopt = jsonToVar(v);
				if(varopt) 
				{
					ds.insert({k, varopt.value()});
				}
			}

			CompPresets preset;
			preset.properties.swap(std::move(ds));
			_context->registry().emplace<CompPresets>(role, preset);
		}

		if (json.contains("behavior"))
		{
			auto& behavior = json["behavior"];
			auto btname = behavior.value("bevtree", "");
			if (!btname.empty()) 
			{
				CompBevtree compBT;
				compBT.bevtree = _context->bevtreeMgr().createBevTree(btname);
				_context->registry().emplace<CompBevtree>(role, compBT);
			}
		}

		if(json.contains("skills")) 
		{
			CompSkills compSkills;
			auto& skillsJs = json["skills"];
			for(auto& sk : skillsJs) 
			{
				auto skname = sk.get<std::string>();
				compSkills.skills.push_back(skname);
			}
			_context->registry().emplace<CompSkills>(role, compSkills);
		}

		if (json.contains("items"))
		{
			CompItems compItems;
			auto& itemsJs = json["items"];
			for (auto& it : itemsJs)
			{
				auto itname = it.get<std::string>();
				compItems.items.push_back(itname);
			}
			_context->registry().emplace<CompItems>(role, compItems);
		}
		
		return role;
	}
	
	void RoleFactory::destroyRole(entt::entity entityid)
	{
		if (!_context)
		{
			spdlog::error("roleFactory need Load first.");
			return ;
		}

		_context->registry().destroy(entityid);
	}

	std::optional<utility::Var> RoleFactory::jsonToVar(const nlohmann::json& value)
	{
		try {
			if (value.is_null()) {
				return std::nullopt;
			}
			else if (value.is_boolean()) {
				return value.get<bool>();
			}
			else if (value.is_number_integer()) {
				return value.get<int>();
			}
			else if (value.is_number_float()) {
				return value.get<double>();
			}
			else if (value.is_string()) {
				return value.get<std::string>();
			}
			else if (value.is_array()) {
				//auto arr = value.get<std::vector<int>>();
				spdlog::error("json value is array.");
			}
			else if (value.is_object()) {
				//std::cout << "Object with " << value.size() << " fields" << std::endl;
				spdlog::error("json value is object.");
			}
			else {
				spdlog::error("json type unkonw: tpye = {}", value.type_name());
			}
		} catch (const std::exception& e) {
			spdlog::error("type convert failed. e = {}", e.what());
		}

		return std::nullopt;
	}

}