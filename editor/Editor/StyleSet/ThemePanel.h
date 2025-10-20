#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Renderer/Color.h>
//theme editor widget
namespace GuGu {
	class VerticalBox;
	class ThemePanel : public CompoundWidget
	{
	public:
		ThemePanel();

		virtual ~ThemePanel();

		struct BuilderArguments : public Arguments<ThemePanel>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		void initThemeVerticalBox();

		Reply onMouseButtonDownColorBlock(const WidgetGeometry& widgetGeometry, const PointerEvent& pointerEvent, GuGuUtf8Str themeKeys);

		Color onGetColorForColorBlock(GuGuUtf8Str themeKeys) const;

		void createColorPicker(bool bUseAlpha, GuGuUtf8Str themeKeys);

	private:
		std::shared_ptr<VerticalBox> m_themeVerticalBox;
	};
}