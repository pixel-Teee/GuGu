#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	namespace animation {
		struct Keyframe
		{
			float time = 0.f;
			math::float4 value = 0.f;
			math::float4 inTangent = 0.f;
			math::float4 outTangent = 0.f;
		};

		enum class InterpolationMode
		{
			Step,
			Linear,
			Slerp,
			CatmullRomSpline,
			HermiteSpline
		};

		math::float4 Interpolate(InterpolationMode mode,
			const Keyframe& a, const Keyframe& b,
			const Keyframe& c, const Keyframe& d, float t, float dt);

		class Sampler {
		public:
			Sampler() = default;

			virtual ~Sampler() = default;

			void SetInterpolationMode(InterpolationMode mode) { m_mode = mode; }

			std::optional<dm::float4> Evaluate(float time, bool extrapolateLastValues = false) const;

			std::vector<Keyframe>& GetKeyframes() { return m_keyFrames; }
			void AddKeyframe(const Keyframe keyframe);

			float GetEndTime() const;
		protected:
			std::vector<Keyframe> m_keyFrames;
			InterpolationMode m_mode = InterpolationMode::Step;
		};
	}
}