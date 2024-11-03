#pragma once

#include <Core/UI/WindowWidget.h> //window widget

namespace GuGu {
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
	private:
	};

	std::shared_ptr<EditorMainWindow> CreateEditorMainWindow();
}