#pragma once

#include "comm_comp.h"


namespace game
{
	using namespace engine;

	enum class SkillType 
	{
		Combat, // 近战(刀枪)
		Projectile, // 远程抛射(弓箭、炮铳、火球术)
		Trap, // 陷阱（地刺、地火、陷阱等有生存周期，会造成伤害）
		Sprint, // 冲刺，对一条线上敌人造成伤害
		Wave, // 龟仙波
		Lightning, // 闪电
		Other, // 其他（天黑、降雨等）
		Invalid,
	};

	enum class SkillTarget 
	{
		Role,
		Pos,
		Range,
		Dir,
		CrossMe,
		AroundMe,
		Other,
	};

	enum class TweenTransform 
	{
		Motion,
		Scale,
		Rotate,
		Other,
	};

	struct CompSkillSpell
	{
		std::string prev_tween = "linear";
		std::string post_tween = "linear";

		TweenTransform trans_type = TweenTransform::Motion;
		float trans_value; 

		tweeny::tween<float, float> tween;
	};

	struct CompUnderAttack
	{
		bool under_attack = false;

		int during = 200;
		float motion_offset = 10;

		std::string prev_tween = "quarticIn";
		std::string post_tween = "quarticOut";

		tweeny::tween<float, float> tween;
	};

	struct CompSkillCD
	{
		int total_ticks = 0;
		int current_tick = 0;
	};

	struct CompProjectileCfg
	{
		std::string name;
		float speed = 100.0f;

		std::string particle;
		std::string tween;
	};
	
	struct CompTrapCfg
	{
		float range = 1.0f;
		int duration = 5000;
		int period = 1000;

		std::string particle;
		Color color = Color::Red;
		std::string texture;
	};

	struct CompTraps
	{
		int period_ticks = 0;
		int during_ticks = 0;
		std::function<void(int64_t)> onUpdate = nullptr;
	};

	struct CompWaveCfg
	{
		int type = 1;
		int grids = 3;
		int interval = 500;
	};

	struct CompWave
	{
		int wave_ticks = 0;
		int cur_wave = 0;
		std::function<void(int64_t)> onUpdate = nullptr;
	};

	struct CompLightningCfg
	{
		int target_count = 1;
		int attack_times = 1;
		
		int interval = 500;
		
		float displace = 50.0f;
		Color color = {200, 200, 255, 200};
		float thickness = 3.0f;
		int during = 500;
	};

	struct CompLightning
	{
		int atk_ticks = 0;
		int cur_atk = 0;

		std::function<void(int64_t)> onUpdate = nullptr;
	};

	struct CompLightningDisplay
	{
		std::vector<Vec2> data;

		Color color = { 200, 200, 255, 200 };
		float thickness = 3.0f;
		int during = 500;
	};

	struct CompSprint
	{
		bool running = false;

		float dis = 100.0f;
		float speed = 200.0f;

		std::string particle;
		std::string tween_mode = "linear";

		std::set<Vec2i, Geometry::Vec2iComparator> passed_grids;
		tweeny::tween<float, float> tween;
	};

	struct CompSkillAffect
	{
		// 前摇时间
		int prev_ticks = 0; 
		// 后摇时间
		int post_ticks = 0; 

		// 功能
		// hp:+10,mp:-10,atk:+20%   加减属性
		// buf:+stun,buf:-slow  加减buf
		std::string func;  

		// 目标：对象还是位置等
		SkillTarget target;

		// 目标范围：aoe 范围攻击
		float range;

		// 事件：引起的额外时间
		std::string event;
	};

	struct CompTargetRole 
	{
		entt::entity target;
	};
	
	struct CompTragetPos 
	{
		Vec2 pos;
	};

	enum class SkillState {
		Launching,
		Cooling,
		OK,
	};

	struct CompSkillComm
	{
		SkillType type;

		entt::entity owner;

		SkillState state = SkillState::OK;

		std::string desc;

		float distance = 0.0f;
	};

	struct CompBuffComm 
	{
		std::string cfgid;
		entt::entity owner;
		
		// 功能
		// hp:+10,mp:-10,atk:+10%   加减属性
		// buf:+stun,buf:-slow  加减buf
		std::string func;

		int duration = 0; // 持续时间，ms， -1表示永久
		int period = 0;	// 周期（定时触发）

		int64_t period_ticks = 0;
		int64_t during_ticks = 0;

		std::function<void(int64_t)> onUpdate = nullptr;
	};

	// 基础属性
	struct CompBaseProp
	{
		int lv;
		int exp;

		float str;
		float cst;
		float dex;
		float met;
	};

	// 战斗属性
	struct CompFightProp
	{
		float hp;
		float hpm;
		float hpr;

		float atk;
		float def;
		float mvs;
		float ats;
		float atd;
		float crt;
		float par;
	};

	// 技能列表
	struct CompSkills {
		std::vector<entt::entity> skills;
	};

	// buf列表
	struct CompBuffs {
		std::list<entt::entity> buffs;
	};

	// 物品列表
	struct CompItems {
		std::list<entt::entity> items;
	};

	// 发射
	struct CompShoot
	{
		tweeny::tween<float, float> tween;
	};

	// 战斗飘字
	struct CompFightText
	{
		std::string text;

		Color color;
		Font* font = nullptr;

		//float font_size;
		tweeny::tween<float, float> tween;
	};


	inline SkillType getSkillType(const std::string& type) 
	{
		if (type == "combat") return SkillType::Combat;
		if (type == "projectile") return SkillType::Projectile;
		if (type == "trap") return SkillType::Trap;
		if (type == "wave") return SkillType::Wave;
		if (type == "sprint") return SkillType::Sprint;
		if (type == "lightning") return SkillType::Lightning;
		if (type == "other") return SkillType::Other;
		LogError("skill type ({}) NOT support", type);
		return SkillType::Other;
	};

	inline SkillTarget getSkillTarget(const std::string& target) 
	{
		if (target == "role")  return SkillTarget::Role;
		if (target == "pos") return SkillTarget::Pos;
		if (target == "range") return SkillTarget::Range;
		if (target == "dir") return SkillTarget::Dir;
		if (target == "cross") return SkillTarget::CrossMe;
		if (target == "around") return SkillTarget::AroundMe;
		if (target == "other") return SkillTarget::Other;
		LogError("skill target ({}) NOT support", target);
		return SkillTarget::Other;
	};

	inline TweenTransform getTransType(const std::string& trans) 
	{
		if (trans == "motion") return TweenTransform::Motion;
		if (trans == "scale") return TweenTransform::Scale;
		if (trans == "rotate") return TweenTransform::Rotate;
		if (trans == "other") return TweenTransform::Other;
		LogError("tween trans ({}) NOT support", trans);
		return TweenTransform::Other;
	};
}
