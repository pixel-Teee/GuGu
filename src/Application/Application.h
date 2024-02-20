#pragma once

namespace GuGu {
	class Timer;
	class Window;
	class Renderer;
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
	protected:
		std::shared_ptr<Renderer> m_renderer;

		std::shared_ptr<Timer> m_timer;

		float fps = 0;//one seconds total frame
		float mfps = 0;//one frame's time
	private:
		bool m_alreadyExit;

        bool m_focused;
	};
	std::shared_ptr<Application> CreateApplicationFactory();
}