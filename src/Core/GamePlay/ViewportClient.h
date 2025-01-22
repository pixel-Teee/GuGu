#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class ViewportClient
	{
	public:
		enum ViewportState
		{
			Editor,
			Runtime
		};
		ViewportClient();

		virtual ~ViewportClient();

		virtual math::affine3 getWorldToViewMatrix() const = 0;

		virtual math::float3 getCamPos() const = 0;

		virtual float getFov() const = 0;

		virtual void update(const float inDeltaTime) = 0;
	};
}