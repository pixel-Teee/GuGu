#pragma once

#include <Core/UI/WindowWidget.h> //window widget

#include <Renderer/nvrhi.h> //render pass

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		class SceneOutliner;
	}
	class Border;
	class MenuAnchor;
	class ViewportWidget;
	class Button;
	class CheckBox;
	class ObjectDetails;
	class GameObject;
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

		Reply openFileMenu();

		Reply openLevel();

		Reply undo();

		Reply redo();

		void switchEditorAndRuntime(CheckBoxState inCheckBoxState);

		void setRenderTarget(nvrhi::TextureHandle renderTarget);

		void onItemSelect(const std::vector<GameObject*>&, bool);
	private:

		std::shared_ptr<ViewportWidget> m_viewportWidget;

		std::shared_ptr<MenuAnchor> m_openFileMenuAnchor;

		std::shared_ptr<Button> m_saveLevelButton;

		std::shared_ptr<CheckBox> m_switchEditorAndRuntime;

		std::shared_ptr<ObjectDetails> m_objectDetails;

		std::shared_ptr<SceneOutlinerNameSpace::SceneOutliner> m_sceneOutliner;

		std::shared_ptr<Border> m_undoBorder;

		std::shared_ptr<Border> m_redoBorder;

		math::float4 getUndoColor() const;

		math::float4 getRedoColor() const;
	};

	std::shared_ptr<EditorMainWindow> CreateEditorMainWindow();
}