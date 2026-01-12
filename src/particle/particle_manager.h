#pragma once

#include "particle_cfg.h"
#include "particle_emitter.h"
#include "particle_effect.h"

#include "engine/application.h"
#include "engine/texture.h"
#include "engine/camera.h"
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

		void Load(const std::string& cfgdir);

		void SetDescription(const ParticleDescription& desc);

		void Update(float dt);
		void Draw(engine::Camera* camera = nullptr);

		void Start();
		void Stop();

		Vec2 GetPos();
		void SetPos(const Vec2& pos);

		bool IsPlaying();

		ParticleEffect* GetEffect() { return _emitter->getParticleEffect(); }
		ParticleEmitter* GetEmitter() { return _emitter.get(); }

		// 修正一些数据(Editor 用)
		static void CorrectDescription(ParticleDescription& desc);

	private:
		// 创建粒子描述
		static ParticleDescription CreateParticleDescription(const std::string& filename);

		void SetTexture(const std::string& file);

	private:
		std::string _name;
		std::string _file;

		//ParticleEmitter* _emitter;
		std::unique_ptr<ParticleEmitter> _emitter = nullptr;

		Texture* m_Texture = nullptr;

		std::vector<Vertex> _vertexData;

		std::vector<int> _indices;

#define Particle_Editor_Mode
#ifdef Particle_Editor_Mode
	public:
		ParticleDescription& Description();
		void Reset();
		void Save();
	private:
		ParticleDescription _descript;
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

		bool LoadParticles(const std::filesystem::path& file);

		bool Reload();

		ParticlePtr CreateParticle(const std::string& name);

		const auto& GetAllParticleConfigs() { return _particleFiles; }

	private:
		engine::Application* GetApplication() { return _application; }

	private:
		engine::Application* _application;

		StringStringMap _particleFiles;

		std::filesystem::path _cfgDir;

	};
}