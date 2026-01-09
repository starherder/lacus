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
		std::string affect_formula; // 影响公式，由公式编辑器或者lua中根据角色等级属性等计算出对目标的伤害或者加成
		SkillTarget affect_target;
		float affect_range;

		int prev_ticks = 0;
		int post_ticks = 0;

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

		// (min,max)
		Vec2 distance = { 0, 0 };
	};


	// 预设：智力、灵巧、灵性、力量等预设值
	struct CompPresets
	{
		utility::DynamicStruct<std::string> properties;
	};

	// HP、攻击力、防御力、移动速度、攻击速度等经过公式计算后得到的属性
	struct CompProps
	{
		utility::DynamicStruct<std::string> properties;
	};

	struct CompSkills {
		std::vector<entt::entity> skills;
	};

	struct CompBuffs {
		std::vector<entt::entity> buffs;
	};

	struct CompItems {
		std::vector<entt::entity> items;
	};

	struct CompShoot
	{
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
