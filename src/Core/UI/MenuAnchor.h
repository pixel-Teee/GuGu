#pragma once

#include "IMenu.h"
#include "Widget.h"
#include "WidgetDelegates.h"
#include "PopupMethodReply.h"
#include "UIMacros.h"
#include "Slot.h"
#include "TextBlockWidget.h"

namespace GuGu {
	//当 pop up 被打开关闭的时候，进行通知
	using OnIsOpenChanged = std::function<void(bool)>;
	//一个 pop up anchor 召唤出一个 pop up 相对于它的内容
	//通过 application 去弹出一个 pop up 相对于它的光标
	class MenuAnchor : public Widget, public IMenuHost
	{
	public:
		MenuAnchor();

		virtual ~MenuAnchor();

		struct BuilderArguments : public Arguments<MenuAnchor>
		{
			BuilderArguments()
				: mpadding(Padding(0.0f))
				, mmenuContent(WIDGET_NEW(TextBlockWidget).text("No Menu Content Assigned"))
				, mplacement(MenuPlacementBelowAnchor)
				, mfitInWindow(true)
				, mshouldDeferPaintingAfterWindowContent(false)//todo:这个暂时改为false，后续改为true，延迟绘制的能力还没添加
				, museApplicationMenuStack(false)//todo:这个暂时改为false，后续改为true，
				, misCollapsedByParent(false)
				, mapplyWidgetStyleToMenu(true)
			{}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(Padding, padding)

			ARGUMENT_VALUE(std::shared_ptr<Widget>, menuContent)

			UI_EVENT(OnGetContent, onGetMenuContent)

			UI_EVENT(OnIsOpenChanged, onMenuOpenChanged)

			ARGUMENT_ATTRIBUTE(MenuPlacement, placement)

			ARGUMENT_VALUE(bool, fitInWindow)

			ARGUMENT_VALUE(std::optional<PopupMethod>, method)

			ARGUMENT_VALUE(bool, shouldDeferPaintingAfterWindowContent)

			ARGUMENT_VALUE(bool, useApplicationMenuStack)

			ARGUMENT_VALUE(bool, isCollapsedByParent)

			ARGUMENT_VALUE(bool, applyWidgetStyleToMenu)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;
	protected:
		//返回真，如果 pop up 现在被打开，并且重复使用一个已经存在的窗口
		bool isOpenAndReusingWindow() const;

		//打开或者关闭 pop up
		bool isOpen() const;

		//打开或者关闭 pop up ，这个函数很重要
		virtual void setIsOpen(bool inIsOpen, const bool bFocusMenu = true);

		//当 pop up 打开的时候，调用这个，从 m_onGetMenuContent 里获取内容
		virtual void setMenuContent(std::shared_ptr<Widget> inMenuContent);

		//当关闭 pop up 的时候，就重置一些内容，做善后工作1
		void resetPopupMenuContent();

		static std::vector<std::weak_ptr<IMenu>> m_openApplicationMenus;

		//IMenuHost的接口
		virtual std::shared_ptr<WindowWidget> getMenuWindow() const override;//宿主窗口
		virtual void onMenuDismissed() override;//当菜单消失的时候
		virtual bool usingApplicationMenuStack() const override;//是否使用应用的菜单栈
	protected:
		struct PopupPlacement
		{
			PopupPlacement(const WidgetGeometry& placementGeometry, const math::float2& popupDesiredSize, MenuPlacement placementMode);

			math::float2 m_localPopupSize;
			math::float2 m_localPopupOffset;
			math::box2 m_anchorLocalSpace;
			Orientation m_orientation;
		};

		//一个表示这个 pop up 的窗口
		//可以是持有一个菜单的窗口或者包含这个 anchor 的窗口，如果菜单是被绘制为这个 anchor 的儿子
		std::weak_ptr<WindowWidget> m_popupWindowPtr;

		//一个指向表示这个 pop up 菜单对象的接口指针
		std::weak_ptr<IMenu> m_popUpMenuPtr;

		//这个菜单被这个 widget 持有，而不是 application 的 menu stack
		std::shared_ptr<IMenu> m_ownedMenuPtr;

		//静态菜单对象所使用的，当委托 OnGetMenuContent 没有被定义的时候
		std::shared_ptr<Widget> m_menuContent;

		std::shared_ptr<Widget> m_wrappedContent;

		//被调用，当 pop up 被调出的时候
		OnGetContent m_onGetMenuContent;

		OnIsOpenChanged m_onMenuOpenChanged;

		//pop up 应当如果放置相对于 anchor
		Attribute<MenuPlacement> m_placeMent;

		//是否 menu anchor 填充进 window 的内部？
		bool m_bFitInWindow;

		//是否 menu 在这个 tick 下清除掉？
		bool m_bDismissedThisTick;

		//是否这个菜单应当被折叠，当它的父亲获取到焦点的时候
		bool m_bIsCollapsedByParent;

		//我们是否应当呼唤出一个新窗口对于这个 pop up
		std::optional<PopupMethod> m_method;

		//被我们现在所使用去展示 pop up 的方法，如果 pop up 被关闭的时候，没有值
		PopupMethodReply m_methodInUse;

		//是否 menu content 应当延迟被绘制？如果不是，那么 menu content 将 layer 在 menu anchor 上，否则将在所有窗口内容上
		bool m_bShouldDeferPaintingAfterWindowContent;

		bool m_bUseApplicationMenuStack;

		bool m_bApplyWidgetStyleToMenu;

		//存储 pop up 的 local offset ，因为我们需要它在 tick 中计算
		math::float2 m_localPopupPosition;

		math::float2 m_screenPopupPosition;

		class BasicLayoutSlot : public Slot<BasicLayoutSlot>
		{
		public:
			struct SlotBuilderArguments : public Slot<BasicLayoutSlot>::SlotBuilderArguments
			{
				SlotBuilderArguments(std::shared_ptr<BasicLayoutSlot> inSlot)
					: Slot<BasicLayoutSlot>::SlotBuilderArguments(inSlot) {}
				virtual ~SlotBuilderArguments() {};

				//ARGUMENT_ATTRIBUTE(std::optional<SizeParam>, sizeParam)
			};
		public:
			BasicLayoutSlot()
				: Slot<BasicLayoutSlot>(HorizontalAlignment::Stretch, VerticalAlignment::Stretch, Padding(0.0f, 0.0f, 0.0f, 0.0f))
			{}

			virtual ~BasicLayoutSlot() = default;

			void init(std::shared_ptr<Widget> inParentWidget, const SlotBuilderArguments& builderArguments)
			{
				m_parentWidget = inParentWidget;
				if(m_childWidget != nullptr)
					m_childWidget->setParentWidget(inParentWidget);
			}
		};

		static BasicLayoutSlot::SlotBuilderArguments Slot()
		{
			return BasicLayoutSlot::SlotBuilderArguments(std::make_shared<BasicLayoutSlot>());
		}

		//在menu anchor 中安排的 children ，在 widget 的开启/关闭状态的时候改变
		std::vector<std::shared_ptr<BasicLayoutSlot>> m_childrens;
	};
}