#pragma once

namespace GuGu {
	class Renderer {
	public:
		Renderer();

		virtual ~Renderer();

		virtual void init();

		virtual void onRender();

		virtual void onDestroy();

        virtual void onResize(int32_t width, int32_t height);
	};

	//static std::shared_ptr<Renderer> CreateRendererFactory();
}