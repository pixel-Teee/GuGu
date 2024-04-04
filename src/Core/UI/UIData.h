#pragma once

namespace GuGu {
	struct UIData
	{
		float metallic = 0.0f;
		float roughness = 0.0f;

		UIData() {}

		float camPos = -10.0f;
		float dir = 0.0f;

		float xWorldPos = 0.0f;
		float yWorldPos = 0.0f;
		float zWorldPos = 0.0f;
	};
}