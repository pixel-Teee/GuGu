#pragma once

namespace GuGu {
	class Renderer;
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
	protected:
		std::shared_ptr<Renderer> m_renderer;
	private:
		bool m_alreadyExit;

        bool m_focused;
	};
	std::shared_ptr<Application> CreateApplicationFactory();
}