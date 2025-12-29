#pragma once 
#include "entt/entt.hpp"
#include "engine/wrapper.h"
#include "engine/texture.h"

namespace partial {

	using namespace engine;

	struct CompParticle {
		float lifetime;
	};

	struct CompMotion {
		Vec2 pos;
		Vec2 vel;
	};

	struct CompRotate {
		float rotation;
		float rotation_speed;
	};

	struct CompColor {
		Color begin;
		Color end;
	};

	struct CompTexture {
		engine::Texture* texture;
		Rect tex_rect;
	};



	

}