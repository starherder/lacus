#include "object_factory.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include "game/ecs/comm_comp.h"
#include "game/ecs/comp_fight.h"
#include "utility/translator.h"

namespace game 
{
	bool ObjectFactory::loadSkills(GameContext& context, const fs::path& skill_cfg)
	{
		_context = &context;

		nlohmann::json json;

		std::ifstream ifile(skill_cfg.string());
		if (!ifile.is_open()) {
			spdlog::error("open file '{}' failed.", skill_cfg.string());
			return false;
		}

		try {
			ifile >> json;
			spdlog::info("load json from '{}' OK.", skill_cfg.string());
		}
		catch (const std::exception& e) {
			spdlog::error("load json form '{}' failed, err = '{}'", skill_cfg.string(), e.what());
			return false;
		}

		auto parentDir = skill_cfg.parent_path();

		for (auto& role : json)
		{
			auto id = role.value("id", "");
			auto cfg = role.value("cfg", "");

			loadSkillCfg(id, parentDir / cfg);

			_objectCfgIds.push_back(id);
		}

		return true;
	}

	bool ObjectFactory::loadObjects(GameContext& context, const fs::path& roles_cfg)
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

			_objectCfgIds.push_back(id);
		}

		return true;
	}
	
	bool ObjectFactory::loadRoleCfg(const std::string& id, const fs::path& cfgfile)
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

		_jsonObjectCfgs[id] = jsonptr;
		return true;
	}

	bool ObjectFactory::loadSkillCfg(const std::string& id, const fs::path& cfgfile)
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

		_jsonSkillCfgs[id] = jsonptr;
		return true;
	}

	entt::entity ObjectFactory::createObject(const std::string& cfgid)
	{
		if (!_context)
		{
			spdlog::error("ObjectFactory need Load first.");
			return entt::null;
		}

		auto jsonptr = _jsonObjectCfgs[cfgid];
		if (!jsonptr)
		{
			spdlog::error("object ({}) cfg NOT found.", cfgid);
			return entt::null;
		}

		auto& json = *jsonptr;
		auto object = _context->registry().create();

		auto name = Trans(json.value("name", ""));
		_context->registry().emplace<CompNameId>(object, object, name, cfgid);

		CompTransform comtrans;
		_context->registry().emplace<CompTransform>(object, comtrans);

		CompSelection comsel;
		_context->registry().emplace<CompSelection>(object, comsel);

		if (json.contains("common"))
		{
			auto& cmmJs = json["common"];

			CompComm comm;
			comm.type = getNpcType(cmmJs.value("type", ""));
			comm.desc = Trans(cmmJs.value("desc", ""));
			comm.state = LifeState::Normal;

			_context->registry().emplace<CompComm>(object, comm);
		}

		if (json.contains("selection"))
		{
			auto& selectJs = json["selection"];
			auto comsel = _context->registry().try_get<CompSelection>(object);
			if (comsel)
			{
				Color bc; bc.fromHexString(selectJs.value("border_color", "255,255,255,255"));
				comsel->border_color = bc;
				comsel->border_size = selectJs.value("border_size", 5.0f);
			}
		}

		if (json.contains("display"))
		{
			auto& display = json["display"];
			auto& trans = _context->registry().get<CompTransform>(object);
			trans.size = ToVec2(display.value("size", "64,64"));

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
			_context->registry().emplace<CompDisplay>(object, comdis);
		}

		if (json.contains("pickable"))
		{
			auto& pickable = json["pickable"];

			CompPickable com;
			com.amount = pickable.value("amount", 1);

			auto effect = pickable.value("effect", "");
			if(!effect.empty()) 
			{
				auto& comDisplay = _context->registry().get<CompDisplay>(object);
				comDisplay.particle = particle::ParticleManager::inst().CreateParticle(effect);
				if(comDisplay.particle)
				{
					comDisplay.particle->Stop();
				}
			}

			_context->registry().emplace<CompPickable>(object, com);
		}

		return object;
	}

	entt::entity ObjectFactory::createRole(const std::string& cfgid)
	{
		auto role = createObject(cfgid);
		if(role == entt::null) {
			return role;
		}

		auto jsonptr = _jsonObjectCfgs[cfgid];
		if (!jsonptr)
		{
			spdlog::error("object ({}) cfg NOT found.", cfgid);
			return entt::null;
		}

		auto& comm = _context->registry().get<CompComm>(role);

		auto& json = *jsonptr;
		if (json.contains("common"))
		{
			auto& cmmJs = json["common"];

			comm.comp = getCompSide(cmmJs.value("camp", "gangster"));

			if (json.contains("patrol"))
			{
				auto& patrolJs = json["patrol"];

				CompNpcPatrol compPatrol;
				compPatrol.patrol_radius = patrolJs.value("raduis", 0.0f);

				_context->registry().emplace<CompNpcPatrol>(role, compPatrol);
			}
		}

		if (json.contains("pick"))
		{
			auto& pickJs = json["pick"];
			CompRolePick pick;
			pick.range = pickJs.value("range", 100);
			pick.pick_types = pickJs["types"];
			_context->registry().emplace<CompRolePick>(role, pick);
		}

		if (json.contains("motion"))
		{
			auto& motionJs = json["motion"];
			
			CompMotion motion;
			motion.tween_mode = motionJs.value("tween", "");
			motion.speed = motionJs.value("speed", 0.0f);
			_context->registry().emplace<CompMotion>(role, motion);
		}

		CompUnderAttack underATK;
		if (json.contains("under_attack"))
		{
			auto& underAtkJs = json["under_attack"];
			underATK.during = underAtkJs.value("during", 200);
			underATK.motion_offset = underAtkJs.value("motion_offset", 10);

			underATK.prev_tween = underAtkJs.value("prev_tween", "");
			underATK.post_tween = underAtkJs.value("post_tween", "");
		}
		_context->registry().emplace<CompUnderAttack>(role, underATK);

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
				CompBehavior compBT;
				compBT.bevtree = _context->bevtreeMgr().createBevTree(btname);
				_context->registry().emplace<CompBehavior>(role, compBT);
			}
		}

		if(json.contains("skills")) 
		{
			CompSkills compSkills;
			auto& skillsJs = json["skills"];
			for(auto& sk : skillsJs) 
			{
				auto skillid = sk.get<std::string>();

				auto skill = createSkill(role, skillid);

				compSkills.skills.push_back(skill);
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
				//compItems.items.push_back(itname);
			}
			_context->registry().emplace<CompItems>(role, compItems);
		}
		
		return role;
	}
	

	entt::entity ObjectFactory::createSkill(entt::entity owner, const std::string& cfgid)
	{
		if (!_context)
		{
			spdlog::error("ObjectFactory need Load first.");
			return entt::null;
		}

		auto jsonptr = _jsonSkillCfgs[cfgid];
		if (!jsonptr)
		{
			spdlog::error("object ({}) cfg NOT found.", cfgid);
			return entt::null;
		}

		auto& json = *jsonptr;
		auto skill = _context->registry().create();

		auto name = Trans(json.value("name", ""));
		_context->registry().emplace<CompNameId>(skill, skill, name, cfgid);

		CompSkillComm compComm;
		compComm.owner = owner;
		compComm.state = SkillState::OK;
		compComm.type = getSkillType(json.value("type", ""));
		compComm.desc = Trans(json.value("desc", ""));
		compComm.distance = ToVec2(json.value("distance", "0,0"));
		_context->registry().emplace<CompSkillComm>(skill, compComm);

		CompSkillCD compCD;
		compCD.current_tick = 0;
		compCD.total_ticks = json.value("cd_ticks", 0);
		_context->registry().emplace<CompSkillCD>(skill, compCD);

		if (json.contains("affect"))
		{
			auto& affectJs = json["affect"];

			CompSkillAffect compAffect;
			compAffect.affect_formula = affectJs.value("affect_formula", "");
			compAffect.affect_range = affectJs.value("affect_range", 0);
			compAffect.affect_target = getSkillTarget(affectJs.value("affect_target", ""));
			compAffect.prev_ticks = affectJs.value("prev_ticks", 0);
			compAffect.post_ticks = affectJs.value("post_ticks", 0);
			compAffect.event = affectJs.value("event", "");
			_context->registry().emplace<CompSkillAffect>(skill, compAffect);
		}

		if (json.contains("tween"))
		{
			auto& tweenJs = json["tween"];

			CompSkillTween compTween;
			compTween.trans_type = getTransType(tweenJs.value("trans_type", ""));
			compTween.trans_value = tweenJs.value("trans_value", 10.0f);
			compTween.prev_tween = tweenJs.value("prev_tween", "");
			compTween.post_tween = tweenJs.value("post_tween", "");
			_context->registry().emplace<CompSkillTween>(skill, compTween);
		}

		if (json.contains("animation"))
		{
			auto& animJs = json["animation"];

			CompSkillAnimation compAnim;
			compAnim.animation = animJs.value("animation", "");
			_context->registry().emplace<CompSkillAnimation>(skill, compAnim);
		}

		if (json.contains("particle"))
		{
			auto& animJs = json["animation"];

			CompSkillParticle compParticle;
			auto parname = animJs.value("animation", "");
			compParticle.particle = particle::ParticleManager::inst().CreateParticle(parname);
			_context->registry().emplace<CompSkillParticle>(skill, compParticle);
		}

		if (json.contains("projectile"))
		{
			auto& projectJs = json["projectile"];

			CompProjectile compParticle;
			compParticle.name = projectJs.value("name", "");
			compParticle.speed = projectJs.value("speed", 10.0f);
			compParticle.tween_type = projectJs.value("tween", "linear");

			auto parname = projectJs.value("particle", "");
			compParticle.particle = particle::ParticleManager::inst().CreateParticle(parname);
			_context->registry().emplace<CompProjectile>(skill, compParticle);
		}

		spdlog::info("create skill ({}) on ({}) OK.", cfgid, (int)owner);
		return skill;
	}

	void ObjectFactory::destroyObject(entt::entity entityid)
	{
		if (!_context)
		{
			spdlog::error("ObjectFactory need Load first.");
			return ;
		}

		_context->registry().destroy(entityid);
	}

	std::optional<utility::Var> ObjectFactory::jsonToVar(const nlohmann::json& value)
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