#pragma once

#include "UIAnchors.h"
#include "UIPadding.h"
#include <Core/Math/MyMath.h>

namespace GuGu {
	struct UIAnchorData
	{
		UIAnchors m_anchors; //anchor

		UIPadding m_offset; //offset

		math::float2 m_alignment;

		UIAnchorData();

		UIAnchorData(UIAnchors inAnchors, UIPadding inOffset, math::float2 inAlignment);
	};
}