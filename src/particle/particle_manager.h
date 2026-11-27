#pragma once

#include "particle_cfg.h"
#include "particle_emitter.h"
#include "particle_effect.h"

#include "engine/application.h"
#include "engine/texture.h"
#include "engine/camera.h"
#include "utility/i_singleton.h"

namespace engine
{
	class IPainter;
}

namespace particle
{
	class Particle
	{
	public:
		using ParticleConfigMap = std::map<std::string, std::string>;

	public:
		Particle(const std::string& name);
		~Particle();

		// 返回配置名和配置文件路径。
		std::string getName();
		std::string getFile();

		// 从 xml 配置加载粒子。
		void load(const std::string& cfgdir);

		// 应用粒子描述。
		void setDescription(const ParticleDescription& desc);

		void update(float dt);
		void draw(engine::IPainter& painter, engine::Camera* camera = nullptr, bool screenSpace = false);

		void start();
		void stop();

		void setAlphaScale(float alpha);
		float getAlphaScale() const;

		Vec2 getPos();
		void setPos(const Vec2& pos);
		void setOwnerPos(const Vec2& pos);

		bool isPlaying();

		ParticleEffect* getEffect();
		ParticleEmitter* getEmitter();

		// 修正编辑器输入数据。
		static void correctDescription(ParticleDescription& desc);

	private:
		// 从 xml 创建粒子描述。
		static ParticleDescription createParticleDescription(const std::string& filename);

		void setTexture(const std::string& file);
		void emitSubParticle(const Vec2& pos);
		void drawParticleList(engine::IPainter& painter, const std::vector<ParticleCfg*>* particleList, engine::Camera* camera, bool screenSpace);

	private:
		std::string _name;
		std::string _file;

		std::unique_ptr<ParticleEmitter> _emitter = nullptr;

		TexTile* _texTile = nullptr;

		std::vector<Vertex> _vertexData;

		std::vector<int> _indices;

		float _alphaScale = 1.0f;
		Vec2 _localEmitPos = {0.0f, 0.0f};
		std::vector<std::shared_ptr<Particle>> _subParticles;

	public:
		ParticleDescription& description();
		void reset();
		void save();

	private:
		ParticleDescription _descript;
	};

	using ParticlePtr = std::shared_ptr<Particle> ;

	// 粒子系统管理器。
	class ParticleManager : public utility::ISingleton<ParticleManager>
	{
		friend class Particle;

	public:
		ParticleManager();
		~ParticleManager();

		void init(engine::Application* app);

		bool loadParticles(const std::filesystem::path& file);

		bool reload();

		ParticlePtr createParticle(const std::string& name);

		const StringStringMap& getAllParticleConfigs();

	private:
		engine::Application* getApplication();

	private:
		engine::Application* _application;

		StringStringMap _particleFiles;

		std::filesystem::path _cfgDir;

	};
}
