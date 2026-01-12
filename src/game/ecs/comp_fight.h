#pragma once

#include "comm_comp.h"


namespace game
{
	using namespace engine;

	enum class SkillType 
	{
		Combat, // 近战(刀枪)
		Projectile, // 远程抛射(弓箭、炮铳、火球术)
		Remote, // 远程魔法（闪电,加Buff等）
		Other, // 其他（天黑、降雨等）
	};

	enum class SkillTarget 
	{
		Role,
		Pos,
		Range,
		Other,
	};

	enum class TweenTransform 
	{
		Motion,
		Scale,
		Rotate,
		Other,
	};

	struct CompSkillTween
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
	
	struct CompTargetAll 
	{
	};

	enum class SkillState {
		Launching,
		Cooling,
		OK,
	};

	struct CompSkillComm
	{
		entt::entity owner;

		SkillState state = SkillState::OK;

		SkillType type;

		std::string desc;

		float distance = 0.0f;
	};

	struct CompBuffComm 
	{
		std::string cfgid;
		//std::string name;
		//std::string desc;

		entt::entity owner;
		
		// 功能
		// hp:+10,mp:-10,atk:+10%   加减属性
		// buf:+stun,buf:-slow  加减buf
		std::string func;

		// 持续时间，ms， -1表示永久
		int duration = 0; 

		// 周期（定时触发）
		int period = 0;

		int64_t period_flag = 0;

		int64_t during_ticks = 0;

		std::function<void(int64_t)> onUpdate = nullptr;
		//tweeny::tween<float, float> tween;
	};


	// 基础属性
	struct CompBaseProp
	{
		int lv;

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
		if (type == "remote") return SkillType::Remote;
		if (type == "other") return SkillType::Other;
		spdlog::error("skill type ({}) NOT support", type);
		return SkillType::Other;
	};

	inline SkillTarget getSkillTarget(const std::string& target) 
	{
		if (target == "role")  return SkillTarget::Role;
		if (target == "pos") return SkillTarget::Pos;
		if (target == "range") return SkillTarget::Range;
		if (target == "other") return SkillTarget::Other;
		spdlog::error("skill target ({}) NOT support", target);
		return SkillTarget::Other;
	};

	inline TweenTransform getTransType(const std::string& trans) 
	{
		if (trans == "motion") return TweenTransform::Motion;
		if (trans == "scale") return TweenTransform::Scale;
		if (trans == "rotate") return TweenTransform::Rotate;
		if (trans == "other") return TweenTransform::Other;
		spdlog::error("tween trans ({}) NOT support", trans);
		return TweenTransform::Other;
	};
}
