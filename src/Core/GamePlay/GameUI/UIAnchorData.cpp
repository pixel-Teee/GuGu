#include <pch.h>

#include "UIAnchorData.h"

namespace GuGu {

	UIAnchorData::UIAnchorData()
	{
		m_anchors = UIAnchors(math::float2(0, 0), math::float2(1, 1));
		m_offset = UIPadding(0, 0, 0, 0);
		m_alignment = math::float2(0, 0);//pivot
	}

	UIAnchorData::UIAnchorData(UIAnchors inAnchors, UIPadding inOffset, math::float2 inAlignment)
	{
		m_anchors = inAnchors;
		m_offset = inOffset;
		m_alignment = inAlignment;
	}

	UIPadding& UIAnchorData::getOffset()
	{
		return m_offset;
	}

	void UIAnchorData::setOffset(UIPadding inOffset)
	{
		m_offset = inOffset;
	}

}