#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	namespace meta {
		class Field;
	}
	class ComboButton;
	class PropertyEditor;
	class Brush;
	class PropertyEditorAsset : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyEditorAsset>
		{
			BuilderArguments() { }

			~BuilderArguments() = default;
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
	private:
		std::shared_ptr<ComboButton> m_assetComboButton;
	};
}