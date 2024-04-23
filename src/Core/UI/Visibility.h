#pragma once

#include "EnumAsByte.h"

namespace GuGu {
	struct Visibility
	{
		//可见的并且可碰撞的(可以和光标交互)，默认值
		static const Visibility Visible;
		//不可见并且在布局中不占据空间(明显的不可碰撞)
		static const Visibility Collapsed;
		//不可见但是占据布局空间(明显的不可碰撞)
		static const Visibility Hidden;
		//可见的，但是不可以碰撞的(不能和光标交互)并且在层级中的儿子也不可以碰撞
		static const Visibility HitTestInvisible;
		//可见的，但是不可以碰撞的(不能和光标交互)并且不影响儿子的碰撞测试
		static const Visibility SelfHitTestInvisible;
		//所有的可见性
		static const Visibility All;

		Visibility()
			: m_value(VIS_Visible)
		{}

	public:
		bool operator==(const Visibility& other) const
		{
			return this->m_value == other.m_value;
		}

		bool operator!=(const Visibility& other) const
		{
			return this->m_value != other.m_value;
		}

	public:
		bool areChildrenHitTestVisible() const
		{
			return 0 != (m_value & VISPRIVATE_ChildrenHitTestisible);
		}

		bool isHitTestVisible() const
		{
			return 0 != (m_value & VISPRIVATE_SelfHitTestVisible);
		}

		bool isVisible() const
		{
			return 0 != (m_value & VIS_Visible);
		}

		static bool doesVisibilityPassFilter(const Visibility inVisiblity, const Visibility inVisibilityFilter)
		{
			return 0 != (inVisiblity.m_value & inVisibilityFilter.m_value);
		}

	public:
		enum Private
		{
			VISPRIVATE_Visible = 0x1 << 0,
			VISPRIVATE_Collapsed = 0x1 << 1,
			VISPRIVATE_Hidden = 0x1 << 2,
			VISPRIVATE_SelfHitTestVisible = 0x1 << 3,
			VISPRIVATE_ChildrenHitTestisible = 0x1 << 4,

			VIS_Visible = VISPRIVATE_Visible | VISPRIVATE_SelfHitTestVisible | VISPRIVATE_ChildrenHitTestisible,

			VIS_Collapsed = VISPRIVATE_Collapsed,

			VIS_Hidden = VISPRIVATE_Hidden,

			VIS_HitTestInvisible = VISPRIVATE_Visible,

			VIS_SelfHiTestInvsible = VISPRIVATE_Visible | VISPRIVATE_ChildrenHitTestisible,

			VIS_All = VISPRIVATE_Visible | VISPRIVATE_Hidden | VISPRIVATE_Collapsed | VISPRIVATE_SelfHitTestVisible | VISPRIVATE_ChildrenHitTestisible
		};

		//私有构造
		Visibility(Private inValue)
			: m_value(inValue)
		{}
	
		//将Private枚举值按uin8_t存储起来
		EnumAsByte<Private> m_value;
	};
}