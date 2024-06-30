#include <pch.h>

#include "WindowWidget.h"

#include "Slot.h"
#include "StyleSet.h"
#include "ElementList.h"
#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

#include <Window/Window.h>

#include <Renderer/DeviceManager.h>//IRenderPass

#include <Application/Application.h>

namespace GuGu {
	WindowWidget::WindowWidget()
		: m_defaultBrush(StyleSet::getStyle()->getBrush("CheckerBoard"))
	{
		m_windowType = WindowType::VirtualWindow;
	}
	WindowWidget::~WindowWidget()
	{
	}
	void WindowWidget::init(const BuilderArguments& arguments)
	{
		m_windowType = arguments.mType;
		m_childWidget = arguments.mContent;
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;

		m_screenPosition = arguments.mScreenPosition;
		//set size
		setCachedSize(arguments.mClientSize);
	}
	uint32_t WindowWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);//设置数组只接受可见的child widget
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

				maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return maxLayer;
	}
	GuGu::math::float2 WindowWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_childWidget)
		{
			const Visibility childVisiblity = m_childWidget->getChildWidget()->getVisibility();
			if (childVisiblity != Visibility::Collapsed)
			{
				return m_childWidget->getChildWidget()->getFixedSize() + m_childWidget->getPadding().getFixedSize();
			}
		}		
		return math::float2(0, 0);
	}
	void WindowWidget::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();
		//check this is one
		//assert(slotNumber == 1);

		if (slotNumber)
		{
			//Padding slotPadding = getSlot(0)->getPadding();
			//
			////horizontal
			//float totalMarginHorizontal = slotPadding.left + slotPadding.right;
			//float marginPreHorizontal = slotPadding.left;
			//float marginPostHorizontal = slotPadding.right;
			//
			//HorizontalAlignment alignment = getSlot(0)->getHorizontalAlignment();
			//
			//float offsetHorizontal = 0;
			//float sizeHorizontal = 0;
			//
			//if (alignment == HorizontalAlignment::Stretch)
			//{
			//	offsetHorizontal = marginPreHorizontal;
			//	sizeHorizontal = std::max((allocatedGeometry.getLocalSize().x - totalMarginHorizontal) * 1.0, 0.0);//todo:multiple scale
			//}
			//else
			//{
			//	float childFixedSize = getSlot(0)->getChildWidget()->getFixedSize().x;
			//
			//	//child actual size
			//	const float childSize = std::max(std::min(childFixedSize, (float)allocatedGeometry.getLocalSize().x - totalMarginHorizontal), 0.0f);
			//
			//	if (alignment == HorizontalAlignment::Left)
			//	{
			//		offsetHorizontal = marginPreHorizontal;
			//		sizeHorizontal = childSize;
			//	}
			//	else if (alignment == HorizontalAlignment::Center)
			//	{
			//		offsetHorizontal = (allocatedGeometry.getLocalSize().x - childSize) / 2.0f + marginPreHorizontal - marginPostHorizontal;
			//		sizeHorizontal = childSize;
			//	}
			//	else if (alignment == HorizontalAlignment::Right)
			//	{
			//		offsetHorizontal = allocatedGeometry.getLocalSize().x - childSize - marginPostHorizontal;
			//		sizeHorizontal = childSize;
			//	}
			//}
			//
			////vertical
			//float totalMarginVertical = slotPadding.top + slotPadding.bottom;
			//float marginPreVertical = slotPadding.top;
			//float marginPostVertical = slotPadding.bottom;
			//
			//VerticalAlignment verticalAlignment = getSlot(0)->getVerticalAlignment();
			//
			//float offsetVertical = 0;
			//float sizeVertical = 0;
			//
			//if (verticalAlignment == VerticalAlignment::Stretch)
			//{
			//	offsetVertical = marginPreVertical;
			//	sizeVertical = std::max((allocatedGeometry.getLocalSize().y - totalMarginVertical) * 1.0, 0.0);
			//}
			//else
			//{
			//	float childFixedSize = getSlot(0)->getChildWidget()->getFixedSize().y;
			//
			//	//child actual size
			//	const float childSize = std::max(std::min(childFixedSize, (float)allocatedGeometry.getLocalSize().y - totalMarginVertical), 0.0f);
			//
			//	if (verticalAlignment == VerticalAlignment::Top)
			//	{
			//		offsetVertical = marginPreVertical;
			//		sizeVertical = childSize;
			//	}
			//	else if (verticalAlignment == VerticalAlignment::Center)
			//	{
			//		offsetVertical = (allocatedGeometry.getLocalSize().y - childSize) / 2.0f + marginPreVertical - marginPostVertical;
			//		sizeVertical = childSize;
			//	}
			//	else if (verticalAlignment == VerticalAlignment::Bottom)
			//	{
			//		offsetVertical = allocatedGeometry.getLocalSize().y - childSize - marginPostVertical;
			//		sizeVertical = childSize;
			//	}
			//}

			const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

			if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
			{
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

				WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset));

				arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
			}		
		}

	}	
	SlotBase* WindowWidget::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t WindowWidget::getSlotsNumber() const
	{
		if (m_childWidget) return 1;
		return 0;
	}
	math::int2 WindowWidget::getViewportSize()
	{
		//todo:check this window type is native window
		if (m_windowType == WindowType::NativeWindow)
		{
			std::shared_ptr<Window> window = m_nativeWindow;
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
		std::shared_ptr<SingleChildSlot> slot = std::make_shared<SingleChildSlot>();
		slot->setChildWidget(widget);
		//slot->setHorizontalAlignment(HorizontalAlignment::Stretch);
		//slot->setVerticalAlignment(VerticalAlignment::Stretch);
		m_childWidget = slot;
	}
	void WindowWidget::assocateWithNativeWindow(std::shared_ptr<Window> nativeWindow)
	{
		m_nativeWindow = nativeWindow;
		if (m_fixedSize.x != 0.0 && m_fixedSize.y != 0.0f)
		{
			m_nativeWindow->reshapeWindow(m_screenPosition, m_fixedSize);
		}
		m_windowType = WindowType::NativeWindow;
		setCachedScreenPosition(nativeWindow->getWindowScreenSpacePosition());
	}	
	std::shared_ptr<Window> WindowWidget::getNativeWindow()
	{
		std::shared_ptr<Window> nativeWindow = m_nativeWindow;
		return nativeWindow;
	}
	void WindowWidget::setCachedScreenPosition(math::float2 newPosition)
	{
		m_screenPosition = newPosition;
	}
	void WindowWidget::setCachedSize(math::float2 newSize)
	{
		if ((m_fixedSize.x != newSize.x) || (m_fixedSize.y != newSize.y))
		{
			//todo:调整 native window 窗口大小
			m_fixedSize = newSize;

			if (m_nativeWindow)
			{
				m_nativeWindow->reshapeWindow(m_screenPosition, newSize);
			}	
		}	
	}
	math::float2 WindowWidget::getPositionInScreen() const
	{
		return m_screenPosition;
	}
	WidgetGeometry WindowWidget::getWindowGeometryInScreen() const
	{
		//我们对于布局来缩放孩子，但是我们的像素边界是不改变的
		//WidgetGeometry要求大小在局部空间，但是我们的大小存储在屏幕空间
		//我们需要去转换大小到窗口的局部空间，对于widget geometry
		
		//首先得到一个 local to screen transform
		math::affine2 localToScreen(math::float2x2::diagonal(m_nativeWindow->getDpiFactor()), m_screenPosition);

		WidgetGeometry res = WidgetGeometry::makeRoot(math::inverse(localToScreen).transformVector(m_fixedSize), localToScreen);

		return res;
	}
	math::box2 WindowWidget::getClientRectInScreen() const
	{
        if(Application::getApplication()->getGlobalPreRotate() == 90.0f || Application::getApplication()->getGlobalPreRotate() == 270.0f)
        {
            math::float2 newSize = math::float2(m_fixedSize.y, m_fixedSize.x);
            return math::box2(math::float2(m_screenPosition), math::float2(m_screenPosition + newSize));
        }
		return math::box2(math::float2(m_screenPosition), math::float2(m_screenPosition + m_fixedSize));
	}
	void WindowWidget::moveWindowTo(math::float2 newPosition)
	{
		if (m_nativeWindow)
		{
			math::float2 speculativeScreenPosition(int32_t(newPosition.x), int32_t(newPosition.y));
			setCachedScreenPosition(speculativeScreenPosition);

			m_nativeWindow->moveWindowTo(math::float2(speculativeScreenPosition.x, speculativeScreenPosition.y));
		}
	}
}