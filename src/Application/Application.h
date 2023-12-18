#pragma once

namespace GuGu {
	class Renderer;
	class Application {
	public:
		Application();

		void Run();

		virtual bool pumpMessage();

		void setExit(bool value);

		virtual void init();
	protected:
		std::shared_ptr<Renderer> m_renderer;
	private:
		bool m_alreadyExit;		
	};
	std::shared_ptr<Application> CreateApplicationFactory();
}