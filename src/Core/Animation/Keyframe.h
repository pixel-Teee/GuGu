#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	//key frame
	struct KeyPosition
	{
		math::float3 m_position;
		float m_timestamp;
	};

	struct KeyRotation
	{
		math::quat m_orientation;
		float m_timestamp;
	};

	struct KeyScale
	{
		math::float3 m_scale;
		float m_timestamp;
	};

	//一个轨道，记录一个骨骼或者一个物体的持续时间里面的所有关键帧
	struct Channel
	{
		Array<KeyPosition> m_positions;
		Array<KeyRotation> m_rotations;
		Array<KeyScale> m_scales;

		GuGuUtf8Str m_name;//对应骨骼名字
	};
}