#include <pch.h>

#include "GAnimation.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

namespace GuGu {
	static bool registerGuGuGAnimation()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::GAnimation");
			auto& type = db.types[id];
			meta::TypeInfo<GAnimation>::Register(id, type, true, "2B9E11D1-1D66-457E-8CE9-60BE82518687");

			auto typeID = typeidof(GAnimation);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GAnimation>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<GAnimation>();

				type.AddConstructor<GAnimation, false, false>({});

				type.AddConstructor<GAnimation, true, true>({});

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				meta::TypeInfo<GAnimation>::Defined = true;
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::GAnimation>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<GAnimation>>::Register(id, type, false, "38512377-656E-4B5E-B217-F763DBFAD196");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::GAnimation>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<GAnimation>>::Register(id, type, false, "EF61A1B3-E9DE-4BFF-8A22-BD44B3DD1ECA");
			}
		}
		return true;
	}
	static bool registerGuGuGAnimationFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		auto& type = db.types[typeof(GAnimation).GetID()];

		type.AddField<GAnimation, int32_t>("m_ticksPerSecond",
			(meta::FieldGetter<GAnimation, int32_t, false>::Signature) & GAnimation::m_ticksPerSecond,
			(meta::FieldSetter<GAnimation, int32_t, false>::Signature) & GAnimation::m_ticksPerSecond, {});

		type.AddField<GAnimation, float>("m_duration",
			(meta::FieldGetter<GAnimation, float, false>::Signature) & GAnimation::m_duration,
			(meta::FieldSetter<GAnimation, float, false>::Signature) & GAnimation::m_duration, {});

		type.AddField<GAnimation, Array<Channel>>("m_channels",
			(meta::FieldGetter<GAnimation, Array<Channel>, false>::Signature) & GAnimation::m_channels,
			(meta::FieldSetter<GAnimation, Array<Channel>, false>::Signature) & GAnimation::m_channels, {});

		type.AddField<GAnimation, Array<BoneInfo>>("m_boneInfoArray",
			(meta::FieldGetter<GAnimation, Array<BoneInfo>, false>::Signature) & GAnimation::m_boneInfoArray,
			(meta::FieldSetter<GAnimation, Array<BoneInfo>, false>::Signature) & GAnimation::m_boneInfoArray, {});
		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GAnimation)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGAnimation)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GAnimation)
		if (!ms_priority2.addPriorityThan(&GuGu::channelPriority)) return 0;
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGAnimationFields)
	IMPLEMENT_INITIAL_FIELDS_END
	GAnimation::GAnimation()
	{
	}
	GAnimation::~GAnimation()
	{
	}

	void GAnimation::PostLoad()
	{
		for (int32_t i = 0; i < m_boneInfoArray.size(); ++i)
		{
			m_boneInfos.insert({ m_boneInfoArray[i].m_boneName, m_boneInfoArray[i] });
		}
	}

	void GAnimation::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type GAnimation::GetType() const
	{
		return typeof(GAnimation);
	}

	meta::Object* GAnimation::Clone(void) const
	{
		GAnimation* animation = new GAnimation();
		animation->m_channels = m_channels;
		animation->m_boneInfoArray = m_boneInfoArray;
		animation->m_ticksPerSecond = m_ticksPerSecond;
		animation->m_duration = m_duration;
		animation->m_boneInfos = m_boneInfos;
		return animation;
	}

	void GAnimation::OnSerialize(nlohmann::json& output) const
	{

	}

	void GAnimation::OnDeserialize(const nlohmann::json& input)
	{
		
	}

	math::float4x4 GAnimation::getBoneLocalTransform(GuGuUtf8Str boneName, float frame)
	{
		math::float4x4 translation = interpolatePosition(boneName, frame);
		math::float4x4 rotation = interpolateRotation(boneName, frame);
		math::float4x4 scale = interpolateScale(boneName, frame);
		//return scale * rotation * translation;
		return scale * rotation * translation;
	}

	math::float4x4 GAnimation::interpolatePosition(GuGuUtf8Str channelName, float frame)
	{
		GuGuUtf8Str currentChannelName = channelName;
		//find channel(todo:fix)
		//auto it = std::find_if(m_channels.begin(), m_channels.end(), [=](const Channel& a) {
		//	if (a.m_name == currentChannelName)
		//		return true;
		//	return false;
		//});
		int32_t findIndex = -1;
		for (int32_t i = 0; i < m_channels.size(); ++i)
		{
			if (m_channels[i].m_name == currentChannelName)
			{
				findIndex = i;
				break;
			}
		}
		if (findIndex != -1)
		{
			Channel channel = m_channels[findIndex];
			if (channel.m_positions.size() == 1)
			{
				return math::affineToHomogeneous(math::translation(channel.m_positions[0].m_position));
			}
			else
			{
				int32_t frameIndex1 = getPositionIndex(channel.m_positions, frame);
				int32_t frameIndex2 = frameIndex1 + 1;
				float scaleFactor = getScaleFactor(channel.m_positions[frameIndex1].m_timestamp,
					channel.m_positions[frameIndex2].m_timestamp, frame);
				math::float3 finalPosition = math::lerp(channel.m_positions[frameIndex1].m_position, channel.m_positions[frameIndex2].m_position, scaleFactor);
				return math::affineToHomogeneous(math::translation(finalPosition));
			}
		}
		return math::float4x4::identity();
	}

	math::float4x4 GAnimation::interpolateRotation(GuGuUtf8Str channelName, float frame)
	{
		GuGuUtf8Str currentChannelName = channelName;
		//find channel(todo:fix)
		//auto it = std::find_if(m_channels.begin(), m_channels.end(), [=](const Channel& a) {
		//	if (a.m_name == currentChannelName)
		//		return true;
		//	return false;
		//});
		int32_t findIndex = -1;
		for (int32_t i = 0; i < m_channels.size(); ++i)
		{
			if (m_channels[i].m_name == currentChannelName)
			{
				findIndex = i;
				break;
			}
		}
		if (findIndex != -1)
		{
			Channel channel = m_channels[findIndex];
			if (channel.m_rotations.size() == 1)
			{
				return math::affineToHomogeneous(math::quat(channel.m_rotations[0].m_orientation).toAffine());
			}
			else
			{
				int32_t frameIndex1 = getRotationIndex(channel.m_rotations, frame);
				int32_t frameIndex2 = frameIndex1 + 1;
				float scaleFactor = getScaleFactor(channel.m_rotations[frameIndex1].m_timestamp,
					channel.m_rotations[frameIndex2].m_timestamp, frame);
				math::quat finalQuat = math::slerp(math::normalize(channel.m_rotations[frameIndex1].m_orientation), math::normalize(channel.m_rotations[frameIndex2].m_orientation), scaleFactor);
				//return finalQuat.toMatrix();
				finalQuat = math::normalize(finalQuat);
				return math::affineToHomogeneous(finalQuat.toAffine());
			}
		}
		return math::float4x4::identity();
	}

	math::float4x4 GAnimation::interpolateScale(GuGuUtf8Str channelName, float frame)
	{
		GuGuUtf8Str currentChannelName = channelName;
		//find channel(todo:fix)
		//auto it = std::find_if(m_channels.begin(), m_channels.end(), [=](const Channel& a) {
		//	if (a.m_name == currentChannelName)
		//		return true;
		//	return false;
		//});
		int32_t findIndex = -1;
		for (int32_t i = 0; i < m_channels.size(); ++i)
		{
			if (m_channels[i].m_name == currentChannelName)
			{
				findIndex = i;
				break;
			}
		}
		if (findIndex != -1)
		{
			Channel channel = m_channels[findIndex];
			if (channel.m_scales.size() == 1)
			{
				return math::affineToHomogeneous(math::translation(channel.m_scales[0].m_scale));
			}
			else
			{
				int32_t frameIndex1 = getScaleIndex(channel.m_scales, frame);
				int32_t frameIndex2 = frameIndex1 + 1;
				float scaleFactor = getScaleFactor(channel.m_scales[frameIndex1].m_timestamp,
					channel.m_scales[frameIndex2].m_timestamp, frame);
				math::float3 finalScale = math::lerp(channel.m_scales[frameIndex1].m_scale, channel.m_scales[frameIndex2].m_scale, scaleFactor);
				return math::affineToHomogeneous(math::scaling(finalScale));
			}
		}
		return math::float4x4::identity();
	}

	int32_t GAnimation::getPositionIndex(Array<KeyPosition>& keyPositions, float frame)
	{
		for (int32_t index = 0; index < keyPositions.size() - 1; ++index)
		{
			if (frame < keyPositions[index + 1].m_timestamp)
				return index;
		}
		return 0;
	}

	int32_t GAnimation::getRotationIndex(Array<KeyRotation>& keyRotations, float frame)
	{
		for (int32_t index = 0; index < keyRotations.size() - 1; ++index)
		{
			if (frame < keyRotations[index + 1].m_timestamp)
				return index;
		}
		return 0;
	}

	int32_t GAnimation::getScaleIndex(Array<KeyScale>& keyScales, float frame)
	{
		for (int32_t index = 0; index < keyScales.size() - 1; ++index)
		{
			if (frame < keyScales[index + 1].m_timestamp)
				return index;
		}
		return 0;
	}

	float GAnimation::getScaleFactor(float lastTimeStamp, float nextStamp, float frame)
	{
		float scaleFactor = 0.0f;
		float midWayLength = frame - lastTimeStamp;
		float framesDiff = nextStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

}