#include <pch.h>

#include "Button.h"

#include "ElementList.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "NullWidget.h"

namespace GuGu {
	Button::Button()
	: m_bIsPressed(false)
	{
	}
	Button::~Button()
	{
	}
	void Button::init(const BuilderArguments& arguments)
	{
		m_buttonStyle = arguments.mbuttonSyle;
		m_clicked = arguments.mClicked;
		m_contentPadding = arguments.mcontentPadding;
		m_imageBursh = m_buttonStyle->m_normal;
		m_bIsFocusable = arguments.misFocusable;
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;

		m_childWidget = arguments.mContent;	
		m_childWidget->m_parentWidget = shared_from_this();
		if(m_childWidget->m_childWidget != NullWidget::getNullWidget())
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}
	uint32_t Button::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		if (m_childWidget) //todo:add null widget
		{
			Border::AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);
		}	

		if (m_imageBursh.Get() && m_imageBursh.Get()->m_drawAs != BrushDrawType::NoDrawType)
		{
			ElementList::addBoxElement(elementList, allocatedGeometry, m_imageBursh.Get()->m_tintColor, m_imageBursh.Get(), layer); //background
		}
		
		uint32_t widgetNumbers = arrangedWidgetArray.getArrangedWidgetsNumber();//note:just one
		//math::double2 size = math::double2(0.0, 0.0);
		uint32_t maxLayer = 0;
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(i);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return maxLayer;
	}
	GuGu::math::float2 Button::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_childWidget->getChildWidget() == NullWidget::getNullWidget())
		{
			return math::float2(m_imageBursh.Get()->m_actualSize.x, m_imageBursh.Get()->m_actualSize.y);
		}	
		else
		{
			return Border::ComputeFixedSize(inLayoutScaleMultiplier);
		}
		return math::float2(0, 0);
	}

	Reply Button::OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		//update button state
		Press();
		if (m_clicked)
		{
			m_clicked();
		}
        GuGu_LOGD("%s", "click button");
		return Reply::Unhandled().captureMouse(shared_from_this());
	}

	Reply Button::OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		Release();
		return Reply::Unhandled().releaseMouseCapture();
	}

	void Button::OnMouseEnter(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		const bool bWasHovered = IsHovered();

		Border::OnMouseEnter(myGeometry, inMouseEvent);//设置悬浮状态
		updateBorderImage();

		if (!bWasHovered && IsHovered())
		{
			
			//executeHoverStateChanged();
		}
	}

	void Button::OnMouseLeave(const PointerEvent& inMouseEvent)
	{
		Border::OnMouseLeave(inMouseEvent);
		updateBorderImage();
	}

	bool Button::supportsKeyboardFocus() const
	{
		return m_bIsFocusable;
	}

	bool Button::isInteractable() const
	{
		return true;
	}

	void Button::executeHoverStateChanged()
	{
		if (IsHovered())
		{
			//todo:add OnHovered
		}
		else
		{
			//todo:add OnUnhovered
		}
	}

	void Button::updateBorderImage()
	{
		if (m_bIsPressed)
		{
			setBorderImage(m_buttonStyle->m_pressed);
		}
		else if(IsHovered())
		{
			setBorderImage(m_buttonStyle->m_hovered);
		}
		else
		{
			setBorderImage(m_buttonStyle->m_normal);
		}
	}

	void Button::setOnClicked(OnClicked inOnClicked)
	{
		m_clicked = inOnClicked;
	}

	void Button::Press()
	{
		if (!m_bIsPressed)
		{
			m_bIsPressed = true;
			//update image
			m_imageBursh = m_buttonStyle->m_pressed;
		}
	}

	void Button::Release()
	{
		if (m_bIsPressed)
		{
			m_bIsPressed = false;
			//update image
			m_imageBursh = m_buttonStyle->m_normal;
		}
	}

}