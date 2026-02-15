#pragma once

namespace GuGu {
	namespace math
	{
		struct Rotator {
			float pitch; //x
			float yaw; //y
			float roll; //z
			Rotator() { yaw = pitch = roll = 0; }
			Rotator(float inPitch, float inYaw, float inRoll) : yaw(inYaw), pitch(inPitch), roll(inRoll) {}
			Rotator(math::float3 inFloat3) { pitch = inFloat3.x; yaw = inFloat3.y; roll = inFloat3.z; }
			Rotator(math::double3 inDouble3) { pitch = inDouble3.x; yaw = inDouble3.y; roll = inDouble3.z; }
			//get vector
			math::float3 getFloat3() const { return math::float3(pitch, yaw, roll); }
			math::double3 getDouble3() const { return math::double3(pitch, yaw, roll); }
			math::double3 getRadians() const;
		};
	}
}