#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class Widget;

	class Timer;
	class Window;
	class Renderer;
	struct PointerEvent;
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

		std::shared_ptr<Widget> getCaptorWidget() const;
		//------input------

		void setGlobalPreRotate(float rotation);

		float getGlobalPreRotate() const;
	protected:
		std::shared_ptr<Renderer> m_renderer;

		std::shared_ptr<Timer> m_timer;

		float fps = 0;//one seconds total frame
		float mfps = 0;//one frame's time
	private:
		bool processMouseButtonDownEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		bool processMouseButtonUpEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		bool processMouseMoveEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		std::shared_ptr<Widget> locateWidgetInWindow(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent);

		math::float2 translateCursorPos(math::float2 cursorPos);

		bool m_alreadyExit;

        bool m_focused;

		math::float2 m_lastCursorPos;

		float m_globalRotation = 0;

		//std::weak_ptr<Widget> m_captorWidget;
		std::vector<std::weak_ptr<Widget>> m_captorWidgetsPath;
	};
	std::shared_ptr<Application> CreateApplicationFactory();
}