#pragma once

namespace GuGu {
	class Renderer {
	public:
		Renderer();

		virtual ~Renderer();

		virtual void init();

		virtual void onRender();

		virtual void onDestroy();
	};

	//static std::shared_ptr<Renderer> CreateRendererFactory();
}