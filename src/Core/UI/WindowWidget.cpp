#include <pch.h>

#include "WindowWidget.h"

#include "Slot.h"

#include "ElementList.h"

#include "Style.h"

namespace GuGu {
	WindowWidget::WindowWidget()
		: m_defaultBrush(Style::getStyle()->getBrush("CheckerBoard"))
	{
		m_windowType = WindowType::VirtualWindow;
	}
	WindowWidget::~WindowWidget()
	{
	}
	void WindowWidget::assocateWithNativeWindow(std::shared_ptr<Window> nativeWindow)
	{
		m_nativeWindow = nativeWindow;
		m_windowType = WindowType::NativeWindow;
	}
	void WindowWidget::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		if (m_windowType != WindowType::NativeWindow)
		{
			ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_defaultBrush, layer); //background
		}
		
		uint32_t widgetNumbers = getSlotsNumber();
		math::double2 size = math::double2(0.0, 0.0);
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<Slot> childSlot = getSlot(i);
			if (childSlot)
			{
				std::shared_ptr<Widget> childWidget = childSlot->getChildWidget();
				
				childWidget->GenerateElement(elementList, allocatedGeometry, layer + 1);
			}
		}
	}
	math::double2 WindowWidget::ComputeFixedSize()
	{
		return m_childWidget->getChildWidget()->getFixedSize();
	}
	void WindowWidget::AllocationChildActualSpace()
	{
	}
	std::shared_ptr<Slot> WindowWidget::getSlot(uint32_t index)
	{
		return m_childWidget;
	}
	uint32_t WindowWidget::getSlotsNumber()
	{
		return 1;
	}
	math::int2 WindowWidget::getViewportSize()
	{
		//todo:check this window type is native window
		if (m_windowType == WindowType::NativeWindow)
		{
			std::shared_ptr<Window> window = m_nativeWindow.lock();
			if (window)
			{
				//math::int2 widthAndHeight = 
				return math::int2(0, 0);
			}
		}
		return math::int2(0, 0);
	}
}