#include <pch.h>

#include "Button.h"

#include "ElementList.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

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
		m_widgetClipping = arguments.mClip;
	}
	uint32_t Button::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray;
		if (m_childWidget) //todo:add null widget
		{
			Border::AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);
		}	

		ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_imageBursh.Get(), layer); //background

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
	math::double2 Button::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::double2(m_imageBursh.Get()->m_actualSize.x, m_imageBursh.Get()->m_actualSize.y);
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
		return Reply::Unhandled();
	}

	Reply Button::OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		Release();
		return Reply::Unhandled();
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