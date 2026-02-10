#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Renderer/Color.h>
#include <Core/AssetManager/AssetData.h>

namespace GuGu {
	class HorizontalBox;
	class VerticalBox;
	class Border;
	class Overlay;
	class ObjectDetails;
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

		std::optional<int32_t> onGetValue(const GuGuUtf8Str& variableName) const;

		void onValueChanged(float variableValue, GuGuUtf8Str variableName);

		void onValueCommitted(float variableValue, TextCommit::Type commitType, GuGuUtf8Str variableName);

		Reply recreateTerrain();

		float getBrushSize() const;

		void onBrushSizeChanged(float inNewBrushSize);

		float getBrushStrength() const;

		void onBrushStrengthChanged(float inNewBrushStrength);

		void onBrushTypeSelect(GuGuUtf8Str inOption, SelectInfo::Type);

		GuGuUtf8Str getBrushTypeStr() const;

		void onPaintAssetSelected(const AssetData& inAsset);

		void paintVegetation(math::float3 inBrushPos);
	private:
		//地形笔刷选择按钮的容器
		std::shared_ptr<HorizontalBox> m_brushSelectButtons;

		std::shared_ptr<Border> m_brushContent;

		std::shared_ptr<Overlay> m_sculptOverlay;
		std::shared_ptr<VerticalBox> m_sculptVerticalBox;

		GuGuUtf8Str m_gameObjectSelectionChangedDelegateHandle;

		std::weak_ptr<GameObject> m_terrainObject;

		//cache variable value
		std::map<GuGuUtf8Str, meta::Variant> m_cacheVariableValues;

		float m_brushSize;

		float m_brushStrength;

		int32_t m_brushType;

		std::vector<GuGuUtf8Str> m_brushTypeStrSources;

		//paint
		std::shared_ptr<VerticalBox> m_paintPanel;

		AssetData m_paintVegetation;
	};
}