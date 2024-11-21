#pragma once

#include <Core/UI/WindowWidget.h> //window widget

#include <Renderer/nvrhi.h> //render pass

namespace GuGu {
	class MenuAnchor;
	class ViewportWidget;
	class Button;
	class EditorMainWindow : public WindowWidget
	{
	public:
		EditorMainWindow();

		virtual ~EditorMainWindow();

		struct BuilderArguments : public Arguments<EditorMainWindow>
		{
			BuilderArguments()
				: mClientSize(math::float2(1280.0f, 720.0f))
				, mScreenPosition(math::float2(0.0f, 0.0f))
				, msizingRule(SizingRule::UserSize)
			{}
			~BuilderArguments() = default;

			ARGUMENT_VALUE(WindowType, Type)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			//窗口大小
			ARGUMENT_VALUE(math::float2, ClientSize)

			//屏幕坐标
			ARGUMENT_VALUE(math::float2, ScreenPosition)

			ARGUMENT_VALUE(SizingRule, sizingRule)
		};

		void init(const BuilderArguments& arguments);

		Reply exitApplication();

		Reply miniMizeWindow();

		Reply openFileMenu();

		Reply saveLevel();

		void setRenderTarget(nvrhi::TextureHandle renderTarget);
	private:

		std::shared_ptr<ViewportWidget> m_viewportWidget;

		std::shared_ptr<MenuAnchor> m_openFileMenuAnchor;

		std::shared_ptr<Button> m_saveLevelButton;
	};

	std::shared_ptr<EditorMainWindow> CreateEditorMainWindow();
}