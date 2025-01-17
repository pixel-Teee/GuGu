#pragma once

#include "DragDropOperation.h"
#include <Core/Math/MyMath.h>
#include "Brush.h"

namespace GuGu {
	//class Brush;
	class DecoratedDragDropOp : public DragDropOperation
	{
	public:
		DRAG_DROP_OPERATOR_TYPE(DecoratedDragDropOp, DragDropOperation)

		GuGuUtf8Str m_currentHoverText;

		std::shared_ptr<Brush> m_currentIconBrush;

		math::float4 m_currentIconColorAndOpacity;

		DecoratedDragDropOp()
		: m_currentIconBrush(nullptr)
		, m_currentIconColorAndOpacity(math::float4(1.0f, 1.0f, 1.0f, 1.0f)) //white
		, m_defaultIconBrush(nullptr)
		, m_defaultIconColorAndOpacity(math::float4(1.0f, 1.0f, 1.0f, 1.0f)) //white
		{}

		virtual void construct() override
		{
			DragDropOperation::construct();
		}

		std::shared_ptr<Brush> getIcon() const;

		GuGuUtf8Str getHoverText() const;

		virtual std::shared_ptr<Widget> getDefaultDecorator() const override;

		virtual void resetToDefaultToolTip();
	protected:
		GuGuUtf8Str m_defaultHoverText;

		std::shared_ptr<Brush> m_defaultIconBrush;

		math::float4 m_defaultIconColorAndOpacity;
	};
}