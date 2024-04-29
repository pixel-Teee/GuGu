#include <pch.h>

#include "MenuAnchor.h"
#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	MenuAnchor::MenuAnchor()
	{
	}
	MenuAnchor::~MenuAnchor()
	{
	}
	void MenuAnchor::init(const BuilderArguments& arguments)
	{
		//MenuAnchor::Slot().operator()
		//	(
		//		arguments.mContent->getChildWidget()
		//	);
		BasicLayoutSlot::SlotBuilderArguments slotArguments = MenuAnchor::Slot()
			(
				arguments.mContent->getChildWidget()
			)
			.setPadding(arguments.mpadding);
		std::shared_ptr<MenuAnchor::BasicLayoutSlot> slot = std::static_pointer_cast<MenuAnchor::BasicLayoutSlot>
			(
				slotArguments.m_slot
			);
		slot->init(shared_from_this(), slotArguments);
		m_childrens.push_back(slot);

		//留一个空的槽后续使用
		BasicLayoutSlot::SlotBuilderArguments emptySlotArguments = MenuAnchor::Slot();
		std::shared_ptr<MenuAnchor::BasicLayoutSlot> emptySlot = std::static_pointer_cast<MenuAnchor::BasicLayoutSlot>
			(
				emptySlotArguments.m_slot
			);
		emptySlot->init(shared_from_this(), slotArguments);
		m_childrens.push_back(emptySlot);

		m_menuContent		= arguments.mmenuContent;
		m_wrappedContent	= arguments.mmenuContent;
		m_onGetMenuContent	= arguments.monGetMenuContent;
		m_onMenuOpenChanged = arguments.monMenuOpenChanged;
		m_placeMent			= arguments.mplacement;
		m_bFitInWindow		= arguments.mfitInWindow;
		m_method			= arguments.mmethod;
		m_bShouldDeferPaintingAfterWindowContent = arguments.mshouldDeferPaintingAfterWindowContent;
		m_bUseApplicationMenuStack				 = arguments.museApplicationMenuStack;
		m_bIsCollapsedByParent					 = arguments.misCollapsedByParent;
		m_bApplyWidgetStyleToMenu				 = arguments.mapplyWidgetStyleToMenu;

		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;
	}
	uint32_t MenuAnchor::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		
		return layer;
	}
	math::float2 MenuAnchor::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::float2();
	}
	void MenuAnchor::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		//arrange single child
		const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

		if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
		{
			AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
			AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);
			const WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset));
			arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
		}

		//弹出的窗口
		const std::shared_ptr<WindowWidget> presentingWindow = m_popupWindowPtr.lock();
		if (isOpenAndReusingWindow() && presentingWindow)
		{
			const PopupPlacement localPlacement(allocatedGeometry, m_childrens[1]->getChildWidget()->getFixedSize(), m_placeMent.Get());
			

		}
	}
	SlotBase* MenuAnchor::getSlot(uint32_t index) const
	{
		return m_childrens[index].get();
	}
	uint32_t MenuAnchor::getSlotsNumber() const
	{
		return m_childrens.size();
	}
	bool MenuAnchor::isOpenAndReusingWindow() const
	{
		return m_methodInUse.isSet() && m_methodInUse.getPopupMethod() == PopupMethod::UseCurrentWindow;
	}
	MenuAnchor::PopupPlacement::PopupPlacement(const WidgetGeometry& placementGeometry, const math::float2& popupDesiredSize, MenuPlacement placementMode)
	{
		//计算 pop up size, offset, 还有 anchor rect 在 local space
		const bool bIsComboBoxPopup = (placementMode == MenuPlacementComboBox || placementMode == MenuPlacementComboBoxRight);
		m_localPopupSize = bIsComboBoxPopup ? math::float2(std::max(placementGeometry.mLocalSize.x, popupDesiredSize.x), popupDesiredSize.y) : popupDesiredSize;
		//compute local pop up offset
		//compute anchor local space
		m_orientation = (placementMode == MenuPlacementMenuRight || placementMode == MenuPlacementMenuLeft) ? Orientation::Horizontal : Orientation::Vertical;
	}
}