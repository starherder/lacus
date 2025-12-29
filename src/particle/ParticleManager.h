#pragma once

#include "ParticleCfg.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "ParticleDescription.h"

#include "engine/application.h"
#include "engine/texture.h"
#include "utility/i_singleton.h"


namespace particle
{
	class Particle
	{
	public:
		typedef std::map<std::string, std::string> ParticleConfigMap;

	public:
		Particle(const std::string& name);
		~Particle();

		std::string GetName();
		std::string GetFile();

		void Load(const std::string& filename);
		void SetDescription(const ParticleDescription& desc);

		void Update(float dt);
		void Draw();

		void Start();
		void Stop();

		Vec2 GetPos();
		void SetPos(const Vec2& pos);

		bool IsPlaying();

		ParticleEffect* GetEffect() { return m_pEmitter->getParticleEffect(); }
		ParticleEmitter* GetEmitter() { return m_pEmitter.get(); }

	private:
		// 创建粒子描述
		static ParticleDescription CreateParticleDescription(const std::string& filename);

		void SetTexture(const std::string& file);

		// 修正一些数据
		static void CorrectDescription(ParticleDescription& desc);

	private:
		std::string m_Name;
		std::string m_File;

		//ParticleEmitter* m_pEmitter;
		std::unique_ptr<ParticleEmitter> m_pEmitter = nullptr;

		Texture* m_Texture = nullptr;

		std::vector<Vertex> m_VertexData;
		std::vector<int> m_Indices;

#define Particle_Editor_Mode
#ifdef Particle_Editor_Mode
	public:
		ParticleDescription& Description();
		void Reset();
		void Save();
	private:
		ParticleDescription m_Descript;
#endif
	};

	using ParticlePtr = std::shared_ptr<Particle> ;

	//-------------------------------------------------------------------------------
	// 粒子系统管理器
	// ParticleSystemManager
	//-------------------------------------------------------------------------------
	class ParticleManager : public utility::ISingleton<ParticleManager>
	{
		friend class Particle;

	public:
		ParticleManager();
		~ParticleManager();

		void init(engine::Application* app);

		bool LoadParticles(const std::string& file);

		ParticlePtr CreateParticle(const std::string& name);

		const auto& GetAllParticleConfigs() { return m_ParticleFiles; }

	private:
		engine::Application* GetApplication() { return _application; }

	private:
		engine::Application* _application;
		StringStringMap m_ParticleFiles;

	};
}