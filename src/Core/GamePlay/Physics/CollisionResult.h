#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	struct CollisionResult
	{
	public:
		CollisionResult();

		~CollisionResult();

		math::float3 m_hitNormal;
		math::float3 m_hitPosition;
		bool m_bHaveResult;
	};
}

