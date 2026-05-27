#pragma once

#include "comp_effect.h"
#include "system_effect.h"

#include "utility/i_singleton.h"
#include "nlohmann/json_fwd.hpp"


/*
	一个特效(Effect)由多个子特效(SubEffect)和多个子特效组(SubEffectGroup)组成。
	多个子特效顺序执行，同一个组内的多个特效并发执行。
	目前想法是在entity上创建特效时，创建一个tween来作为定时器使用，
	每个tween.point节点执行下一个特效， 执行时在entity身上创建一个对应的子特效组件，
	子特效执行完就删除掉该组件。
	所有特效组件的upate由effect_system负责

	子特效类型有 移动、缩放、抖动、旋转、透明度变化、颜色渐变、粒子、声音等等
*/



namespace game
{
	using namespace engine;

	class GameContext;

	enum class EffectState 
	{
		None,
		Ready,
		Running,
		Finish,
	};


	class SubEffect 
	{
	public:
		SubEffect() = delete;
		SubEffect(entt::entity ent) : _entity(ent) {}
		virtual ~SubEffect() {}

		entt::entity entity() { return _entity; }

		EffectState state() { return _state; }

		virtual bool create(const nlohmann::json& js) = 0;

		virtual void execute() = 0;

		//virtual void update(float delta) = 0;
		//virtual void draw() = 0;

		//virtual void start() = 0;
		//virtual void stop() = 0;

	protected:
		EffectState _state = EffectState::None;
		entt::entity _entity;
	};


	class SubEffectGroup : public SubEffect
	{
		using SubEffectPtr = std::shared_ptr<SubEffect>;

	public:
		bool create(const nlohmann::json& js) override;

		void execute() override;

		//void update(float delta) override {}
		//void draw() override {}

		//void start() override {}
		//void stop() override {}

		const auto& subEffects() { return _subEffects; }
	private:


	private:
		std::map<std::string, SubEffectPtr> _subEffects;
	};



	class EffectManager : public utility::ISingleton<EffectManager>
	{
	public:
		EffectManager() = delete;
		EffectManager(EffectManager&&) = delete;
		EffectManager(const EffectManager&) = delete;

		EffectManager(GameContext& context);
		~EffectManager();

		bool load(const fs::path& cfgfile);
		void reload();

		bool createEffectOnEntity(entt::entity ent, const std::string& effect_name);


	private:
		GameContext& _context;
	};

}