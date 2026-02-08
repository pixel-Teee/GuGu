#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/Reflection/Type.h>
#include "PropertyEditorManager.h"//PropertyAccess::Result

namespace GuGu {
	namespace meta {
		class Field;
	}
	class ComboButton;
	class PropertyEditor;
	class Brush;
	struct AssetData;
	class IPropertyHandle;
	class GameObject;
	struct GameObjectLevelRef;
	class PropertyEditorAsset : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyEditorAsset>
		{
			BuilderArguments() 
			: mpropertyHandle(nullptr)
			{}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(std::shared_ptr<IPropertyHandle>, propertyHandle)
			ARGUMENT_VALUE(std::vector<std::shared_ptr<AssetData>>, ownerAssetDataArray)
		};

		static bool supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor);
		static bool supports(const meta::Field* nodeProperty);

		PropertyEditorAsset();

		virtual ~PropertyEditorAsset();

		void init(const BuilderArguments& arguments, const std::shared_ptr<PropertyEditor>& inPropertyEditor);

		std::shared_ptr<Widget> onGetMenuContent();

		std::shared_ptr<Brush> getStatusIcon() const;

		GuGuUtf8Str onGetAssetName() const;

		void getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth);

		PropertyAccess::Result getValue(AssetData& outValue) const;

		PropertyAccess::Result getValue(GameObjectLevelRef& outValue) const;

		void onAssetSelected(const AssetData& assetData);

		void onGameObjectSelected(std::shared_ptr<GameObject> inGameObject);

		void setValue(const AssetData& assetData);

		void setValue(const GameObjectLevelRef& inGameObjectLevelRef);
	private:
		std::shared_ptr<ComboButton> m_assetComboButton;

		std::shared_ptr<PropertyEditor> m_propertyEditor;

		std::shared_ptr<IPropertyHandle> m_propertyHandle;

		//填充可选择的资产使用
		std::vector<std::shared_ptr<AssetData>> m_ownerAssetDataArray;

		meta::Type m_objectClass;

		std::vector<meta::Type> m_allowedClassFilters;

		bool m_isGameObjectLevelRef;
	};
}