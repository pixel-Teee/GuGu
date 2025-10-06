#pragma once

#include <Core/Model/BoneInfo.h>
#include "Keyframe.h"

namespace GuGu {
	//animation
	class GAnimation : public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		GAnimation();

		virtual ~GAnimation();

		virtual void PostLoad() override;

		virtual void Update(float fElapsedTimeSeconds) override;
		virtual meta::Type GetType() const override;
		//拷贝对象
		virtual Object* Clone(void) const override;
		//序列化
		virtual void OnSerialize(nlohmann::json& output) const override;
		//反序列化
		virtual void OnDeserialize(const nlohmann::json& input) override;

		math::float4x4 getBoneLocalTransform(GuGuUtf8Str boneName, float frame);

		math::float4x4 interpolatePosition(GuGuUtf8Str channelName, float frame);
		math::float4x4 interpolateRotation(GuGuUtf8Str channelName, float frame);
		math::float4x4 interpolateScale(GuGuUtf8Str channelName, float frame);

		int32_t getPositionIndex(Array<KeyPosition>& keyPositions, float frame);
		int32_t getRotationIndex(Array<KeyRotation>& keyPositions, float frame);
		int32_t getScaleIndex(Array<KeyScale>& keyPositions, float frame);

		float getScaleFactor(float lastTimeStamp, float nextStamp, float frame);

		//持续时间
		float m_duration;

		//每秒多少帧
		int32_t m_ticksPerSecond;

		//bone map(骨骼名，对应骨骼信息)，运行时信息
		std::map<GuGuUtf8Str, BoneInfo> m_boneInfos;

		//序列化用
		Array<BoneInfo> m_boneInfoArray;

		//animation
		Array<Channel> m_channels;
	};
}