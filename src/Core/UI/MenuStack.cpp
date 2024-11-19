#include <pch.h>

#include "MenuStack.h"
#include "WidgetPath.h"
#include "ArrangedWidget.h"
#include "Widget.h"
#include "Menu.h"
#include "Overlay.h"
#include "WindowWidget.h"
#include "Popup.h"
#include "Box.h"
#include "LayoutUtils.h"
#include <Window/Window.h>
#include <Application/Application.h>

namespace GuGu {
	class MenuPanel : public Overlay
	{
	public:
		struct BuilderArguments : public Arguments<Overlay>
		{
			BuilderArguments() 
			{
				mVisibility = Visibility::SelfHitTestInvisible;
			}	

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments)
		{
			Overlay::init(Overlay::BuilderArguments());
		}

		//逻辑菜单MenuBase
		void pushMenu(std::shared_ptr<MenuBase> inMenu, const math::float2& inLocation)
		{
			std::shared_ptr<WindowWidget> parentWindow = inMenu->getParentWindow();

			math::float2 panelInScreen = parentWindow->getClientRectInScreen().getCorner(0);//left top corner
			math::float2 panelInWindow = math::inverse(parentWindow->getLocalToScreenTransform()).transformPoint(panelInScreen);
			math::float2 locationInWindow = math::inverse(parentWindow->getLocalToScreenTransform()).transformPoint(inLocation);
			math::float2 locationInPanel = locationInWindow - panelInWindow;

			OverlaySlot& overlaySlot = addSlot();
			overlaySlot.setHorizontalAlignment(HorizontalAlignment::Left);
			overlaySlot.setVerticalAlignment(VerticalAlignment::Top);
			overlaySlot.setPadding(Padding(locationInPanel.x, locationInPanel.y, 0, 0));
			overlaySlot.setChildWidget(inMenu->getContent());
		}

		void onMenuClosed(std::shared_ptr<IMenu> inMenu)
		{
			removeSlot(inMenu->getContent());
		}
	};

	namespace MenuStackInternal
	{
		using MenuOnKeyDown = std::function<Reply(Key)>;
		using OnMenuLostFocus = std::function<void(const WidgetPath&)>;
		//这个控件用来包裹创建在 FMenuStack中的任何控件，去提供默认的键盘输入，焦点追踪，并且帮助我们去发现菜单在控件路径中
		class MenunContentWrapper : public Widget
		{
		public:
			struct BuilderArguments : public Arguments<MenunContentWrapper>
			{
				BuilderArguments()
				{
				}

				~BuilderArguments() = default;

				ARGUMENT_NAMED_SLOT(SingleChildSlot, MenuContent)
				UI_EVENT(MenuOnKeyDown, onKeyDown)
				UI_EVENT(OnMenuLostFocus, onMenuLostFocus)
				ARGUMENT_VALUE(OptionalSize, OptionalMinMenuWidth)
				ARGUMENT_VALUE(OptionalSize, OptionalMinMenuHeight)
			};

			void init(const BuilderArguments& arguments)
			{
				m_visibilityAttribute = arguments.mVisibility;

				this->m_childWidget = std::make_shared<SingleChildSlot>();
				this->m_childWidget->setChildWidget(
					WIDGET_NEW(BoxWidget)
					.MinDesiredWidth(arguments.mOptionalMinMenuWidth)
					.MinDesiredHeight(arguments.mOptionalMinMenuHeight)
					.Content(
						arguments.mMenuContent->m_childWidget
					)
				);
				//m_childWidget = arguments.mMenuContent;
				m_childWidget->m_parentWidget = shared_from_this();
				m_childWidget->m_childWidget->setParentWidget(shared_from_this());

				m_onKeyDownDelegate = arguments.monKeyDown;
				m_onMenuLostFocus = arguments.monMenuLostFocus;
			}

			virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override
			{
				ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);//设置数组只接受可见的child widget
				AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

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

			virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override
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

			virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override
			{
				//arrange single children
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

			virtual SlotBase* getSlot(uint32_t index) const override
			{
				return m_childWidget.get();
			}

			virtual uint32_t getSlotsNumber() const override
			{
				return 1;
			}

			virtual bool supportsKeyboardFocus() const override
			{
				return true;
			}

			virtual Reply OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent) override
			{
				if (m_onKeyDownDelegate)
				{
					return m_onKeyDownDelegate(inKeyEvent.getKey());
				}
				return Reply::Unhandled();
			}

			virtual void OnFocusChanging(const WeakWidgetPath& previousFocusPath, const WidgetPath& newWidgetPath) override
			{
				if (m_onMenuLostFocus && previousFocusPath.containsWidget(shared_from_this().get()))
				{
					return m_onMenuLostFocus(newWidgetPath);
				}
			}
		protected:
			std::shared_ptr<SingleChildSlot> m_childWidget;

			MenuOnKeyDown m_onKeyDownDelegate;

			//去通知一个菜单已经失去焦点，需要去被关闭
			OnMenuLostFocus m_onMenuLostFocus;
		};

		Reply OnMenuKeyDown(const Key key)
		{
			//if(key == Keys::e)

			return Reply::Unhandled();
		}
	}

	std::shared_ptr<IMenu> MenuStack::push(const WidgetPath& inOwnerPath, const std::shared_ptr<Widget>& inContent, const math::float2& summonLocation, const bool bFocusImmediately, const math::float2& summonLocationSize, std::optional<PopupMethod> inMethod, const bool bIsCollapsedByParent)
	{
		//return std::shared_ptr<IMenu>();

		math::box anchor(summonLocation, summonLocationSize);
		std::shared_ptr<IMenu> parentMenu;

		if (hasMenus())
		{
			//找到在InOwnerPath中的一个菜单，去决定什么等级插入
			//一个具体的菜单控件到逻辑上的MenuBase
			parentMenu = findMenuInWidgetPath(inOwnerPath);
		}

		if (!parentMenu)
		{
			//放入一个根root menu

			//active method 被决定，当一个新的根 root menu 被放入的时候
			//创建新窗口还是使用现在的窗口
			m_activeMethod = inMethod.has_value() ? PopupMethodReply::useMethod(inMethod.value()) : queryPopupMethod(inOwnerPath);
			
			//当一个新的根menu被push的时候，宿主窗口将被决定出来
			setHostPath(inOwnerPath);
		}

		return pushInternal(parentMenu, inContent, anchor, bFocusImmediately, bIsCollapsedByParent);
	}
	bool MenuStack::hasMenus() const
	{
		return m_stack.size() > 0;
	}
	std::shared_ptr<IMenu> MenuStack::findMenuInWidgetPath(const WidgetPath& pathToQuery) const
	{
		int32_t widgetNumber = pathToQuery.m_widgets.getArrangedWidgetsNumber();
		for (int32_t pathIndex = widgetNumber - 1; pathIndex >= 0; --pathIndex)
		{
			std::shared_ptr<Widget> widget = pathToQuery.m_widgets[pathIndex]->getWidget();
			
			auto it = m_cachedContentMap.find(widget);

			if (it != m_cachedContentMap.end())
			{
				return std::static_pointer_cast<IMenu>(it->second);
			}
		}

		return std::shared_ptr<IMenu>();
	}
	PopupMethodReply MenuStack::queryPopupMethod(const WidgetPath& pathToQuery)
	{
		int32_t widgetNumber = pathToQuery.m_widgets.getArrangedWidgetsNumber();
		for (int32_t widgetIndex = widgetNumber - 1; widgetIndex >= 0; --widgetIndex)
		{
			PopupMethodReply popupMethod = pathToQuery.m_widgets[widgetIndex]->getWidget()->OnQueryPopupMethod();
			if (popupMethod.isEventHandled())
			{
				return popupMethod;
			}
		}

		return PopupMethodReply::useMethod(PopupMethod::CreateNewWindow);
	}
	void MenuStack::setHostPath(const WidgetPath& inOwnerPath)
	{
		//todo:添加 host window guard
		if (m_hostPopupLayer)
		{
			if (!inOwnerPath.containsWidget(m_hostPopupLayer->getHost().get()))
			{
				m_hostPopupLayer->remove();
				m_hostPopupLayer.reset();
				m_hostWindowPopupPanel.reset();
			}
		}

		hostWindow = inOwnerPath.isValid() ? inOwnerPath.getWindow() : std::shared_ptr<WindowWidget>();//后续 push internal 的时候用到，作为根菜单窗口的父窗口
		
		hostWidget = inOwnerPath.isValid() ? inOwnerPath.getLastWidget() : std::weak_ptr<Widget>();

		if (hostWindow)
		{
			std::shared_ptr<MenuPanel> newHostWindowPopupPanel = WIDGET_NEW(MenuPanel);
			int32_t widgetNumber = inOwnerPath.m_widgets.getArrangedWidgetsNumber() - 1;
			for (int32_t i = widgetNumber - 1; i >= 0; --i)
			{
				const std::shared_ptr<Widget>& currentWidget = inOwnerPath.m_widgets[i]->getWidget();//window
				m_hostPopupLayer = currentWidget->onVisualizePopup(newHostWindowPopupPanel);
				if (m_hostPopupLayer)
				{
					m_hostWindowPopupPanel = newHostWindowPopupPanel;
					break;
				}
			}
		}
	}
	void MenuStack::onMenuContentLostFocus(const WidgetPath& inFocussedPath)
	{
		if (hasMenus() && !m_pendingNewMenu)
		{
			std::shared_ptr<IMenu> focussedMenu = findMenuInWidgetPath(inFocussedPath);

			if (focussedMenu)
			{
				auto it = std::find(m_stack.begin(), m_stack.end(), focussedMenu);
				if (it != m_stack.end())
				{
					int32_t focussedIndex = it - m_stack.begin();
					for (int32_t dimissIndex = focussedIndex + 1; dimissIndex < m_stack.size(); ++dimissIndex)
					{
						if (m_stack[dimissIndex]->isCollapsedByParent())
						{
							dimissFrom(m_stack[dimissIndex]);
							break;
						}
					}
				}
			}
			else
			{
				//焦点已经移出到所有的菜单-折叠栈
				dimissAll();
			}
		}
	}
	void MenuStack::dimissFrom(const std::shared_ptr<IMenu>& inFromMenu)
	{
		auto it = std::find(m_stack.begin(), m_stack.end(), inFromMenu);
		int32_t index = it - m_stack.begin();
		if (index != -1)
		{

		}
	}
	void MenuStack::dimissAll()
	{
		const int32_t topLevel = 0;

	}
	void MenuStack::dimissInternal(int32_t firstStackIndexToRemove)
	{
		//倒序去销毁
		for (int32_t stackIndex = m_stack.size() - 1; stackIndex >= firstStackIndexToRemove; --stackIndex)
		{
			if (stackIndex >= 0 && stackIndex < m_stack.size())
			{
				m_stack[stackIndex]->dismiss();
			}
		}
	}
	std::shared_ptr<IMenu> MenuStack::pushInternal(const std::shared_ptr<IMenu>& inParentMenu, const std::shared_ptr<Widget>& inContent, math::box2 anchor, const bool bFocusImmediately, const bool bIsCollapsedByParent)
	{
		PrePushArgs prePushArgs;
		prePushArgs.m_content = inContent;
		prePushArgs.m_anchor = anchor;
		prePushArgs.m_bFocusImmediately = bFocusImmediately;
		prePushArgs.m_bIsCollapsedByParent = bIsCollapsedByParent;

		//pre-push stage
		//决定正确的布局
		//包裹内容
		//其他通用的设置步骤
		const PrePushResults prePushResults = prePush(prePushArgs);

		//菜单对象创建过程
		std::shared_ptr<MenuBase> outMenu = m_activeMethod.getPopupMethod() == PopupMethod::CreateNewWindow ?
			pushNewWindow(inParentMenu, prePushResults)
			: pushPopup(inParentMenu, prePushResults);

		const bool bInInsertAfterDismiss = m_activeMethod.getPopupMethod() == PopupMethod::CreateNewWindow;
		postPush(inParentMenu, outMenu, bInInsertAfterDismiss);

		return outMenu;
	}
	MenuStack::PrePushResults MenuStack::prePush(const PrePushArgs& inArgs)
	{
		PrePushResults outResults;
		outResults.m_bIsCollapsedByParent = inArgs.m_bIsCollapsedByParent;
		outResults.m_bFocusImmediately = inArgs.m_bFocusImmediately;
		if (inArgs.m_bFocusImmediately)
		{
			outResults.m_widgetToFocus = inArgs.m_content;
		}

		//计算最大可用的高度，对于菜单
		float maxHeight;
		const float applicationScale = hostWindow->getNativeWindow()->getDpiFactor();
		if (m_activeMethod.getPopupMethod() == PopupMethod::CreateNewWindow)
		{
			//todo:修复这个
			maxHeight = 0.8f * hostWindow->getClientSizeInScreen().y / applicationScale;
		}
		else
		{
			maxHeight = 0.8f * hostWindow->getClientSizeInScreen().y / applicationScale;
		}
		OptionalSize optionalMinWidth = OptionalSize();
		OptionalSize optionalMinHeight = maxHeight;

		std::shared_ptr<Widget> tempContent = WIDGET_NEW(Popup).Content(inArgs.m_content);

		outResults.m_warppedContent = wrapContent(tempContent, optionalMinWidth, optionalMinHeight);

		outResults.m_warppedContent->prepass(applicationScale);
		outResults.m_expectedSize = outResults.m_warppedContent->getFixedSize() * applicationScale;

		if (m_activeMethod.getPopupMethod() == PopupMethod::CreateNewWindow)
		{
			//已经缩放过
			const bool bAutoAdjustForDPIScale = false;
			//outResults.m_startLocation = Application::getApplication()->calculatePopupWindowPosition(inArgs.m_anchor, outResults.m_expectedSize, bAutoAdjustForDPIScale, math::float2(0, 0), Orientation::Vertical);
			outResults.m_startLocation = inArgs.m_anchor.getCorner(0);//left top
		}

		if (inArgs.m_bFocusImmediately)
		{
			//todo:实现这里
		}

		return outResults;
	}
	std::shared_ptr<MenuBase> MenuStack::pushNewWindow(std::shared_ptr<IMenu> inParentMenu, const PrePushResults& inPrePushResults)
	{
		std::shared_ptr<WindowWidget> newMenuWindow = WIDGET_NEW(WindowWidget)
		.ClientSize(inPrePushResults.m_expectedSize)
		.ScreenPosition(inPrePushResults.m_startLocation)
		.Content
		(
			inPrePushResults.m_warppedContent
		);

		std::shared_ptr<MenuInWindow> menu = std::make_shared<MenuInWindow>(newMenuWindow, inPrePushResults.m_warppedContent, inPrePushResults.m_bIsCollapsedByParent);
		m_pendingNewMenu = menu;

		std::shared_ptr<WindowWidget> parentWindow;
		if (inParentMenu)
		{
			parentWindow = inParentMenu->getParentWindow();
		}
		else
		{
			parentWindow = hostWindow;
		}

		//add window as native child
		//todo:修复这里
		Application::getApplication()->makeWindow(newMenuWindow);
		Application::getApplication()->showWindow(newMenuWindow);

		m_pendingNewMenu.reset();

		return menu;
	}
	std::shared_ptr<MenuBase> MenuStack::pushPopup(std::shared_ptr<IMenu> inParentMenu, const PrePushResults& inPrePushResults)
	{
		return nullptr;
	}
	void MenuStack::postPush(std::shared_ptr<IMenu> inParentMenu, std::shared_ptr<MenuBase> inMenu, bool bInInsertAfterDismiss)
	{
		int32_t insertIndex = 0;
		if (inParentMenu)
		{
			int32_t parentIndex = -1;
			for (size_t i = 0; i < m_stack.size(); ++i)
			{
				if (m_stack[i] == inParentMenu)
				{
					parentIndex = i;
					break;
				}
			}

			insertIndex = parentIndex + 1;
		}

		int32_t removingAtIndex = insertIndex;
		if (!bInInsertAfterDismiss)
		{
			m_stack.insert(m_stack.begin() + insertIndex, inMenu);
			m_cachedContentMap.insert({ inMenu->getContent(), inMenu });
			removingAtIndex = insertIndex + 1;
		}

		//todo:实现后续逻辑
	}
	std::shared_ptr<Widget> MenuStack::wrapContent(std::shared_ptr<Widget> inContent, OptionalSize optionalMinWidth, OptionalSize optionalMinHeight)
	{
		return WIDGET_NEW(MenuStackInternal::MenunContentWrapper)
			.onKeyDown(&MenuStackInternal::OnMenuKeyDown)
			.onMenuLostFocus(this, &MenuStack::onMenuContentLostFocus)
			.OptionalMinMenuWidth(optionalMinWidth)
			.OptionalMinMenuHeight(optionalMinHeight)
			.MenuContent(
				inContent
			);
	}
}