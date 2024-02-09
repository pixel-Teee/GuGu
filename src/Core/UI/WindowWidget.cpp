#include <pch.h>

#include "WindowWidget.h"

#include "Slot.h"

#include "ElementList.h"

namespace GuGu {
	WindowWidget::WindowWidget()
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
	void WindowWidget::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry)
	{
		//ElementList::addBoxElement(elementList, );
	}
	math::double2 WindowWidget::ComputeFixedSize()
	{
		return m_childWidget->getChildWidget()->getFixedSize();
	}
	void WindowWidget::AllocationChildActualSpace()
	{
	}
}