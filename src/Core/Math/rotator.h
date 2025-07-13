#pragma once

namespace GuGu {
	namespace math
	{
		struct Rotator {
			float yaw;//z
			float pitch;//y
			float roll;//x
			Rotator() { yaw = pitch = roll = 0; }
			Rotator(float inYaw, float inPitch, float inRoll) : yaw(inYaw), pitch(inPitch), roll(inRoll) {}
			Rotator(math::float3& inFloat3) { roll = inFloat3.x, pitch = inFloat3.y, yaw = inFloat3.z; }
			Rotator(math::double3& inDouble3) { roll = inDouble3.x, pitch = inDouble3.y, yaw = inDouble3.z; }
			//get vector
			math::float3 getFloat3() const { return math::float3(roll, pitch, yaw); }
			math::double3 getDouble3() const { return math::double3(roll, pitch, yaw); }
			math::double3 getRadians() const;
		};
	}
}