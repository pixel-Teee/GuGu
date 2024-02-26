#include <pch.h>

#include "WindowWidget.h"

#include "Slot.h"

#include "ElementList.h"

#include "StyleSet.h"

#include "ArrangedWidgetArray.h"
#include "ArrangedWidget.h"

namespace GuGu {
	WindowWidget::WindowWidget()
		: m_defaultBrush(StyleSet::getStyle()->getBrush("CheckerBoard"))
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
	uint32_t WindowWidget::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray;
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		if (m_windowType != WindowType::NativeWindow)
		{
			ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_defaultBrush, layer); //background
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
				
				maxLayer = std::max(maxLayer, widget->GenerateElement(elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return maxLayer;
	}
	math::double2 WindowWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return m_childWidget->getChildWidget()->getFixedSize();
	}

	void WindowWidget::AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();
		//check this is one
		//assert(slotNumber == 1);

		if (slotNumber)
		{
			Padding slotPadding = getSlot(0)->getPadding();

			//horizontal
			float totalMarginHorizontal = slotPadding.left + slotPadding.right;
			float marginPreHorizontal = slotPadding.left;
			float marginPostHorizontal = slotPadding.right;

			HorizontalAlignment alignment = getSlot(0)->getHorizontalAlignment();

			float offsetHorizontal = 0;
			float sizeHorizontal = 0;

			if (alignment == HorizontalAlignment::Stretch)
			{
				offsetHorizontal = marginPreHorizontal;
				sizeHorizontal = std::max((allocatedGeometry.getLocalSize().x - totalMarginHorizontal) * 1.0, 0.0);//todo:multiple scale
			}
			else
			{
				float childFixedSize = getSlot(0)->getChildWidget()->getFixedSize().x;

				//child actual size
				const float childSize = std::max(std::min(childFixedSize, (float)allocatedGeometry.getLocalSize().x - totalMarginHorizontal), 0.0f);

				if (alignment == HorizontalAlignment::Left)
				{
					offsetHorizontal = marginPreHorizontal;
					sizeHorizontal = childSize;
				}
				else if (alignment == HorizontalAlignment::Center)
				{
					offsetHorizontal = (allocatedGeometry.getLocalSize().x - childSize) / 2.0f + marginPreHorizontal - marginPostHorizontal;
					sizeHorizontal = childSize;
				}
				else if (alignment == HorizontalAlignment::Right)
				{
					offsetHorizontal = allocatedGeometry.getLocalSize().x - childSize - marginPostHorizontal;
					sizeHorizontal = childSize;
				}
			}

			//vertical
			float totalMarginVertical = slotPadding.top + slotPadding.bottom;
			float marginPreVertical = slotPadding.top;
			float marginPostVertical = slotPadding.bottom;

			VerticalAlignment verticalAlignment = getSlot(0)->getVerticalAlignment();

			float offsetVertical = 0;
			float sizeVertical = 0;

			if (verticalAlignment == VerticalAlignment::Stretch)
			{
				offsetVertical = marginPreVertical;
				sizeVertical = std::max((allocatedGeometry.getLocalSize().y - totalMarginVertical) * 1.0, 0.0);
			}
			else
			{
				float childFixedSize = getSlot(0)->getChildWidget()->getFixedSize().y;

				//child actual size
				const float childSize = std::max(std::min(childFixedSize, (float)allocatedGeometry.getLocalSize().y - totalMarginVertical), 0.0f);

				if (verticalAlignment == VerticalAlignment::Top)
				{
					offsetVertical = marginPreVertical;
					sizeVertical = childSize;
				}
				else if (verticalAlignment == VerticalAlignment::Center)
				{
					offsetVertical = (allocatedGeometry.getLocalSize().y - childSize) / 2.0f + marginPreVertical - marginPostVertical;
					sizeVertical = childSize;
				}
				else if (verticalAlignment == VerticalAlignment::Bottom)
				{
					offsetVertical = allocatedGeometry.getLocalSize().y - childSize - marginPostVertical;
					sizeVertical = childSize;
				}
			}

			WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::double2(sizeHorizontal, sizeVertical), math::double2(offsetHorizontal, offsetVertical), allocatedGeometry.getAccumulateTransform());

			arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
		}
		
	}
	std::shared_ptr<Slot> WindowWidget::getSlot(uint32_t index)
	{
		return m_childWidget;
	}
	uint32_t WindowWidget::getSlotsNumber()
	{
		if (m_childWidget) return 1;
		return 0;
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
	void WindowWidget::setChildWidget(std::shared_ptr<Widget> widget)
	{
		std::shared_ptr<Slot> slot = std::make_shared<Slot>();
		slot->setChildWidget(widget);
		//slot->setHorizontalAlignment(HorizontalAlignment::Stretch);
		//slot->setVerticalAlignment(VerticalAlignment::Stretch);
		m_childWidget = slot;
	}
	std::shared_ptr<Window> WindowWidget::getNativeWindow()
	{
		std::shared_ptr<Window> nativeWindow = m_nativeWindow.lock();
		return nativeWindow;
	}
}