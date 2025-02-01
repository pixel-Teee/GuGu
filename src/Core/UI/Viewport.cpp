#include <pch.h>

#include "Viewport.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "LayoutUtils.h"
#include "ElementList.h"

#include <Core/GamePlay/InputManager.h>
#include <Application/Application.h>

namespace GuGu {
	ViewportWidget::ViewportWidget()
	{
	}
	ViewportWidget::~ViewportWidget()
	{
	}
	void ViewportWidget::init(const BuilderArguments& arguments)
	{
		m_childWidget = arguments.mContent;
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;
		m_widgetClipping = arguments.mClip;
	}
	uint32_t ViewportWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		if (m_viewportClient.lock())
		{
			m_viewportClient.lock()->resizeViewport(allocatedGeometry.getLocalSize().x, allocatedGeometry.getLocalSize().y);
		}

		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		if (m_renderTarget)
		{
			ElementList::addViewportElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_renderTarget, layer + 1);
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

		return layer + 1;
	}
	math::float2 ViewportWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::float2(600.0f, 480.0f);//todo:fix this
	}
	void ViewportWidget::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		uint32_t slotNumber = getSlotsNumber();
		if (slotNumber)
		{
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
	SlotBase* ViewportWidget::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t ViewportWidget::getSlotsNumber() const
	{
		return 1;
	}
	void ViewportWidget::setRenderTarget(nvrhi::TextureHandle renderTarget)
	{
		m_renderTarget = renderTarget;
	}
	Reply ViewportWidget::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		math::float2 localMousePosition = myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition);
		InputManager::getInputManager().updateMouseButton(inMouseEvent.m_effectingButton, localMousePosition.x, localMousePosition.y, true);
		return Reply::Handled().setFocus(shared_from_this());
	}

	Reply ViewportWidget::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{	
		math::float2 localMousePosition = myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition);
		InputManager::getInputManager().updateMouseButton(inMouseEvent.m_effectingButton, localMousePosition.x, localMousePosition.y, false);
		return Reply::Handled();
	}

	Reply ViewportWidget::OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		math::float2 localMousePosition = myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition);
		InputManager::getInputManager().updateMouseButton(inMouseEvent.m_effectingButton, localMousePosition.x, localMousePosition.y, false);
		return Reply::Handled();
	}

	void ViewportWidget::OnMouseLeave(const PointerEvent& inMouseEvent)
	{
		InputManager::getInputManager().clearEvents();
		InputManager::getInputManager().clearMouseStates();
	}

	Reply ViewportWidget::OnMouseWheel(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		InputManager::getInputManager().updateWheelDelta(inMouseEvent.m_wheelOrGestureDelta);
		return Reply();
	}

	Reply ViewportWidget::OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent)
	{
		ModifierKeysState keysState = Application::getApplication()->getModifierKeys();
		applyModifierKeys(keysState);
		InputManager::getInputManager().updateKeyboard(inKeyEvent.getKey(), true); //game play 
		return Reply::Handled();
	}
	Reply ViewportWidget::OnKeyUp(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent)
	{
		ModifierKeysState keysState = Application::getApplication()->getModifierKeys();
		updateModifierKeys(keysState);
		InputManager::getInputManager().updateKeyboard(inKeyEvent.getKey(), false); //game play 
		return Reply::Handled();
	}
	void ViewportWidget::OnFocusLost()
	{
		InputManager::getInputManager().clearEvents();
		InputManager::getInputManager().clearMouseStates();
	}
	void ViewportWidget::applyModifierKeys(ModifierKeysState keyState)
	{
		if(keyState.isLeftShiftDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::LeftShift, true);
		}
		if (keyState.isRightShiftDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::RightShift, true);
		}
		if (keyState.isLeftAltDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::LeftAlt, true);
		}
		if (keyState.isRightAltDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::RightAlt, true);
		}
		if (keyState.isLeftControlDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::LeftControl, true);
		}
		if (keyState.isRightControlDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::RightControl, true);
		}
	}
	void ViewportWidget::updateModifierKeys(ModifierKeysState keyState)
	{
		if (!keyState.isLeftShiftDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::LeftShift, false);
		}
		if (!keyState.isRightShiftDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::RightShift, false);
		}
		if (!keyState.isLeftAltDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::LeftAlt, false);
		}
		if (!keyState.isRightAltDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::RightAlt, false);
		}
		if (!keyState.isLeftControlDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::LeftControl, false);
		}
		if (!keyState.isRightControlDown())
		{
			InputManager::getInputManager().updateKeyboard(Keys::RightControl, false);
		}
	}

	std::shared_ptr<GuGu::ViewportClient> ViewportWidget::getViewportClient()
	{
		return m_viewportClient.lock();
	}

}