#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GuGuUtf8Str.h>

#include <Core/UI/Events.h>//ModifierKeysState depends on it
#include <Core/UI/WidgetPath.h>
#include <Core/UI/PopupMethodReply.h>
#include <Core/UI/MenuStack.h>
#include <Core/UI/BasicElement.h>
#include <Core/UI/WidgetPath.h>

//#include <memory>

// #include <Core/GamePlay/GameObject.h>
// #include <Core/GamePlay/Level.h>

namespace GuGu {
	enum class WindowActivation : uint8_t
	{
		Activate,
		ActivateByMouse,
		Deactivate
	};

	namespace MouseButtons
	{
		enum Type
		{
			Left = 0,
			Middle,
			Right,
			Thumb01,
			Thumb02
		};
	}
	class DragDropOperation;
	class Widget;
	class IToolTip;
	class Timer;
	class Window;
	class Renderer;
	struct PointerEvent;
	struct CharacterEvent;
    struct KeyEvent;
	class Reply;
	class WidgetPath;
	class WindowWidget;
	class Level;
	class IMenu;
	class Application {
	public:
		Application();

		virtual ~Application();

		virtual void init(std::shared_ptr<WindowWidget> inWindowWidget = nullptr);

		void Run();

		virtual void pumpMessage();

		void setExit(bool value);

        void setFocused(bool value);

		bool getFocused();

        //virtual void resize(int32_t width, int32_t height);

		std::shared_ptr<Renderer> getRenderer();

		std::shared_ptr<Timer> getTimer();

        static std::shared_ptr<Application> getApplication();//this defined in the platform application file    

		virtual void miniMizeWindow(std::shared_ptr<WindowWidget> windowWidget);

		void calculateFrameStats();

		virtual std::shared_ptr<Window> getWindow(uint32_t index);

		const std::vector<std::shared_ptr<WindowWidget>>& getWindows() const;

		float getFps() const;

		float getmFps() const;

		static GuGuUtf8Str GetDirectoryWithExecutable();

		static GuGuUtf8Str GetExecutableFilePath();

		static GuGuUtf8Str GetContentDirectoryWithExecutable();

		//------input------
		virtual bool onMouseDown(const std::shared_ptr<Window>& window, MouseButtons::Type mouseButton, math::float2 cursorPos);

		virtual bool onMouseUp(const std::shared_ptr<Window>& window, MouseButtons::Type mouseButton, math::float2 cursorPos);

		virtual bool onMouseMove(const std::shared_ptr<Window>& window, math::float2 cursorPos);

		virtual bool onMouseDoubleClick(const std::shared_ptr<Window>& window, MouseButtons::Type mouseButton, math::float2 cursorPos);

		virtual bool onKeyChar(const GuGuUtf8Str Character);

		virtual bool onKeyDown(const int32_t keyCode, const uint32_t characterCode);

		virtual bool onWindowActivationChanged(const std::shared_ptr<Window>& window, const WindowActivation);

		virtual void onMovedWindow(const std::shared_ptr<Window>& window, const int32_t x, const int32_t y);

		virtual bool onMouseWheel(const std::shared_ptr<Window>& window, const float delta, const math::float2 cursorPos);

		std::shared_ptr<Widget> getCaptorWidget() const;

		virtual ModifierKeysState getModifierKeys() const { return ModifierKeysState(); }
		//------input------

		virtual WindowZone::Type getWindowZoneForPoint(const std::shared_ptr<Window>& window, const int32_t X, const int32_t Y);

		bool hasAnyFocus(std::shared_ptr<const Widget> inWidget) const;

		bool doesWidgetHaveMouseCapture(std::shared_ptr<const Widget> inWidget) const;

		bool hasCapture() const;

		void setGlobalPreRotate(float rotation);

		float getGlobalPreRotate() const;

		void processReply(const Reply& theReply, const WidgetPath& widgetPath, const PointerEvent* inMouseEvent = nullptr);

		void setFocus(const std::shared_ptr<Widget>& widgetToFocus, const WidgetPath& widgetPath);

		std::shared_ptr<Widget> getKeyboardFocusedWidget() const;

		std::shared_ptr<Widget> getFocusedWidget() const;

		//true 表示找到
		static bool findPathToWidget(const std::vector<std::shared_ptr<WindowWidget>>& windowsToSearch,
			std::shared_ptr<Widget> inWidget,
			WidgetPath& outWidgetPath,
			Visibility visibilityFilter = Visibility::Visible);

		//查找一个确切的 widget 并且生成一个完整的路径到它，这是一个比较慢的操作
		//visibility filter 表示这个路径内必须有这个 visibility 的 widget
		bool generatePathToWidgetUnchecked(std::shared_ptr<Widget> inWidget, WidgetPath& outWidgetPath,
			Visibility visibilityFilter = Visibility::Visible) const;

		//去创建 window widget 相应的 native window
		virtual void makeWindow(std::shared_ptr<WindowWidget> windowWidget);

		//创建 window widget 相应的 swap chain 和 surface
		virtual void showWindow(std::shared_ptr<WindowWidget> windowWidget);

		std::vector<std::shared_ptr<WindowWidget>>& getWidowWidgets();

		void OnResizeChanged(std::shared_ptr<Window> inWindow);

		void requestDestroyWindow(std::shared_ptr<WindowWidget> windowToDestroy);

		//parent widget (菜单的父控件)，通过查找父控件的 widget path 中是否有一个打开的菜单来决定新菜单的等级
		//可选的完整的父亲的控件路径，如果提供的话，如果没提供的话，那么就会生成一条路径到InParentWidget
		std::shared_ptr<IMenu> pushMenu(const std::shared_ptr<Widget>& inParentWidget,
			const WidgetPath& inOwnerPath,
			const std::shared_ptr<Widget>& inContent,
			const math::float2& summonLocation,
			const bool bFocusImmediately = true,
			const math::float2& summonLocationSize = math::float2(0, 0),
			std::optional<PopupMethod> method = std::optional<PopupMethod>(),
			const bool bIsCollapsedByParent = true);

		virtual math::float2 calculatePopupWindowPosition(const math::box2& inAnchor, const math::float2& inSize, bool bAutoAdjustForDPIScale = true,
			const math::float2& inProposedPlacement = math::float2(0, 0), const Orientation orientation = Orientation::Vertical) const;

		virtual math::box2 getWorkArea(const math::box2& currentWindow) const;

		std::shared_ptr<Widget> locateWidgetInWindow(const std::shared_ptr<Window>& window, const math::float2& screenSpacePosition);

		virtual math::float2 getCursorPos() const;

		std::shared_ptr<IToolTip> makeToolTip(const Attribute<GuGuUtf8Str>& toolTipText);

		std::shared_ptr<IToolTip> makeToolTip(const GuGuUtf8Str& toolTipText);

		void updateToolTip(const MenuStack& menuStack, bool bCanSpawnNewToolTip);

		WidgetPath locateWidgetUnderMouse(math::float2 screenSpaceMouseCoordinate, const std::vector<std::shared_ptr<WindowWidget>>& windows);

		struct DragDetectionState
		{
			DragDetectionState() {}

			DragDetectionState(const WidgetPath& pathToWidget, Key dragButton, const math::float2& startLocation)
				: m_detectDragForWidget(pathToWidget)
				, m_dragStartLocation(startLocation)
				, m_triggerButton(dragButton)
			{}

			//如果非空，一个小控件请求我们检测此小控件中触发的拖动，并发送一个OnDragDetected的事件
			WeakWidgetPath m_detectDragForWidget;

			math::float2 m_dragStartLocation = math::float2(0, 0);
			Key m_triggerButton = Keys::Invalid;
		};

		struct ActiveTooltipInfo
		{
			void reset();

			std::weak_ptr<IToolTip> m_toolTip;

			std::weak_ptr<Widget> m_tooltipVisualizer;

			std::weak_ptr<Widget> m_sourceWidget;

			math::float2 m_desiredLocation = math::float2(0, 0);

			double m_summoTime = 0.0;
		};

		ActiveTooltipInfo m_activeTooltipInfo;

		void closeTooltip();

		void showTooltip(const std::shared_ptr<IToolTip>& inTooltip, const math::float2& inLocation);

		std::shared_ptr<WindowWidget> getOrCreateTooltipWindow();

		void resetTooltipWindow();
	protected:
		std::shared_ptr<Renderer> m_renderer;

		std::shared_ptr<Timer> m_timer;

		float fps = 0;//one seconds total frame
		float mfps = 0;//one frame's time

		std::vector<std::shared_ptr<WindowWidget>> m_windowWidgets;
	private:
		bool processMouseButtonDownEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		bool processMouseButtonUpEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		bool processMouseMoveEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		bool processKeyCharEvent(const CharacterEvent& inCharacterEvent);

		bool processKeyDownEvent(const KeyEvent& inKeyEvent);

		bool processWindowActivatedEvent(const WindowActivateEvent& activateEvent);

		bool processMouseButtonDoubleClickEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		bool processMouseWheelEvent(const std::shared_ptr<Window>& window, const PointerEvent& inWheelEvent);

		math::float2 translateCursorPos(math::float2 cursorPos, std::shared_ptr<WindowWidget> inWindowWidget);

		bool m_alreadyExit;

        bool m_focused;

		math::float2 m_lastCursorPos;

		float m_globalRotation = 0;

		//上一次事件处理的控件路径，用于判断鼠标移动进一个按钮或者移出一个按钮
		WeakWidgetPath m_widgetsUnderPointerLastEvent;

		//std::weak_ptr<Widget> m_captorWidget;
		WeakWidgetPath m_captorWidgetsPath;

		WeakWidgetPath m_focusWidgetsPath;

		float m_dragTriggerDistance;

		std::shared_ptr<DragDetectionState> m_dragstate;

		std::shared_ptr<DragDropOperation> m_dragDropContent;

		MenuStack m_menuStack;

		std::weak_ptr<WindowWidget> m_toolTipWindowPtr;

		std::weak_ptr<WindowWidget> m_dragDropWindowPtr;
	};
	std::shared_ptr<Application> CreateApplicationFactory();
}