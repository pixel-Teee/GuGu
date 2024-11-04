#pragma once

#include <Core/Math/MyMath.h>

#include "Attribute.h"

namespace GuGu {
	namespace ButtonClickMethod
	{
		enum Type
		{
			//必须按下和释放按钮，才能触发回调
			DownAndUp,

			MouseDown,

			MouseUp
		};
	}
	struct OptionalSize
	{
	public:
		OptionalSize()
			: m_size(unspecified)
		{}

		OptionalSize(const float specifiedSize)
			: m_size(specifiedSize)
		{}

		bool isSet() const
		{
			return m_size != unspecified;
		}

		float get() const
		{
			return m_size;
		}

		bool operator==(const OptionalSize& other) const
		{
			return (m_size == other.m_size);
		}

	private:
		static const float unspecified;

		float m_size;
	};
	enum class HorizontalAlignment
	{
		Left,
		Center,
		Right,
		Stretch
	};

	enum class VerticalAlignment
	{
		Top,
		Center,
		Bottom,
		Stretch
	};

	enum class Orientation
	{
		Horizontal,
		Vertical
	};

	struct Padding
	{
		float left;
		float top;
		float right;
		float bottom;
		Padding() { left = top = right = bottom = 0.0f; }
		Padding(float inLeft, float inTop, float inRight, float inBottom)
			: left(inLeft), top(inTop), right(inRight), bottom(inBottom)
		{}
		Padding(float uniformPadding)
			: left(uniformPadding)
			, top(uniformPadding)
			, right(uniformPadding)
			, bottom(uniformPadding)
		{}

		math::float2 getFixedSize() const
		{
			return math::float2(left + right, top + bottom);
		}

		template<Orientation orientation>
		float getTotalSpaceAlong() const
		{
			return 0.0f;
		}
	};
	template<>
	inline float Padding::getTotalSpaceAlong<Orientation::Horizontal>() const { return left + right; }

	template<>
	inline float Padding::getTotalSpaceAlong<Orientation::Vertical>() const { return top + bottom; }
	//typedef math::double4 Padding;

	struct UIVertex
	{
		math::float4 textureCoordinate;
		math::float2 position;
		math::float4 color;
		math::float4 secondaryColor;
		UIVertex(math::float4 inTextureCoordinate, math::float2 inPosition, math::float4 inColor, math::float4 inSecondaryColor)
			: textureCoordinate(inTextureCoordinate), position(inPosition), color(inColor), secondaryColor(inSecondaryColor)
		{}
	};

	struct SizeParam
	{
		enum SizeRule
		{
			Fixed,
			Stretch
		};

		SizeRule m_sizeRule;

		Attribute<float> m_value;

	public:
		SizeParam(SizeRule inTypeOfSize, const Attribute<float>& inValue)
			: m_sizeRule(inTypeOfSize)
			, m_value(inValue)
		{}
	};

	struct Stretch : public SizeParam
	{
		Stretch(const Attribute<float>& stretchAmount)
			: SizeParam(SizeParam::Stretch, stretchAmount)
		{}

		Stretch() : SizeParam(SizeParam::Stretch, 1.0f)
		{}
	};

	struct Fixed : public SizeParam
	{
		Fixed() : SizeParam(SizeParam::Fixed, 0.0f)
		{}
	};

	enum class CheckBoxState : uint8_t
	{
		Unchecked,

		Checked
	};

	//文本提交方式
	namespace TextCommit
	{
		enum Type
		{
			/*失去焦点或者相似的事件造成隐式的提交*/
			Default,
			/*用户通过输入键提交*/
			OnEnter,
			/*用户通过tab或者显式地移动焦点进行提交*/
			OnUserMovedFocus,
			/*键盘焦点通过escape key 或者其他相似的操作清除焦点*/
			OnCleared
		};
	}

	/*
	* 如何绘制一个图片
	*/
	namespace BrushDrawType
	{
		enum Type
		{
			//不做任何事情
			NoDrawType,

			//绘制一个3x3 box ，边缘和中间会被拉伸基于 margin
			Box,

			//绘制一个3x3 border ，边缘会平铺，中间是空的
			Border,

			//绘制一个图片，margin是空的
			Image,

			//绘制一个solid rectangle ，带一个外描边和一个角落半径
			RoundedBox
		};
	}

	namespace SelectInfo
	{
		enum Type
		{
			OnKeyPress,

			OnNavigation,

			OnMouseClick,

			Direct
		};
	}

	//可能的放置枚举，对于 pop-up 菜单
	enum MenuPlacement
	{
		//立马将 menu 放置在 anchor 下面
		MenuPlacementBelowAnchor,

		//立马将 menu 放置在 anchor 中间下面
		MenuPlacementCenteredBelowAnchor,

		MenuPlacementBelowRightAnchor,

		MenuPlacementComboBox,

		MenuPlacementComboBoxRight,

		MenuPlacementMenuRight,

		MenuPlacementAboveAnchor,

		MenuPlacementCenteredAboveAnchor,

		MenuPlacementAboveRightAnchor,

		MenuPlacementMenuLeft,

		MenuPlacementCenter,

		MenuPlacementRightLeftCenter,

		MenuPlacementMatchBottomLeft
	};
}