#pragma once

#include <Core/Math/MyMath.h>

#include <Core/GuGuUtf8Str.h>

#include <Core/UI/BasicElement.h>

#include <functional>

namespace GuGu {
	//class GuGuUtf8Str;
	class SceneGraphNode;
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

		math::float3 color = math::float3(1.0f, 1.0f, 1.0f);

		//node name trees
		std::vector<GuGuUtf8Str> nodeNames;

		std::function<void(GuGuUtf8Str, std::vector<GuGuUtf8Str>&)> getNodeChildrens;

		std::function<void(GuGuUtf8Str, SelectInfo::Type)> selectionChanged;

		std::shared_ptr<SceneGraphNode> m_currentSelectItems;
	};
}