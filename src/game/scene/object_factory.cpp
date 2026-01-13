#include "object_factory.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include "game/ecs/comm_comp.h"
#include "game/ecs/comp_fight.h"
#include "game/ecs/comm_event.h"

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

		std::string cfgid = jsonptr->value("id", "");
		if (cfgid.empty())
		{
			spdlog::error("role file({}) NOT found cfgid.", cfgfile.string());
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

		std::string cfgid = jsonptr->value("cfgid", "");
		if(cfgid.empty())
		{
			spdlog::error("skill file({}) NOT found cfgid.", cfgfile.string());
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

		std::string cfgid = jsonptr->value("cfgid", "");
		if (cfgid.empty())
		{
			spdlog::error("buff file({}) NOT found cfgid.", cfgfile.string());
			return false;
		}

		_buffCfgIds.push_back(cfgid);

		_jsonBuffCfgs[cfgid] = jsonptr;
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

		if (json.contains("common"))
		{
			auto& cmmJs = json["common"];

			CompComm comm;
			comm.type = getNpcType(cmmJs.value("type", ""));
			comm.desc = Trans(cmmJs.value("desc", ""));

			_context->registry().emplace<CompComm>(object, comm);
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
			com.effect = pickable.value("effect", "");
			_context->registry().emplace<CompPickable>(object, com);
		}

		spdlog::info("create object: cfg = ({}), object = ({})", cfgid, (uint32_t)object);
		
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
			float roll = baseJs["roll"];

			CompBaseProp compBase;
			compBase.str = baseJs.value("str", 0.0f) + utility::Random_Minus1_1() * roll;
			compBase.cst = baseJs.value("cst", 0.0f) + utility::Random_Minus1_1() * roll;
			compBase.dex = baseJs.value("dex", 0.0f) + utility::Random_Minus1_1() * roll;
			compBase.met = baseJs.value("met", 0.0f) + utility::Random_Minus1_1() * roll;

			compBase.str = std::clamp(compBase.str, 10.0f, 100.0f);
			compBase.cst = std::clamp(compBase.cst, 10.0f, 100.0f);
			compBase.dex = std::clamp(compBase.dex, 10.0f, 100.0f);
			compBase.met = std::clamp(compBase.met, 10.0f, 100.0f);

			_context->registry().emplace<CompBaseProp>(role, compBase);

			CompFightProp fightProp;
			_context->registry().emplace<CompFightProp>(role, fightProp);

			_context->dispatcher().trigger(RoleLevelAlter{role, 1});
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
			spdlog::error("ObjectFactory need Load first.");
			return entt::null;
		}

		auto jsonptr = _jsonBuffCfgs[cfgid];
		if (!jsonptr)
		{
			spdlog::error("object ({}) cfg NOT found.", cfgid);
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

			CompSkillTween compTween;
			compTween.trans_type = getTransType(tweenJs.value("trans_type", ""));
			compTween.trans_value = tweenJs.value("trans_value", 10.0f);
			compTween.prev_tween = tweenJs.value("prev_tween", "");
			compTween.post_tween = tweenJs.value("post_tween", "");
			_context->registry().emplace<CompSkillTween>(skill, compTween);
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

		spdlog::info("create skill ({}) on ({}) OK.", cfgid, (uint32_t)owner);
		return skill;
	}

	particle::ParticlePtr ObjectFactory::createParticleOnObject(entt::entity owner, const std::string& particle)
	{
		if (!_context || _context->registry().valid(owner) == false)
		{
			spdlog::error("create particle ({}) on invald object({})", particle, (uint32_t)owner);
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

	entt::entity ObjectFactory::createProjectile(const Vec2& source, const Vec2& target, float speed, const std::string& tween_type, const std::string& particle)
	{
		assert(speed != 0.0f);
		speed = (speed == 0) ? 100.0f : speed;

		auto bullet = _context->registry().create();

		CompNameId compName;
		compName.cfg_id = "";
		compName.name = fmt::format("projectile_{}", (uint32_t)bullet);
		compName.id = bullet;
		_context->registry().emplace<CompNameId>(bullet, compName);

		CompTransform compTrans;
		_context->registry().emplace<CompTransform>(bullet, compTrans);

		auto res = createParticleOnObject(bullet, particle);
		if (!res) 
		{
			spdlog::error("createProjectile: create particle failed.");
		}

		spdlog::info("create projectile {} (source:({},{}), target({},{}), particle:{}) OK.", 
			(uint32_t)bullet, source.x, source.y, target.x, target.y, particle );

		return bullet;
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
					spdlog::info("sky effect({}) finish", magic_enum::enum_name(effect).data());
					_context->registry().emplace<CompDestroy>(sky);
					return true;
				}

				spdlog::info("sky.color.a = {}", a);

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