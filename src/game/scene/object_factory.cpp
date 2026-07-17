#include "object_factory.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include "game/ecs/comm_comp.h"
#include "game/ecs/comp_fight.h"
#include "game/ecs/comm_event.h"
#include "game/scene/game_scene.h"
#include "game/game_config.h"

#include "utility/translator.h"
#include "magic_enum/magic_enum.h"

namespace game 
{

	void ObjectFactory::init(GameContext* context)
	{
		_context = context;

		_roleCfgIds.clear();
		_enemyCfgIds.clear();
		_otherCfgIds.clear();
		_skillCfgIds.clear();
		_buffCfgIds.clear();

		_jsonObjectCfgs.clear();
		_jsonSkillCfgs.clear();
		_jsonBuffCfgs.clear();
	}

	void ObjectFactory::reloadAll()
	{
		init(_context);

		loadBuffs(_buffPath);

		loadSkills(_skillPath);

		loadRoles(_rolePath);

		loadEnemies(_enemyPath);

		loadOther(_otherPath);

		loadItems(_itemPath);
	}

	bool ObjectFactory::loadBuffs(const fs::path& buffdir)
	{
		assert(_context);

		_buffPath = buffdir;

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

	bool ObjectFactory::loadSkills(const fs::path& skilldir)
	{
		assert(_context);

		_skillPath = skilldir;

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

	bool ObjectFactory::loadEnemies(const fs::path& cfgdir)
	{
		assert(_context);

		_enemyPath = cfgdir;

		for (const auto& entry : std::filesystem::directory_iterator(cfgdir))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path();
				auto cfgid = loadObjectCfg(filename);
				if (!cfgid.empty()) 
				{
					_enemyCfgIds.push_back(cfgid);
				}
			}
		}

		return true;
	}

	bool ObjectFactory::loadOther(const fs::path& cfgdir)
	{
		assert(_context);

		_otherPath = cfgdir;

		for (const auto& entry : std::filesystem::directory_iterator(cfgdir))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path();
				auto cfgid = loadObjectCfg(filename);
				if (!cfgid.empty())
				{
					_otherCfgIds.push_back(cfgid);
				}
			}
		}

		return true;
	}
	bool ObjectFactory::loadRoles(const fs::path& cfgdir)
	{
		assert(_context);

		_rolePath = cfgdir;

		for (const auto& entry : std::filesystem::directory_iterator(cfgdir))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path();
				auto cfgid = loadObjectCfg(filename);
				if (!cfgid.empty())
				{
					_roleCfgIds.push_back(cfgid);
				}
			}
		}

		return true;
	}
	bool ObjectFactory::loadItems(const fs::path& cfgdir)
	{
		assert(_context);

		_itemPath = cfgdir;

		for (const auto& entry : std::filesystem::directory_iterator(cfgdir))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path();
				loadObjectCfg(filename);
			}
		}

		return true;
	}
	
	std::string ObjectFactory::loadObjectCfg(const fs::path& cfgfile)
	{
		assert(_context);

		auto jsonptr = std::make_shared<nlohmann::json>();

		std::ifstream ifile(cfgfile.string());
		if (!ifile.is_open()) {
			LogError("open file '{}' failed.", cfgfile.string());
			return "";
		}

		try {
			ifile >> *jsonptr;
			LogInfo("load json from '{}' OK.", cfgfile.string());
		}
		catch (const std::exception& e) {
			LogError("load json form '{}' failed, err = '{}'", cfgfile.string(), e.what());
			return "";
		}

		std::string cfgid = jsonptr->value("id", "");
		if (cfgid.empty())
		{
			LogError("role file({}) NOT found cfgid.", cfgfile.string());
			return "";
		}

		_objectCfgIds.push_back(cfgid);
		_jsonObjectCfgs[cfgid] = jsonptr;
		return cfgid;
	}

	std::string ObjectFactory::loadSkillCfg(const fs::path& cfgfile)
	{
		assert(_context);
		auto jsonptr = std::make_shared<nlohmann::json>();

		std::ifstream ifile(cfgfile.string());
		if (!ifile.is_open()) {
			LogError("open file '{}' failed.", cfgfile.string());
			return "";
		}

		try {
			ifile >> *jsonptr;
			LogInfo("load json from '{}' OK.", cfgfile.string());
		}
		catch (const std::exception& e) {
			LogError("load json form '{}' failed, err = '{}'", cfgfile.string(), e.what());
			return "";
		}

		std::string cfgid = jsonptr->value("cfgid", "");
		if(cfgid.empty())
		{
			LogError("skill file({}) NOT found cfgid.", cfgfile.string());
			return "";
		}

		_skillCfgIds.push_back(cfgid);

		_jsonSkillCfgs[cfgid] = jsonptr;
		return cfgid;
	}

	std::string ObjectFactory::loadBuffCfg(const fs::path& cfgfile)
	{
		assert(_context);

		auto jsonptr = std::make_shared<nlohmann::json>();

		std::ifstream ifile(cfgfile.string());
		if (!ifile.is_open()) {
			LogError("open file '{}' failed.", cfgfile.string());
			return "";
		}

		try {
			ifile >> *jsonptr;
			LogInfo("load json from '{}' OK.", cfgfile.string());
		}
		catch (const std::exception& e) {
			LogError("load json form '{}' failed, err = '{}'", cfgfile.string(), e.what());
			return "";
		}

		std::string cfgid = jsonptr->value("cfgid", "");
		if (cfgid.empty())
		{
			LogError("buff file({}) NOT found cfgid.", cfgfile.string());
			return "";
		}

		_buffCfgIds.push_back(cfgid);

		_jsonBuffCfgs[cfgid] = jsonptr;
		return cfgid;
	}

	entt::entity ObjectFactory::createObject(const std::string& cfgid)
	{
		assert(_context);

		auto jsonptr = _jsonObjectCfgs[cfgid];
		if (!jsonptr)
		{
			LogError("object ({}) cfg NOT found.", cfgid);
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
		assert(_context);

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

			//Color bg; bg.fromHexString(display.value("groud_color", "0,0,0,0"));
			//Color bd; bd.fromHexString(display.value("border_color", "0,0,0,0"));
			//Color fc; fc.fromHexString(display.value("font_color", "0,0,0,0"));

			auto texture = display.value("texture", "");
			auto bg_texture = display.value("ground_texture", "");

			auto font_file = display.value("font_file", "fonts/msyh.ttf");
			auto font_size = display.value("font_size", 12);

			CompDisplay comdis;
			comdis.visible = true;
			//comdis.ground_color = bg;
			//comdis.border_color = bd;
			//comdis.font_color = fc;
			comdis.texture = _context->textureMgr().getCfgTexTile(texture.c_str());
			comdis.ground_texture = _context->textureMgr().getCfgTexTile(bg_texture.c_str());
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

		//LogInfo("create object: cfg = ({}), object = ({})", cfgid, (uint32_t)object);
		
		return object;
	}

	entt::entity ObjectFactory::createSpawner(const nJson& json)
	{
		assert(_context);

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
		if(role == entt::null) 
		{
			return role;
		}

		if (json.contains("role_event"))
		{
			auto& roleEventJs = json["role_event"];
			CompRoleEvent roleEvent;
			roleEvent.dead = roleEventJs.value("dead", "");
			roleEvent.born = roleEventJs.value("born", "");
			_context->registry().emplace<CompRoleEvent>(role, roleEvent);
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
			
			CompMoveCfg move;
			move.speed = motionJs.value("speed", 0.0f);
			move.swim_speed = motionJs.value("swim_speed", 0.0f);
			_context->registry().emplace<CompMoveCfg>(role, move);
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
			compBase.str = baseJs.value("str", 0.0f) + utility::rand_minus1_1() * roll;
			compBase.cst = baseJs.value("cst", 0.0f) + utility::rand_minus1_1() * roll;
			compBase.dex = baseJs.value("dex", 0.0f) + utility::rand_minus1_1() * roll;
			compBase.met = baseJs.value("met", 0.0f) + utility::rand_minus1_1() * roll;
			compBase.exp = 0;

			_context->registry().emplace<CompBaseProp>(role, compBase);
			_context->registry().emplace<CompFightProp>(role, CompFightProp{});

			_context->dispatcher().trigger(EvtRoleLevelAlter{role, compBase.lv-1});
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
				auto item_name = it.get<std::string>();
				auto item = createObject(item_name);
				auto pDisplay = _context->registry().try_get<CompDisplay>(item);
				if(pDisplay) {
					pDisplay->visible = false;
				}

				compItems.items.push_back(item);
			}
			_context->registry().emplace<CompItems>(role, compItems);
		}

		return role;
	}

	entt::entity ObjectFactory::createBuff(entt::entity owner, const std::string& cfgid)
	{
		if (!_context)
		{
			LogError("ObjectFactory need Load first.");
			return entt::null;
		}

		auto jsonptr = _jsonBuffCfgs[cfgid];
		if (!jsonptr)
		{
			LogError("object ({}) cfg NOT found.", cfgid);
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

			//Color bg; bg.fromHexString(display.value("groud_color", "0,0,0,0"));
			//Color bd; bd.fromHexString(display.value("border_color", "0,0,0,0"));
			//Color fc; fc.fromHexString(display.value("font_color", "0,0,0,0"));

			auto texture = display.value("texture", "");
			auto bg_texture = display.value("bg_texture", "");

			auto font_file = display.value("font_file", "fonts/msyh.ttf");
			auto font_size = display.value("font_size", 12);

			CompDisplay comdis;
			comdis.visible = true;
			//comdis.ground_color = bg;
			//comdis.border_color = bd;
			//comdis.font_color = fc;
			comdis.texture = _context->textureMgr().getCfgTexTile(texture.c_str());
			comdis.ground_texture = _context->textureMgr().getCfgTexTile(bg_texture.c_str());

			comdis.font = _context->fontMgr().get(HashString(font_file.c_str()), font_size);
			_context->registry().emplace<CompDisplay>(buff, comdis);
		}

		return buff;
	}

	entt::entity ObjectFactory::createSkill(entt::entity owner, const std::string& cfgid)
	{
		assert(_context);

		auto jsonptr = _jsonSkillCfgs[cfgid];
		if (!jsonptr)
		{
			LogError("object ({}) cfg NOT found.", cfgid);
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

			CompProjectileCfg compProjectile;
			compProjectile.name = projectJs.value("name", "");
			compProjectile.speed = projectJs.value("speed", 10.0f);
			compProjectile.tween = projectJs.value("tween", "linear");
			compProjectile.particle = projectJs.value("particle", "");
			compProjectile.texture = projectJs.value("texture", "");
			compProjectile.texangle = projectJs.value("texangle", 0.0f);

			_context->registry().emplace<CompProjectileCfg>(skill, compProjectile);
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
			trap.particle = waveJs.value("particle", "");
			trap.color = Color::parseHexString(waveJs.value("color", "#FF0000FF"));
			trap.texture = waveJs.value("texture", "");
			_context->registry().emplace<CompTrapCfg>(skill, trap);

			CompWaveCfg wave;
			wave.type = waveJs.value("type", 1);
			wave.grids = waveJs.value("grids", 1);
			wave.interval = waveJs.value("interval", 500);
			_context->registry().emplace<CompWaveCfg>(skill, wave);
		}


		if (json.contains("lightning"))
		{
			auto& lightningJs = json["lightning"];

			CompLightningCfg cfg;
			cfg.target_count = lightningJs.value("target_count", 1);
			cfg.attack_times = lightningJs.value("attack_times", 1);
			cfg.interval = lightningJs.value("interval", 500);
			cfg.color = Color::parseHexString(lightningJs.value("color", "#FF0000FF55"));
			cfg.displace = lightningJs.value("displace", 50.0f);
			cfg.thickness = lightningJs.value("thickness", 2.0f);
			cfg.during = lightningJs.value("during", 500);
			_context->registry().emplace<CompLightningCfg>(skill, cfg);
		}

		return skill;
	}

	particle::ParticlePtr ObjectFactory::createParticleOnObject(entt::entity owner, const std::string& particle)
	{
		assert(_context);

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

	entt::entity ObjectFactory::createProjectile(const Vec2& source, const Vec2& target, const CompProjectileCfg& cfg)
	{
		assert(_context);

		float angle = glm::angle(source, target);

		auto bullet = _context->registry().create();

		CompNameId compName;
		compName.cfg_id = "";
		compName.name = fmt::format("projectile_{}", (uint32_t)bullet);
		compName.id = bullet;
		_context->registry().emplace<CompNameId>(bullet, compName);

		CompTransform compTrans;
		compTrans.position = source;
		_context->registry().emplace<CompTransform>(bullet, compTrans);

		Vec2 dir = glm::normalize(target - source);

		CompProjectileDisplay compDisplay;
		compDisplay.texture = _context->textureMgr().getCfgTexTile(cfg.texture);
		compDisplay.orient = glm::orientedAngle(Vec2{1.0f, 0.0f}, dir) + glm::radians(cfg.texangle);
		compDisplay.offset = - compDisplay.texture->center() * compDisplay.texture->rect().size();
		_context->registry().emplace<CompProjectileDisplay>(bullet, compDisplay);

		createParticleOnObject(bullet, cfg.particle);
	
		return bullet;
	}

	entt::entity ObjectFactory::createTrap(const Vec2& target, float range, const Color& color, 
										const std::string& texture, const std::string& particle, ShapeType shape_type)
	{
		assert(_context);

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
		compTraps.onUpdate = nullptr;
		_context->registry().emplace<CompTraps>(trap, compTraps);

		createParticleOnObject(trap, particle);

		return trap;
	}

	void ObjectFactory::createSkyEffect(SkyEffect effect,  Color color, int last, int fadein, int fadeout)
	{
		assert(_context);

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
					LogInfo("sky effect({}) finish", magic_enum::enum_name(effect).data());
					_context->registry().emplace<CompDestroy>(sky);
					return true;
				}

				LogInfo("sky.color.a = {}", a);

				auto& compSky = _context->registry().get<CompSkyEffect>(sky);
				compSky.color.a = a;
				return false;
			});

		_context->registry().emplace<CompSkyEffect>(sky, compSky);
	}

	void ObjectFactory::destroyObject(entt::entity entityid)
	{
		assert(_context);

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
				LogError("json value is array.");
			}
			else if (value.is_object()) {
				//std::cout << "Object with " << value.size() << " fields" << std::endl;
				LogError("json value is object.");
			}
			else {
				LogError("json type unkonw: tpye = {}", value.type_name());
			}
		} catch (const std::exception& e) {
			LogError("type convert failed. e = {}", e.what());
		}

		return std::nullopt;
	}
	
	bool ObjectFactory::findObjectCfg(const std::string& cfgid)
	{
		auto it = _jsonObjectCfgs.find(cfgid);
		return it != _jsonObjectCfgs.end();
	}

	const Properties& ObjectFactory::getObjectCfgProperties(const std::string& cfgid)
	{
		static Properties props;
		props.clear();

		auto jsonptr = _jsonObjectCfgs[cfgid];
		if (!jsonptr)
		{
			LogError("getObjectCfgProperties: object ({}) cfg NOT found.", cfgid);
			return props;
		}

		auto& json = *jsonptr;

		props["cfgid"] = json.value("id", "");
		props["name"] = Trans(json.value("name", ""));
		props["desc"] = Trans(json.value("desc", ""));
		props["type"] = (int)getNpcType(json.value("type", ""));
		props["side"] = (int)getCampSide(json.value("side", ""));
		props["icon"] = json.value("icon", "");

		return props;
	}
}