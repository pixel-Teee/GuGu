#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Renderer/Color.h>

namespace GuGu {
	class HorizontalBox;
	class VerticalBox;
	class Border;
	//terrain editor
	class TerrainEditorPanel : public CompoundWidget
	{
	public:
		TerrainEditorPanel();

		virtual ~TerrainEditorPanel();

		struct BuilderArguments : public Arguments<TerrainEditorPanel>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		enum TerrainEditorContentType
		{
			Scuplt,
			Paint
		};

		void onGameObjectSelectionChanged(const std::vector<GameObject*>& newSelection, bool bForceRefresh);

		Reply showPanelContent(TerrainEditorContentType inContentType);
	private:
		//地形笔刷选择按钮的容器
		std::shared_ptr<HorizontalBox> m_brushSelectButtons;

		std::shared_ptr<Border> m_brushContent;

		GuGuUtf8Str m_gameObjectSelectionChangedDelegateHandle;

		std::weak_ptr<GameObject> m_terrainObject;
	};
}