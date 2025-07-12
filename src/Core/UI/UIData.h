#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GuGuUtf8Str.h>
#include <Core/UI/BasicElement.h>
#include <Renderer/Color.h>

#include <functional>

namespace GuGu {
	//class GuGuUtf8Str;
	class SceneGraphNode;

	struct GridProperties
	{
		float _planeAxis = 1.0f;
		float _majorGridDivisions = 10.0f;
		float _axisLineWidth = 0.04f;
		float _majorLineWidth = 0.02f;
		float _minorLineWidth = 0.01f;
		math::float3 pad0;
		math::float4 _baseColor = math::float4(0.0f, 0.0f, 0.0f, 0.0f);
		math::float4 _majorLineColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
		math::float4 _minorLineColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
		math::float4 _xAxisLineColor = math::float4(1.0f, 0.0f, 0.0f, 1.0f);
		math::float4 _xAxisDashColor = math::float4(0.5f, 0.0f, 0.0f, 1.0f);
		math::float4 _yAxisLineColor = math::float4(0.0f, 1.0f, 0.0f, 1.0f);;
		math::float4 _yAxisDashColor = math::float4(0.0f, 0.5f, 0.0f, 1.0f);;
		math::float4 _zAxisLineColor = math::float4(0.0f, 0.0f, 1.0f, 1.0f);;
		math::float4 _zAxisDashColor = math::float4(0.0f, 0.0f, 0.5f, 1.0f);;
		float _axisDashScale = 1.33;
		math::float3 pad1;
		math::float4 _axisCenterColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
	};

	struct UIData
	{
		float metallic = 0.0f;
		float roughness = 0.0f;

		UIData() {}

		float camPos = -25.0f;
		float dir = 0.0f;

		float xWorldPos = 0.0f;
		float yWorldPos = 0.0f;
		float zWorldPos = 0.0f;

		Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		GridProperties m_gridProperties;

		//node name trees
		std::vector<GuGuUtf8Str> nodeNames;

		std::function<void(GuGuUtf8Str, std::vector<GuGuUtf8Str>&)> getNodeChildrens;

		std::function<void(GuGuUtf8Str, SelectInfo::Type)> selectionChanged;

		std::shared_ptr<SceneGraphNode> m_currentSelectItems;
	};
}