#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GuGuUtf8Str.h>

#include <Core/UI/Events.h>//ModifierKeysState depends on it
#include <Core/UI/WidgetPath.h>

//#include <memory>

namespace GuGu {
	class Widget;

	class Timer;
	class Window;
	class Renderer;
	struct PointerEvent;
	struct CharacterEvent;
    struct KeyEvent;
	class Reply;
	class WidgetPath;
	class WindowWidget;
	class Application {
	public:
		Application();

		virtual void init();

		void Run();

		virtual void pumpMessage();

		void setExit(bool value);

        void setFocused(bool value);

		bool getFocused();

        //virtual void resize(int32_t width, int32_t height);

		std::shared_ptr<Renderer> getRenderer();

		std::shared_ptr<Timer> getTimer();

        static std::shared_ptr<Application> getApplication();//this defined in the platform application file    

		void calculateFrameStats();

		virtual std::shared_ptr<Window> getWindow(uint32_t index);

		float getFps() const;

		float getmFps() const;

		static GuGuUtf8Str GetDirectoryWithExecutable();

		//------input------
		virtual bool onMouseDown(const std::shared_ptr<Window>& window, math::float2 cursorPos);

		virtual bool onMouseUp(const std::shared_ptr<Window>& window, math::float2 cursorPos);

		virtual bool onMouseMove(const std::shared_ptr<Window>& window, math::float2 cursorPos);

		virtual bool onKeyChar(const GuGuUtf8Str Character);

		virtual bool onKeyDown(const int32_t keyCode, const uint32_t characterCode);

		std::shared_ptr<Widget> getCaptorWidget() const;

		virtual ModifierKeysState getModifierKeys() const { return ModifierKeysState(); }
		//------input------

		bool hasAnyFocus(std::shared_ptr<const Widget> inWidget) const;

		bool doesWidgetHaveMouseCapture(std::shared_ptr<const Widget> inWidget) const;

		void setGlobalPreRotate(float rotation);

		float getGlobalPreRotate() const;

		void processReply(const Reply& theReply, const WidgetPath& widgetPath);

		void setFocus(const std::shared_ptr<Widget>& widgetToFocus, const WidgetPath& widgetPath);

		std::shared_ptr<Widget> getKeyboardFocusedWidget() const;

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

		std::shared_ptr<Widget> locateWidgetInWindow(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		math::float2 translateCursorPos(math::float2 cursorPos, std::shared_ptr<WindowWidget> inWindowWidget);

		bool m_alreadyExit;

        bool m_focused;

		math::float2 m_lastCursorPos;

		float m_globalRotation = 0;

		//std::weak_ptr<Widget> m_captorWidget;
		WeakWidgetPath m_captorWidgetsPath;

		WeakWidgetPath m_focusWidgetsPath;
	};
	std::shared_ptr<Application> CreateApplicationFactory();
}