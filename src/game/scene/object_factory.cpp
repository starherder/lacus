#include "object_factory.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include "game/ecs/comm_comp.h"
#include "game/ecs/comp_fight.h"
#include "game/ecs/comm_event.h"
#include "game/scene/game_scene.h"

#include "utility/translator.h"
#include "magic_enum/magic_enum.h"

namespace game 
{

	bool ObjectFactory::loadBuffs(GameContext& context, const fs::path& buffdir)
	{
		_context = &context;

		for (const auto& entry : std::filesystem::directory_iterator(buffdir))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path();
				loadBuffCfg(filename);
			}
		}

		return true;
	}

	bool ObjectFactory::loadSkills(GameContext& context, const fs::path& skilldir)
	{
		_context = &context;

		for (const auto& entry : std::filesystem::directory_iterator(skilldir))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path();
				loadSkillCfg(filename);
			}
		}

		return true;
	}

	bool ObjectFactory::loadObjects(GameContext& context, const fs::path& cfgdir)
	{
		_context = &context;


		for (const auto& entry : std::filesystem::directory_iterator(cfgdir))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path();
				loadRoleCfg(filename);
			}
		}

		return true;
	}
	
	bool ObjectFactory::loadRoleCfg(const fs::path& cfgfile)
	{
		auto jsonptr = std::make_shared<nlohmann::json>();

		std::ifstream ifile(cfgfile.string());
		if (!ifile.is_open()) {
			SPDLOG_ERROR("open file '{}' failed.", cfgfile.string());
			return false;
		}

		try {
			ifile >> *jsonptr;
			SPDLOG_INFO("load json from '{}' OK.", cfgfile.string());
		}
		catch (const std::exception& e) {
			SPDLOG_ERROR("load json form '{}' failed, err = '{}'", cfgfile.string(), e.what());
			return false;
		}

		std::string cfgid = jsonptr->value("id", "");
		if (cfgid.empty())
		{
			SPDLOG_ERROR("role file({}) NOT found cfgid.", cfgfile.string());
			return false;
		}

		_objectCfgIds.push_back(cfgid);

		_jsonObjectCfgs[cfgid] = jsonptr;
		return true;
	}

	bool ObjectFactory::loadSkillCfg(const fs::path& cfgfile)
	{
		auto jsonptr = std::make_shared<nlohmann::json>();

		std::ifstream ifile(cfgfile.string());
		if (!ifile.is_open()) {
			SPDLOG_ERROR("open file '{}' failed.", cfgfile.string());
			return false;
		}

		try {
			ifile >> *jsonptr;
			SPDLOG_INFO("load json from '{}' OK.", cfgfile.string());
		}
		catch (const std::exception& e) {
			SPDLOG_ERROR("load json form '{}' failed, err = '{}'", cfgfile.string(), e.what());
			return false;
		}

		std::string cfgid = jsonptr->value("cfgid", "");
		if(cfgid.empty())
		{
			SPDLOG_ERROR("skill file({}) NOT found cfgid.", cfgfile.string());
			return false;
		}

		_skillCfgIds.push_back(cfgid);

		_jsonSkillCfgs[cfgid] = jsonptr;
		return true;
	}

	bool ObjectFactory::loadBuffCfg(const fs::path& cfgfile)
	{
		auto jsonptr = std::make_shared<nlohmann::json>();

		std::ifstream ifile(cfgfile.string());
		if (!ifile.is_open()) {
			SPDLOG_ERROR("open file '{}' failed.", cfgfile.string());
			return false;
		}

		try {
			ifile >> *jsonptr;
			SPDLOG_INFO("load json from '{}' OK.", cfgfile.string());
		}
		catch (const std::exception& e) {
			SPDLOG_ERROR("load json form '{}' failed, err = '{}'", cfgfile.string(), e.what());
			return false;
		}

		std::string cfgid = jsonptr->value("cfgid", "");
		if (cfgid.empty())
		{
			SPDLOG_ERROR("buff file({}) NOT found cfgid.", cfgfile.string());
			return false;
		}

		_buffCfgIds.push_back(cfgid);

		_jsonBuffCfgs[cfgid] = jsonptr;
		return true;
	}


	entt::entity ObjectFactory::createActor(const std::string& cfgid)
	{
		if (!_context)
		{
			SPDLOG_ERROR("ObjectFactory need Load first.");
			return entt::null;
		}

		auto jsonptr = _jsonObjectCfgs[cfgid];
		if (!jsonptr)
		{
			SPDLOG_ERROR("object ({}) cfg NOT found.", cfgid);
			return entt::null;
		}

		auto& json = *jsonptr;

		auto type = getNpcType(json.value("type", ""));
		switch(type)
		{
			case ObjectType::Item:
			{
				return createObject(json);
			}break;
			case ObjectType::Npc:
			{
				return createRole(json);
			
			}break;
			case ObjectType::Spawner:
			{
				return createSpawner(json);
			}break;
			default:
			break;
		}

		return entt::null;
	}

	entt::entity ObjectFactory::createObject(const nJson& json)
	{
		auto object = _context->registry().create();

		std::string cfgid = json.value("id", "");
		std::string name = Trans(json.value("name", ""));
		_context->registry().emplace<CompNameId>(object, object, name, cfgid);

		CompComm comm;
		comm.type = getNpcType(json.value("type", ""));
		comm.desc = Trans(json.value("desc", ""));
		comm.side = getCampSide(json.value("side", ""));
		_context->registry().emplace<CompComm>(object, comm);

		CompTransform comtrans;
		_context->registry().emplace<CompTransform>(object, comtrans);

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
			com.effect = pickable.value("effect", "");
			com.funcs = pickable.value("func", "");
			com.pick_use = pickable.value("pick_use", false);
			com.use_ticks = pickable.value("use_ticks", 1000);
			_context->registry().emplace<CompPickable>(object, com);
		}

		//SPDLOG_INFO("create object: cfg = ({}), object = ({})", cfgid, (uint32_t)object);
		
		return object;
	}


	entt::entity ObjectFactory::createSpawner(const nJson& json)
	{
		auto object = createObject(json);
		if (_context->registry().valid(object) == false) 
		{
			return entt::null;
		}

		if (!json.contains("spawner"))
		{
			return entt::null;
		}

		auto& spawnerJs = json["spawner"];

		CompSpawner spawner;
		spawner.side = getCampSide(spawnerJs.value("side", ""));
		spawner.radius = spawnerJs.value("radius", 100.0f);
		spawner.interval = spawnerJs.value("interval", 1000);
		spawner.npc = spawnerJs.value("npc", "");
		spawner.min_count = spawnerJs.value("min_count", 1);
		spawner.max_count = spawnerJs.value("max_count", 10);
		_context->registry().emplace<CompSpawner>(object, spawner);

		return object;
	}

	entt::entity ObjectFactory::createRole(const nJson& json)
	{
		auto role = createObject(json);
		if(role == entt::null) {
			return role;
		}

		if (json.contains("patrol"))
		{
			auto& patrolJs = json["patrol"];

			CompNpcPatrol compPatrol;
			compPatrol.patrol_radius = patrolJs.value("raduis", 0.0f);

			_context->registry().emplace<CompNpcPatrol>(role, compPatrol);
		}

		if (json.contains("pick"))
		{
			auto& pickJs = json["pick"];
			CompRolePick pick;
			pick.range = pickJs.value("range", 100.0f);
			pick.pick_types = pickJs["types"];
			_context->registry().emplace<CompRolePick>(role, pick);
		}

		if (json.contains("motion"))
		{
			auto& motionJs = json["motion"];
			
			CompMotion motion;
			motion.speed = motionJs.value("speed", 0.0f);
			_context->registry().emplace<CompMotion>(role, motion);
		}

		CompUnderAttack underATK;
		if (json.contains("under_attack"))
		{
			auto& underAtkJs = json["under_attack"];
			underATK.during = underAtkJs.value("during", 200);
			underATK.motion_offset = underAtkJs.value("motion_offset", 10.0f);

			underATK.prev_tween = underAtkJs.value("prev_tween", "");
			underATK.post_tween = underAtkJs.value("post_tween", "");
		}
		_context->registry().emplace<CompUnderAttack>(role, underATK);

		if (json.contains("base_prop")) 
		{
			auto& baseJs = json["base_prop"];
			float roll = baseJs.value("roll", 0.0f);

			CompBaseProp compBase;
			compBase.lv = baseJs.value("level", 0);
			compBase.str = baseJs.value("str", 0.0f) + utility::Random_Minus1_1() * roll;
			compBase.cst = baseJs.value("cst", 0.0f) + utility::Random_Minus1_1() * roll;
			compBase.dex = baseJs.value("dex", 0.0f) + utility::Random_Minus1_1() * roll;
			compBase.met = baseJs.value("met", 0.0f) + utility::Random_Minus1_1() * roll;
			compBase.exp = 0;

			_context->registry().emplace<CompBaseProp>(role, compBase);
			_context->registry().emplace<CompFightProp>(role, CompFightProp{});

			_context->dispatcher().trigger(EvtRoleLevelAlter{role, 1});
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

		if (json.contains("skills"))
		{
			CompSkills compSkills;
			auto& skillsJs = json["skills"];
			for (auto& sk : skillsJs)
			{
				auto skillid = sk.get<std::string>();
				auto skill = createSkill(role, skillid);

				compSkills.skills.push_back(skill);
			}
			_context->registry().emplace<CompSkills>(role, compSkills);
		}

		if (json.contains("buffs"))
		{
			CompBuffs compBuffs;
			auto& skillsJs = json["buffs"];
			for (auto& sk : skillsJs)
			{
				auto buffid = sk.get<std::string>();
				auto buff = createBuff(role, buffid);

				compBuffs.buffs.push_back(buff);
			}
			_context->registry().emplace<CompBuffs>(role, compBuffs);
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

	entt::entity ObjectFactory::createBuff(entt::entity owner, const std::string& cfgid)
	{
		if (!_context)
		{
			SPDLOG_ERROR("ObjectFactory need Load first.");
			return entt::null;
		}

		auto jsonptr = _jsonBuffCfgs[cfgid];
		if (!jsonptr)
		{
			SPDLOG_ERROR("object ({}) cfg NOT found.", cfgid);
			return entt::null;
		}

		auto& json = *jsonptr;
		auto buff = _context->registry().create();

		auto name = Trans(json.value("name", ""));
		_context->registry().emplace<CompNameId>(buff, buff, name, cfgid);
		_context->registry().emplace<CompTransform>(buff, CompTransform{});
		_context->registry().emplace<CompBuffComm>(buff, CompBuffComm{});

		auto& compBuff = _context->registry().get<CompBuffComm>(buff);
		compBuff.owner = owner;
		compBuff.cfgid = json.value("cfgid", "");
		compBuff.duration = json.value("duration", 0);
		compBuff.func = json.value("func", "");
		compBuff.period = json.value("period", 0);

		if (json.contains("display"))
		{
			auto& display = json["display"];

			auto& trans = _context->registry().get<CompTransform>(buff);
			trans.size = ToVec2(display.value("size", "16,16"));

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
			_context->registry().emplace<CompDisplay>(buff, comdis);
		}

		return buff;
	}

	entt::entity ObjectFactory::createSkill(entt::entity owner, const std::string& cfgid)
	{
		if (!_context)
		{
			SPDLOG_ERROR("ObjectFactory need Load first.");
			return entt::null;
		}

		auto jsonptr = _jsonSkillCfgs[cfgid];
		if (!jsonptr)
		{
			SPDLOG_ERROR("object ({}) cfg NOT found.", cfgid);
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
		compComm.distance = json.value("distance",0.0f);
		_context->registry().emplace<CompSkillComm>(skill, compComm);

		CompSkillCD compCD;
		compCD.current_tick = 0;
		compCD.total_ticks = json.value("cd_ticks", 0);
		_context->registry().emplace<CompSkillCD>(skill, compCD);

		if(json.contains("sound"))
		{
			CompAudio compAudio;
			compAudio.audio_name = json.value("sound", "");
			_context->registry().emplace<CompAudio>(skill, compAudio);
		}

		if (json.contains("affect"))
		{
			auto& affectJs = json["affect"];

			CompSkillAffect compAffect;
			compAffect.func = affectJs.value("func", "");
			compAffect.range = affectJs.value("range", 0.0f);
			compAffect.target = getSkillTarget(affectJs.value("target", ""));
			compAffect.prev_ticks = affectJs.value("prev_ticks", 0);
			compAffect.post_ticks = affectJs.value("post_ticks", 0);
			compAffect.event = affectJs.value("event", "");
			_context->registry().emplace<CompSkillAffect>(skill, compAffect);
		}

		if (json.contains("tween"))
		{
			auto& tweenJs = json["tween"];

			CompSkillSpell compTween;
			compTween.trans_type = getTransType(tweenJs.value("trans_type", ""));
			compTween.trans_value = tweenJs.value("trans_value", 10.0f);
			compTween.prev_tween = tweenJs.value("prev_tween", "");
			compTween.post_tween = tweenJs.value("post_tween", "");
			_context->registry().emplace<CompSkillSpell>(skill, compTween);
		}

		if (json.contains("projectile"))
		{
			auto& projectJs = json["projectile"];

			CompProjectileCfg compParticle;
			compParticle.name = projectJs.value("name", "");
			compParticle.speed = projectJs.value("speed", 10.0f);
			compParticle.tween = projectJs.value("tween", "linear");
			compParticle.particle = projectJs.value("particle", "");

			_context->registry().emplace<CompProjectileCfg>(skill, compParticle);
		}

		if (json.contains("sprint"))
		{
			auto& sprintJs = json["sprint"];
			
			CompSprint sprint;
			sprint.dis = sprintJs.value("dis", 100.0f);
			sprint.speed = sprintJs.value("speed", 100.0f);
			sprint.particle = sprintJs.value("particle", "");
			sprint.tween_mode = sprintJs.value("tween_mode", "linear");

			_context->registry().emplace<CompSprint>(skill, sprint);
		}

		if (json.contains("trap"))
		{
			auto& trapsJs = json["trap"];

			CompTrapCfg trap;
			trap.range = trapsJs.value("range", 32.0f);
			trap.duration = trapsJs.value("duration", 1000);
			trap.period = trapsJs.value("period", 500);
			trap.func = trapsJs.value("func", "");
			trap.particle = trapsJs.value("particle", "");
			trap.color = Color::parseHexString(trapsJs.value("color", "#FF0000FF"));
			trap.texture = trapsJs.value("texture", "");

			_context->registry().emplace<CompTrapCfg>(skill, trap);
		}

		if (json.contains("wave"))
		{
			auto& waveJs = json["wave"];

			CompTrapCfg trap;
			trap.range = _context->scene().tileSize().x;
			trap.duration = waveJs.value("duration", 1000);
			trap.period = trap.duration;
			trap.func = waveJs.value("func", "");
			trap.particle = waveJs.value("particle", "");
			trap.color = Color::parseHexString(waveJs.value("color", "#FF0000FF"));
			trap.texture = waveJs.value("texture", "");
			_context->registry().emplace<CompTrapCfg>(skill, trap);

			CompWaveCfg wave;
			wave.count = waveJs.value("count", 1);
			wave.range = waveJs.value("range", 1);
			wave.interval = waveJs.value("interval", 500);
			_context->registry().emplace<CompWaveCfg>(skill, wave);
		}

		return skill;
	}

	particle::ParticlePtr ObjectFactory::createParticleOnObject(entt::entity owner, const std::string& particle)
	{
		if (!_context || _context->registry().valid(owner) == false)
		{
			SPDLOG_ERROR("create particle ({}) on invald object({})", particle, (uint32_t)owner);
			return nullptr;
		}

		auto particlePtr = particle::ParticleManager::inst().CreateParticle(particle);
		if (particlePtr)
		{
			CompBindParticle compParticle;
			compParticle.particle = particlePtr;
			compParticle.particle->Start();
			_context->registry().emplace<CompBindParticle>(owner, compParticle);

			return particlePtr;
		}

		return nullptr;
	}

	entt::entity ObjectFactory::createProjectile(const Vec2& source, const Vec2& target, const std::string& particle)
	{
		auto bullet = _context->registry().create();

		CompNameId compName;
		compName.cfg_id = "";
		compName.name = fmt::format("projectile_{}", (uint32_t)bullet);
		compName.id = bullet;
		_context->registry().emplace<CompNameId>(bullet, compName);

		CompTransform compTrans;
		_context->registry().emplace<CompTransform>(bullet, compTrans);

		createParticleOnObject(bullet, particle);
	
		return bullet;
	}

	entt::entity ObjectFactory::createTrap(const Vec2& target, float range, const Color& color, 
										const std::string& texture, const std::string& particle, ShapeType shape_type)
	{
		auto trap = _context->registry().create();

		CompNameId compName;
		compName.cfg_id = "";
		compName.name = fmt::format("projectile_{}", trap);
		compName.id = trap;
		_context->registry().emplace<CompNameId>(trap, compName);

		CompTransform compTrans;
		compTrans.position = target;
		compTrans.size = Vec2{ range, range };
		_context->registry().emplace<CompTransform>(trap, compTrans);

		CompMarkDisplay compDisplay;
		compDisplay.shape_type = shape_type;
		compDisplay.ground_color = color;
		compDisplay.border_color = color;
		compDisplay.texture = _context->textureMgr().get(texture);
		_context->registry().emplace<CompMarkDisplay>(trap, compDisplay);

		CompTraps compTraps;
		compTraps.during_ticks = 0;
		compTraps.period_ticks = 0;
		compTraps.running = false;
		_context->registry().emplace<CompTraps>(trap, compTraps);

		createParticleOnObject(trap, particle);

		return trap;
	}

	void ObjectFactory::createSkyEffect(SkyEffect effect,  Color color, int last, int fadein, int fadeout)
	{
		auto sky = _context->registry().create();
		CompTransform trans;
		trans.position = { 0, 0 };
		_context->registry().emplace<CompTransform>(sky, trans);

		CompSkyEffect compSky;
		compSky.color = color;
		compSky.effect = effect;
		compSky.tween = tweeny::from(0)
			.to(color.a).via("linear").during(fadein)
			.to(color.a).via("linear").during(last)
			.to(0).via("linear").during(fadeout)
			.onStep([sky, effect, this](auto& t, int a) {
				if (!_context->registry().valid(sky)) {
					return true;
				}

				if (t.isFinished()) {
					SPDLOG_INFO("sky effect({}) finish", magic_enum::enum_name(effect).data());
					_context->registry().emplace<CompDestroy>(sky);
					return true;
				}

				SPDLOG_INFO("sky.color.a = {}", a);

				auto& compSky = _context->registry().get<CompSkyEffect>(sky);
				compSky.color.a = a;
				return false;
			});

		_context->registry().emplace<CompSkyEffect>(sky, compSky);
	}

	void ObjectFactory::destroyObject(entt::entity entityid)
	{
		if (!_context)
		{
			SPDLOG_ERROR("ObjectFactory need Load first.");
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
				SPDLOG_ERROR("json value is array.");
			}
			else if (value.is_object()) {
				//std::cout << "Object with " << value.size() << " fields" << std::endl;
				SPDLOG_ERROR("json value is object.");
			}
			else {
				SPDLOG_ERROR("json type unkonw: tpye = {}", value.type_name());
			}
		} catch (const std::exception& e) {
			SPDLOG_ERROR("type convert failed. e = {}", e.what());
		}

		return std::nullopt;
	}

	const Properties& ObjectFactory::getObjectCfgProperties(const std::string& cfgid)
	{
		static Properties props;
		props.clear();

		auto jsonptr = _jsonObjectCfgs[cfgid];
		if (!jsonptr)
		{
			SPDLOG_ERROR("getObjectCfgProperties: object ({}) cfg NOT found.", cfgid);
			return props;
		}

		auto& json = *jsonptr;

		props["cfgid"] = json.value("id", "");
		props["name"] = Trans(json.value("name", ""));
		props["desc"] = Trans(json.value("desc", ""));
		props["type"] = (int)getNpcType(json.value("type", ""));
		props["side"] = (int)getCampSide(json.value("side", ""));

		return props;
	}
}