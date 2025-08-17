#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"

namespace GuGu {
	//class Slot;
	class Overlay;
	class Brush;
	class ElementList;
	class WidgetGeometry;
	class ArrangedWidgetArray;
	class WindowActivateEvent;

	class Window;

	class WindowWidget;

	using OnWindowClosed = std::function<void(const std::shared_ptr<WindowWidget>&)>;

	//窗口如何缩放
	enum class SizingRule
	{
		FixedSize, //固定大小

		AutoSized, //由内容的大小决定

		UserSize //由用户决定
	};
	
	//简单的overlay layer去允许内容叠加在一个窗口或者类似的控件商
	class OverlayPopupLayer : public PopupLayer
	{
	public:
		OverlayPopupLayer(const std::shared_ptr<WindowWidget>& initHostWindow, const std::shared_ptr<Widget>& initPopupContent, std::shared_ptr<Overlay> initOverlay);

		virtual void remove() override;
		virtual math::box2 getAbsoluteClientRect() override;
	private:
		std::shared_ptr<WindowWidget> m_hostWindow;
		std::shared_ptr<Overlay> m_overlay;
	};

	class WindowWidget : public Widget
	{
	public:
		enum WindowType
		{
			Normal,
			Menu,
			ToolTip,
			CursorDecorator
		};
		WindowWidget();

		virtual ~WindowWidget();

		struct BuilderArguments : public Arguments<WindowWidget>
		{
			BuilderArguments()
				: mClientSize(math::float2(1280.0f, 720.0f))
				, mScreenPosition(math::float2(0.0f, 0.0f))
				, msizingRule(SizingRule::UserSize)
				, mFocusWhenFirstShown(true)
				, muserResizeBorder(Padding(5, 5, 5, 5))
				, mIsPopupWindow(false)
			{}
			~BuilderArguments() = default;

			ARGUMENT_VALUE(WindowType, Type)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			//窗口大小
			ARGUMENT_VALUE(math::float2, ClientSize)

			//屏幕坐标
			ARGUMENT_VALUE(math::float2, ScreenPosition)

			ARGUMENT_VALUE(SizingRule, sizingRule)

			//当窗口显示的时候，是否立马获取焦点？
			ARGUMENT_VALUE(bool, FocusWhenFirstShown)

			ARGUMENT_VALUE(Padding, userResizeBorder)

			ARGUMENT_VALUE(bool, IsPopupWindow)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		virtual std::shared_ptr<PopupLayer> onVisualizePopup(const std::shared_ptr<Widget>& popupContent) override;

		math::int2 getViewportSize();

		void setChildWidget(std::shared_ptr<Widget> widget);

		void assocateWithNativeWindow(std::shared_ptr<Window> nativeWindow);

		std::shared_ptr<Window> getNativeWindow();

		void setCachedScreenPosition(math::float2 newPosition);

		void resize(math::float2 newSize);

		math::float2 getPositionInScreen() const;

		WidgetGeometry getWindowGeometryInScreen() const;

		//static Slot::SlotBuilderArguments SingleSlot() { return Slot::SlotBuilderArguments(std::make_shared<Slot>()); };

		math::box2 getClientRectInScreen() const;

		math::float2 getClientSizeInScreen() const;

		math::affine2 getLocalToScreenTransform() const;

		void moveWindowTo(math::float2 newPosition);

		bool isAutoSized() const;

		void requestDestroyWindow();

		void destroyNativeWindow();

		virtual bool OnIsActivateChanged(const WindowActivateEvent& activateEvent);

		virtual bool supportsKeyboardFocus() const;

		void setWidgetToFocusOnActivate(std::shared_ptr<Widget> inWidget);

		bool isFocusedInitially() const;

		void setParentWindow(const std::shared_ptr<WindowWidget>& inParentWindow);

		std::weak_ptr<WindowWidget> getParentWindow() const;

		WindowZone::Type getCurrentWindowZone(math::float2 localMousePosition);

		bool isRegularWindow() const;

		void hideWindow();

		void setContent(const std::shared_ptr<Widget>& inWidget);

		static std::shared_ptr<WindowWidget> makeCursorDecorator();

		void addChildWindow(const std::shared_ptr<WindowWidget>& inChildWindow);

		std::vector<std::shared_ptr<WindowWidget>>& getChildWindows();

	protected:
		std::shared_ptr<Window> m_nativeWindow;
		std::shared_ptr<SingleChildSlot> m_childWidget;
		WindowType m_windowType;
		std::shared_ptr<Brush> m_defaultBrush;//for window background

		math::float2 m_screenPosition;//窗口的屏幕坐标

		math::float2 m_size;//窗口在屏幕空间的内容区域的大小，只有窗口使用这个，不使用fixed size

		SizingRule m_sizingRule;

		//window overlay widget
		std::shared_ptr<Overlay> m_windowOverlay;

		//提供给tooltips, drag-drop decorators 功能，那些不需要创建窗口的
		std::shared_ptr<PopupLayer> m_popupLayer;

		//用于转移焦点的控件，当这个窗口变的激活，这个被用来恢复焦点到一个控件，在一个popup销毁后
		std::weak_ptr<Widget> m_widgetToFocusOnActivate;

		std::weak_ptr<Widget> m_widgetToFocusOnDeactivate;

		bool m_bFocusWhenFirstShown;

		std::weak_ptr<WindowWidget> m_parentWindow;

		Padding m_userResizeBorder;//用户可以被抓取的边界尺寸

		WindowZone::Type m_windowZone;//鼠标现在悬浮的窗口区域

		float m_titleBarSize;

		bool m_bIsPopupWindow;

		std::vector<std::shared_ptr<WindowWidget>> m_childWindows;
	};
}