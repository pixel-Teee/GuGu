#include <pch.h>

#include "KeyframeAnimation.h"

namespace GuGu {
	namespace animation {
		math::float4 Interpolate(InterpolationMode mode, const Keyframe& a, const Keyframe& b, const Keyframe& c, const Keyframe& d, float t, float dt)
		{
			switch (mode)
			{
			case InterpolationMode::Step:
				return b.value;

			case InterpolationMode::Linear:
				return lerp(b.value, c.value, t);

			case InterpolationMode::Slerp: {
				math::quat qb = math::quat::fromXYZW(b.value);
				math::quat qc = math::quat::fromXYZW(c.value);
				math::quat qr = slerp(qb, qc, t);
				return math::float4(qr.x, qr.y, qr.z, qr.w);
			}

			case InterpolationMode::CatmullRomSpline: {
				// https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Interpolation_on_the_unit_interval_with_matched_derivatives_at_endpoints
				// a = p[n-1], b = p[n], c = p[n+1], d = p[n+2]
				math::float4 i = -a.value + 3.f * b.value - 3.f * c.value + d.value;
				math::float4 j = 2.f * a.value - 5.f * b.value + 4.f * c.value - d.value;
				math::float4 k = -a.value + c.value;
				return 0.5f * ((i * t + j) * t + k) * t + b.value;
			}

			case InterpolationMode::HermiteSpline: {
				// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-c-spline-interpolation
				const float t2 = t * t;
				const float t3 = t2 * t;
				return (2.f * t3 - 3.f * t2 + 1.f) * b.value
					+ (t3 - 2.f * t2 + t) * b.outTangent * dt
					+ (-2.f * t3 + 3.f * t2) * c.value
					+ (t3 - t2) * c.inTangent * dt;
			}

			default:
				assert(!"Unknown interpolation mode");
				return b.value;
			}
		}
		std::optional<dm::float4> Sampler::Evaluate(float time, bool extrapolateLastValues) const
		{
			const size_t count = m_keyFrames.size();

			if (count == 0)
				return std::optional<math::float4>();

			if (time <= m_keyFrames[0].time)
				return std::optional(m_keyFrames[0].value);

			if (m_keyFrames.size() == 1 || time >= m_keyFrames[count - 1].time)
			{
				if (extrapolateLastValues)
					return std::optional(m_keyFrames[count - 1].value);
				else
					return std::optional<math::float4>();
			}

			for (size_t offset = 0; offset < count; offset++)
			{
				const float tb = m_keyFrames[offset].time;
				const float tc = m_keyFrames[offset + 1].time;

				if (tb <= time && time < tc)
				{
					const Keyframe& b = m_keyFrames[offset];
					const Keyframe& c = m_keyFrames[offset + 1];
					const Keyframe& a = (offset > 0) ? m_keyFrames[offset - 1] : b;
					const Keyframe& d = (offset < count - 2) ? m_keyFrames[offset + 2] : c;
					const float dt = tc - tb;
					const float u = (time - tb) / dt;

					math::float4 y = Interpolate(m_mode, a, b, c, d, u, dt);

					return std::optional(y);
				}
			}

			// shouldn't get here if the keyframes are properly ordered in time
			return std::optional<math::float4>();
		}
		void Sampler::AddKeyframe(const Keyframe keyframe)
		{
			m_keyFrames.push_back(keyframe);
		}
		float Sampler::GetEndTime() const
		{
			if (!m_keyFrames.empty())
				return m_keyFrames[m_keyFrames.size() - 1].time;
			return 0.0f;
		}
	}

}