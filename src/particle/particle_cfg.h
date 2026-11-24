#pragma once

#include "particle_header.h"

namespace particle
{
	// 发射器类型。
	enum class EmitterType
	{
		Gravity, // 受重力、速度和加速度影响。
		Radius // 以极坐标绕原点运动。
	};

	// 粒子运动模式。
	enum class MotionMode
	{
		Free, // 粒子运动不跟随发射器。
		Relative // 粒子运动跟随发射器位置。
	};

	// 发射器形状。
	enum class EmitterShape
	{
		Point, // 点。
		Line, // 线段。
		Rect, // 矩形。
		Circle // 圆形。
	};

	// 重力模式参数。
	struct GravityMode
	{
		Vec2 gravity = {0, 0}; // 重力方向。

		float tangentialAccel = 0.0f; // 切向加速度。
		float tangentialAccelVar = 0.0f; // 切向加速度随机范围。

		float radialAccel = 0.0f; // 径向加速度。
		float radialAccelVar = 0.0f; // 径向加速度随机范围。
	};

	// 半径模式参数。
	struct RadiusMode
	{
		float beginRadius = 0.0f; // 起始半径。
		float beginRadiusVar = 0.0f; // 起始半径随机范围。

		float endRadius = 0.0f; // 结束半径。
		float endRadiusVar = 0.0f; // 结束半径随机范围。

		float spinPerSecond = 0.0f; // 每秒旋转角度。
		float spinPerSecondVar = 0.0f; // 每秒旋转角度随机范围。
	};

	// 单个运行中粒子的数据。
	struct ParticleCfg
	{
		Vec2 pos = {0, 0};
		Vec2 changePos = {0, 0};
		Vec2 startPos = {0, 0};

		Color color = {0, 0, 0, 0};
		Color deltaColor = {0, 0, 0, 0};

		float currentSize = 0.0f;
		float size = 0.0f;
		float deltaSize = 0.0f;

		float rotation = 0.0f;
		float deltaRotation = 0.0f;

		float remainingLife = 0.0f;

		// 重力模式运行数据。
		struct GravityModeData
		{
			Vec2 initialVelocity = {0, 0}; // 初速度。
			float radialAccel = 0.0f; // 径向加速度。
			float tangentialAccel = 0.0f; // 切向加速度。
		} gravityMode;

		// 半径模式运行数据。
		struct RadiusModeData
		{
			float angle = 0.0f; // 发射角度。
			float degreesPerSecond = 0.0f; // 每秒旋转角度。
			float radius = 0.0f; // 半径。
			float deltaRadius = 0.0f; // 半径变化量。
		} radiusMode;
	};

	// 粒子发射器和粒子表现配置。
	struct ParticleDescription
	{
		Vec2 emitPos = {0, 0}; // 发射器位置。
		Vec2 emitPosVar = {0, 0}; // 发射位置随机范围。

		float emitAngle = 0.0f; // 发射角度。
		float emitAngleVar = 0.0f; // 发射角度随机范围。

		float emitSpeed = 0.0f; // 发射速度。
		float emitSpeedVar = 0.0f; // 发射速度随机范围。

		EmitterShape emitterShape = EmitterShape::Rect;
		Vec2 emitShapeSize = {0, 0}; // 线段或矩形大小。
		float emitShapeRadius = 0.0f; // 圆形半径。
		float emitShapeRotation = 0.0f; // 线段或矩形旋转角度。
		bool emitShapeEdgeOnly = false; // 是否只从边缘发射。

		int particleCount = 0; // 最大粒子数量。
		float emitRate = 0.0f; // 每秒发射速率。
		float duration = -1.0f; // 发射持续时间，-1 表示持续发射。

		EmitterType emitterType = EmitterType::Gravity;
		MotionMode motionMode = MotionMode::Free;

		float life = 0.0f; // 粒子生命周期。
		float lifeVar = 0.0f; // 生命周期随机范围。

		Color beginColor; // 起始颜色。
		Color beginColorVar; // 起始颜色随机范围。
		Color endColor; // 结束颜色。
		Color endColorVar; // 结束颜色随机范围。

		float beginSize = 0.0f; // 起始大小。
		float beginSizeVar = 0.0f; // 起始大小随机范围。
		float endSize = 0.0f; // 结束大小。
		float endSizeVar = 0.0f; // 结束大小随机范围。

		float beginSpin = 0.0f; // 起始旋转角度。
		float beginSpinVar = 0.0f; // 起始旋转角度随机范围。
		float endSpin = 0.0f; // 结束旋转角度。
		float endSpinVar = 0.0f; // 结束旋转角度随机范围。

		std::string texture; // 粒子贴图。

		GravityMode gravityMode;
		RadiusMode radiusMode;
	};
}
