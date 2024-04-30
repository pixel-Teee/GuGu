#include <pch.h>

#include "MenuAnchor.h"
#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "WidgetPath.h"
#include "Menu.h"
#include "NullWidget.h"

#include <Application/Application.h>

namespace GuGu {
	std::vector<std::weak_ptr<IMenu>> MenuAnchor::m_openApplicationMenus;

	static math::float2 getMenuOffsetForPlacement(const WidgetGeometry& allottedGeometry, MenuPlacement placementMode, math::float2 popupSizeLocalSpace)
	{
		switch (placementMode)
		{
		case MenuPlacementBelowAnchor:
			return math::float2(0.0f, allottedGeometry.getLocalSize().y);//在光标下面
			break;
		case MenuPlacementCenteredBelowAnchor:
			return math::float2(-((popupSizeLocalSpace.x / 2.0f) - (allottedGeometry.getLocalSize().x / 2.0f)), allottedGeometry.getLocalSize().y);
			break;
		case MenuPlacementBelowRightAnchor:
			return math::float2(-(popupSizeLocalSpace.x) + (allottedGeometry.getLocalSize().x), allottedGeometry.getLocalSize().y);
			break;
		case MenuPlacementComboBox:
			return math::float2(0.0f, allottedGeometry.getLocalSize().y);
			break;
		case MenuPlacementComboBoxRight:
			return math::float2(allottedGeometry.getLocalSize().x - popupSizeLocalSpace.x, allottedGeometry.getLocalSize().y);
			break;
		case MenuPlacementMenuRight:
			return math::float2(allottedGeometry.getLocalSize().x, 0.0f);
			break;
		case MenuPlacementAboveAnchor:
			return math::float2(0.0f, -popupSizeLocalSpace.y);
			break;
		case MenuPlacementCenteredAboveAnchor:
			return math::float2(-((popupSizeLocalSpace.x / 2.0f) - (allottedGeometry.getLocalSize().x / 2.0f)), -popupSizeLocalSpace.y);
			break;
		case MenuPlacementAboveRightAnchor:
			return math::float2(-(popupSizeLocalSpace.x) - (allottedGeometry.getLocalSize().x), -popupSizeLocalSpace.y);
			break;
		case MenuPlacementMenuLeft:
			return math::float2(-popupSizeLocalSpace.x, 0.0f);
			break;
		case MenuPlacementCenter:
			return math::float2(-((popupSizeLocalSpace.x / 2.0f) - (allottedGeometry.getLocalSize().x / 2.0f)), -((popupSizeLocalSpace.y / 2.0f) - (allottedGeometry.getLocalSize().y / 2.0f)));
			break;
		case MenuPlacementRightLeftCenter:
			return math::float2(allottedGeometry.getLocalSize().x, -((popupSizeLocalSpace.y / 2.0f) - (allottedGeometry.getLocalSize().y / 2.0f)));
			break;
		case MenuPlacementMatchBottomLeft:
			return math::float2(0.0f, (allottedGeometry.getLocalSize().y - popupSizeLocalSpace.y));
			break;
		default:
			return math::float2(0.0f, 0.0f);
		}
	}
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
		emptySlot->init(shared_from_this(), emptySlotArguments);
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
		ArrangedWidgetArray arrangedChildren(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedChildren);

		//这里可能有0个元素，如果我们的孩子折叠或者隐藏了
		if (arrangedChildren.getArrangedWidgetsNumber() > 0)
		{
			const ArrangedWidget& firstChild = *arrangedChildren.getArrangedWidget(0);

			const bool bHasArrangedAnchorContent = firstChild.getWidget() == m_childrens[0]->getChildWidget();

			if (bHasArrangedAnchorContent)
			{
				layer = firstChild.getWidget()->generateElement(paintArgs, cullingRect, elementList, firstChild.getWidgetGeometry(), layer + 1);
			}

			const bool bIsOpen = isOpen();//是否打开菜单

			if (bIsOpen)
			{
				//在这种情况下，anchor 内容被呈现并且可见的，它是第一个索引的孩子
				//anchor 内容是不可见的，它是第0个索引孩子
				ArrangedWidget* popupChild = nullptr;
				if (bHasArrangedAnchorContent && arrangedChildren.getArrangedWidgetsNumber() > 1)
				{
					popupChild = arrangedChildren.getArrangedWidget(1).get();
				}
				else if (!bHasArrangedAnchorContent && arrangedChildren.getArrangedWidgetsNumber() == 1)
				{
					popupChild = arrangedChildren.getArrangedWidget(0).get();
				}

				if (popupChild != nullptr)
				{
					//todo:后续添加延迟绘制的方法
					if (m_bShouldDeferPaintingAfterWindowContent)
					{

					}
					else
					{
						const std::shared_ptr<WindowWidget> presentingWindow = m_popupWindowPtr.lock();
						if (presentingWindow)
						{
							//todo:这个 culling rect 改成 presenting window 的 get clipping rectangle in window
							popupChild->getWidget()->generateElement(paintArgs, cullingRect, elementList, popupChild->getWidgetGeometry(), layer + 1);
						}
					}
				}
			}
		}

		return layer;
	}
	math::float2 MenuAnchor::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		const math::float2 desiredWidgetSize = m_childrens[0]->getChildWidget()->getFixedSize();

		return math::float2(std::max(desiredWidgetSize.x, 1.0f), std::max(desiredWidgetSize.y, 1.0f));
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
			
			WidgetGeometry childGeometry;
			childGeometry = allocatedGeometry.getChildGeometry(localPlacement.m_localPopupSize, localPlacement.m_localPopupOffset);
			arrangedWidgetArray.pushWidget(childGeometry, getSlot(1)->getChildWidget());
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
	bool MenuAnchor::isOpen() const
	{
		return m_methodInUse.isSet() && m_popUpMenuPtr.lock() != nullptr;
	}
	void MenuAnchor::setIsOpen(bool inIsOpen, const bool bFocusMenu)
	{
		if (isOpen() != inIsOpen)
		{
			if (inIsOpen)
			{
				if (m_onGetMenuContent)
				{
					setMenuContent(m_onGetMenuContent());
				}

				if (m_menuContent)
				{
					//open pop up
					if (m_onMenuOpenChanged)
					{
						m_onMenuOpenChanged(true);
					}
				}

				WidgetPath myWidgetPath;
				//找出 menu anchor 在屏幕的位置，那么我们可以设置我们 pop-up 窗口初始的位置
				//menu anchor 到窗口的 path
				Application::getApplication()->generatePathToWidgetUnchecked(shared_from_this(), myWidgetPath);
				if (myWidgetPath.isValid())
				{
					const WidgetGeometry& myGeometry = myWidgetPath.m_widgets.back().getWidgetGeometry();
					const float layoutScaleMultiplier = myGeometry.getAccumulateLayoutTransform().m_linear[0][0];//scale

					prepass(layoutScaleMultiplier);

					//找出 content widget 有多大，那么我们可以正确地设置窗口的初始位置
					std::shared_ptr<Widget> menuContent = m_menuContent;
					menuContent->prepass(layoutScaleMultiplier);

					const math::float2 desiredContentSize = menuContent->getFixedSize();
					const MenuPlacement placementMode = m_placeMent.Get();

					const math::float2 newPosition = myGeometry.mAbsolutePosition;
					math::float2 newWindowSize = desiredContentSize;

					if (placementMode == MenuPlacementComboBox || placementMode == MenuPlacementComboBoxRight)
					{
						newWindowSize = math::float2(std::max(myGeometry.mLocalSize.x, desiredContentSize.x), desiredContentSize.y);
					}
					else if (placementMode == MenuPlacementBelowAnchor)
					{

					}
					else if (placementMode == MenuPlacementMenuRight)
					{
						newWindowSize = myGeometry.getAbsoluteSize();
					}

					//todo:这里要修复
					m_methodInUse = m_method.has_value() ? PopupMethodReply::useMethod(m_method.value()) : PopupMethodReply::useMethod(PopupMethod::UseCurrentWindow);

					//普通的菜单被应用程序的 menu stack 函数所创建和管理
					if (m_bUseApplicationMenuStack)
					{

					}
					else
					{
						assert(m_methodInUse.getPopupMethod() == PopupMethod::UseCurrentWindow);
						m_popupWindowPtr = myWidgetPath.getWindow();

						if (bFocusMenu)
						{
							//todo:释放应用程序所有的捕获
						}

						std::shared_ptr<MenuAnchor> sharedThis = std::static_pointer_cast<MenuAnchor>(shared_from_this());
						std::shared_ptr<IMenu> newMenu = std::make_shared<MenuInHostWidget>(std::static_pointer_cast<MenuAnchor>(shared_from_this()), menuContent, m_bIsCollapsedByParent);

						m_popUpMenuPtr = m_ownedMenuPtr = newMenu;

						m_childrens[1]->setChildWidget(menuContent);
						menuContent->setParentWidget(shared_from_this());

						if (bFocusMenu)
						{
							//todo:设置焦点
							//Application::getApplication()->setFocus(menuContent);
						}

						//open application menus
						m_openApplicationMenus.push_back(newMenu);
					}
				}
			}
			else
			{
				//close pop up
				if (m_popUpMenuPtr.expired() != 0)
				{
					//m_popUpMenuPtr.lock()->dismiss();
				}

				resetPopupMenuContent();//重置菜单
			}
		}
	}
	void MenuAnchor::setMenuContent(std::shared_ptr<Widget> inMenuContent)
	{
		m_menuContent = inMenuContent;
		m_wrappedContent = inMenuContent;
	}
	void MenuAnchor::resetPopupMenuContent()
	{
		m_methodInUse = PopupMethodReply::unHandled();
		m_popUpMenuPtr.reset();
		m_ownedMenuPtr.reset();
		m_popupWindowPtr.reset();

		//m_childrens[1] = NullWidget::getNullWidget();
		m_childrens[1]->setChildWidget(NullWidget::getNullWidget());

		//null widget 是不需要设置父亲的

		//if (m_childWidget->m_childWidget != NullWidget::getNullWidget())
		//	m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}
	std::shared_ptr<WindowWidget> MenuAnchor::getMenuWindow() const
	{
		return isOpen() ? m_popupWindowPtr.lock() : std::shared_ptr<WindowWidget>();
	}
	void MenuAnchor::onMenuDismissed()
	{
	}
	bool MenuAnchor::usingApplicationMenuStack() const
	{
		return m_bUseApplicationMenuStack;
	}
	MenuAnchor::PopupPlacement::PopupPlacement(const WidgetGeometry& placementGeometry, const math::float2& popupDesiredSize, MenuPlacement placementMode)
	{
		//计算 pop up size, offset, 还有 anchor rect 在 local space
		const bool bIsComboBoxPopup = (placementMode == MenuPlacementComboBox || placementMode == MenuPlacementComboBoxRight);
		m_localPopupSize = bIsComboBoxPopup ? math::float2(std::max(placementGeometry.mLocalSize.x, popupDesiredSize.x), popupDesiredSize.y) : popupDesiredSize;
		//compute local pop up offset
		m_localPopupOffset = getMenuOffsetForPlacement(placementGeometry, placementMode, m_localPopupSize);
		//compute anchor local space
		m_anchorLocalSpace = math::box2(math::float2(0.0, 0.0f), math::float2(placementGeometry.getLocalSize().x, placementGeometry.getLocalSize().y));
		m_orientation = (placementMode == MenuPlacementMenuRight || placementMode == MenuPlacementMenuLeft) ? Orientation::Horizontal : Orientation::Vertical;
	}
}