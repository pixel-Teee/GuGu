#pragma once

#include <Math/MyMath.h>
#include <nvrhi.h>
#include <Renderer/Color.h> //color

namespace GuGu {
	struct GameUIVertex
	{
		math::float2 m_uv;
		math::float3 m_position;
		Color m_color;

		GameUIVertex(math::float2 inUV, math::float3 inPosition, Color inColor)
			: m_uv(inUV), m_position(inPosition), m_color(inColor)
		{}
	};
	class GTexture;

	//draw type
	enum class UIDrawType
	{
		Default,
		Font
	};

	struct UIDrawInfo
	{
		//zorder
		float m_zorder;
		std::vector<GameUIVertex> m_uiVertex;
		std::vector<uint32_t> m_uiIndices;
		std::shared_ptr<GTexture> m_texture;
		nvrhi::BufferHandle m_uiVertexHandle;
		nvrhi::BufferHandle m_uiIndexHandle;
		UIDrawType m_drawType = UIDrawType::Default;
	};
}