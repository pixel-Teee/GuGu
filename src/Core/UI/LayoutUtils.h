#pragma once

#include "BasicElement.h"
#include "WidgetGeometry.h"

namespace GuGu {
	struct AlignmentArrangeResult
	{
		AlignmentArrangeResult(float inOffset, float inSize)
			: m_offset(inOffset), m_size(inSize)
		{}
		float m_offset;
		float m_size;
	};

	namespace ArrangeUtils
	{
		template<Orientation orientation>
		struct GetChildAlignment
		{
			template<typename SlotType>
			static int32_t AsInt(const SlotType& inSlot);
		};

		template<>
		struct GetChildAlignment<Orientation::Horizontal>
		{
			template<typename SlotType>
			static int32_t AsInt(const SlotType& inSlot)
			{
				return static_cast<int32_t>(inSlot.getHorizontalAlignment());
			}
		};

		template<>
		struct GetChildAlignment<Orientation::Vertical>
		{
			template<typename SlotType>
			static int32_t AsInt(const SlotType& inSlot)
			{
				return static_cast<int32_t>(inSlot.getVerticalAlignment());
			}
		};
	}

	template<Orientation orientation, typename SlotType>
	static AlignmentArrangeResult AlignChild(const SlotType& childToArrange, float allocatedSize)
	{
		Padding slotPadding = childToArrange.getPadding();

		//horizontal
		float totalMargin = slotPadding.getTotalSpaceAlong<orientation>();
		float marginPre = (orientation == Orientation::Horizontal) ? slotPadding.left : slotPadding.top;
		float marginPost = (orientation == Orientation::Horizontal) ? slotPadding.right : slotPadding.bottom;

		//HorizontalAlignment alignment = childToArrange->getHorizontalAlignment();
		const uint32_t alignment = ArrangeUtils::GetChildAlignment<orientation>::AsInt(childToArrange);

		float offset = 0;
		float size = 0;

		if (alignment == (uint32_t)HorizontalAlignment::Stretch)
		{
			offset = marginPre;
			size = std::max((allocatedSize - totalMargin) * 1.0, 0.0);//todo:multiple scale
		}
		else
		{
			float childFixedSize = (orientation == Orientation::Horizontal) ? (childToArrange.getChildWidget()->getFixedSize().x) : (childToArrange.getChildWidget()->getFixedSize().y);

			//child actual size
			const float childSize = std::max(std::min(childFixedSize, allocatedSize - totalMargin), 0.0f);

			if (alignment == (uint32_t)HorizontalAlignment::Left)
			{
				offset = marginPre;
				size = childSize;
			}
			else if (alignment == (uint32_t)HorizontalAlignment::Center)
			{
				offset = (allocatedSize - childSize) / 2.0f + marginPre - marginPost;
				size = childSize;
			}
			else if (alignment == (uint32_t)HorizontalAlignment::Right)
			{
				offset = allocatedSize - childSize - marginPost;
				size = childSize;
			}
		}

		return AlignmentArrangeResult(offset, size);
	}
	/*
	* inAnchor 相对于 pop up 被创建的区域 (例如，combo box 的按钮部分)
	* pop up rect (pop up) 被建议的区域，位置可能被调整
	* orientation 我们尝试去展示 pop up 相对于 anchor 的上下或者左右？
	* rect to fit 可用的区域对于展示这个 pop up ，我们想完整地包含它，而不裁剪
	* 
	* 返回一个最佳的在 rect to fit 的位置，那么就没有 pop up 的裁剪超出 rect to fit 的区域
	*/
	math::float2 computePopupFitInRect(const math::box2& inAnchor,
		const math::box2& popupRect,
		const Orientation& orientation,
		const math::box2& rectToFit);
}