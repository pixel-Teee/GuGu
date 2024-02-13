#pragma once

namespace GuGu {
	class Renderer;
	class Timer;
	class Window;
	class Application {
	public:
		Application();

		void Run();

		virtual void pumpMessage();

		void setExit(bool value);

		virtual void init();

        void setFocused(bool value);

        //virtual void resize(int32_t width, int32_t height);

		std::shared_ptr<Renderer> getRenderer();

        static std::shared_ptr<Application> getApplication();

        bool getFocused();

        std::shared_ptr<Timer> getTimer();

		void calculateFrameStats();

		virtual std::shared_ptr<Window> getWindow(uint32_t index);

		float getFps();
	protected:
		std::shared_ptr<Renderer> m_renderer;

		std::shared_ptr<Timer> m_timer;

		float fps = 0;//one seconds's total frame
		float mfps = 0;//one frame's time
	private:
		bool m_alreadyExit;

        bool m_focused;
	};
	std::shared_ptr<Application> CreateApplicationFactory();
}