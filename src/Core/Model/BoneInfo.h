#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	struct BoneInfo
	{
		GuGuUtf8Str m_boneName;//bone name

		math::float4x4 m_offsetMatrix = math::float4x4::identity();

		int32_t m_boneId;
	};
}