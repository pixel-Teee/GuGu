#pragma once

namespace GuGu {
	class Application {
	public:
		Application();

		void Run();

		virtual void pumpMessage();

		void setExit(bool value);
	private:

		bool m_alreadyExit;
	};
	std::shared_ptr<Application> CreateApplicationFactory();
}