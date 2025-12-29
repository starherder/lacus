#pragma once

#include "particle_header.h"

namespace particle
{
	/* 发射器类型 */
	enum class EmitterType
	{
		EMITTER_TYPE_GRAVITY,	/* 重力模式 */
		EMITTER_TYPE_RADIUS		/* 半径模式 */
	};

	/* 粒子运动模式 */
	enum class MotionMode
	{
		MOTION_MODE_FREE,		/* 粒子运动和发射器无关 */
		MOTION_MODE_RELATIVE	/* 粒子运动跟随发射器位置 */
	};

	/* 重力模式 */
	struct GravityMode
	{
		Vec2 vGravity = {0, 0};				/* 重力方向 */

		float fTangentialAccel = 0.0f;		/* 切向加速度 */
		float fTangentialAccelVar = 0.0f;	/* 径向加速度变化值 */

		float fRadialAccel = 0.0f;			/* 径向加速度 */
		float fRadialAccelVar = 0.0f;		/* 径向加速度变化值 */
	};

	/* 半径模式 */
	struct RadiusMode
	{
		float fBeginRadius = 0.0f;		/* 起始半径  */
		float fBeginRadiusVar = 0.0f;	/* 起始半径变化值 */

		float fEndRadius = 0.0f;		/* 结束半径 */
		float fEndRadiusVar = 0.0f;		/* 结束半径变化值 */

		float fSpinPerSecond = 0.0f;	/* 每秒旋转角度 */
		float fSpinPerSecondVar = 0.0f;	/* 每秒旋转角度变化值 */
	};


	struct ParticleCfg
	{
		Vec2 vPos = {0, 0};
		Vec2 vChangePos = {0, 0};
		Vec2 vStartPos = {0, 0};

		Color4f cColor = {0,0,0,0};
		Color4f cDeltaColor = {0,0,0,0};

		float fCurrentSize = 0.0f;
		float fSize = 0.0f;
		float fDeltaSize = 0.0f;

		float fRotation = 0.0f;
		float fDeltaRotation = 0.0f;

		float fRemainingLife = 0.0f;

		/* 重力模式数据 */
		struct GravityModeData
		{
			Vec2  vInitialVelocity = {0, 0};	/* 初速度 */
			float fRadialAccel = 0.0f;			/* 径向加速度（法相加速度）， 与运动方向垂直 */
			float fTangentialAccel = 0.0f;		/* 切向加速度 */
		} gravityMode;

		/* 半径模式数据 */
		struct RadiusModeData
		{
			float fAngle = 0.0f;				/* 发射角度 */
			float fDegressPerSecond = 0.0f;		/* 每秒旋转角度 */
			float fRadius = 0.0f;				/* 半径 */
			float fDelatRadius = 0.0f;			/* 半径变化量 */
		} radiusMode;
	};
	
	/* 发射器属性 */
	struct ParticleDescription
	{
		Vec2 vEmitPos = {0, 0};				/* 发射器位置 */
		Vec2 vEmitPosVar = {0, 0};			

		float fEmitAngle = 0.0f;			/* 发射器发射粒子角度 */
		float fEmitAngleVar = 0.0f;
				
		float fEmitSpeed = 0.0f;			/* 发射器发射粒子速度 */
		float fEmitSpeedVar = 0.0f;

		int nParticleCount = 0;			/* 粒子数量 */
		float fEmitRate = 0.0f;			/* 粒子每秒发射速率 */
		float fDuration = -1.0f;			/* 发射器发射粒子时间 */

		EmitterType emitterType = EmitterType::EMITTER_TYPE_GRAVITY;
		MotionMode  motionMode = MotionMode::MOTION_MODE_FREE;

		/* 粒子属性 */

		/* 粒子生命周期 */
		float fLife  = 0.0f;
		float fLifeVar = 0.0f;

		/* 粒子的颜色变化 */
		Color4f cBeginColor;
		Color4f cBeginColorVar;
		Color4f cEndColor;
		Color4f cEndColorVar;

		/* 粒子的大小变化 */
		float fBeginSize = 0.0f;
		float fBeginSizeVar = 0.0f;
		float fEndSize = 0.0f;
		float fEndSizeVar = 0.0f;

		/* 粒子旋转角度变化 */
		float fBeginSpin = 0.0f;
		float fBeginSpinVar = 0.0f;
		float fEndSpin = 0.0f;
		float fEndSpinVar = 0.0f;

		std::string sTexture;

		GravityMode gravityMode;
		RadiusMode radiusMode;
	};
}