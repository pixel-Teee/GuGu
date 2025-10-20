#include <pch.h>

#include "ThemePanel.h"
#include "EditorStyleSet.h"
#include <Core/UI/Overlay.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/ColorBlock.h>
#include <Editor/UI/ColorPicker.h>
//#include <Core/UI/ComplexGradient.h>
//#include <Core/UI/NullWidget.h>
//#include <Core/UI/Button.h>
//#include <Core/UI/WindowWidget.h>
//#include <Editor/StyleSet/EditorStyleSet.h>
//
//#include <Application/Application.h>

namespace GuGu {

	ThemePanel::ThemePanel()
	{

	}

	ThemePanel::~ThemePanel()
	{

	}

	void ThemePanel::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		std::shared_ptr<Overlay> overlay = WIDGET_NEW(Overlay)
		+ Overlay::Slot()
		(
			WIDGET_ASSIGN_NEW(VerticalBox, m_themeVerticalBox)
		);

		//loop add theme
		initThemeVerticalBox();

		m_childWidget->m_childWidget = overlay;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		m_visibilityAttribute = Visibility::Visible;
	}

	void ThemePanel::initThemeVerticalBox()
	{
		std::shared_ptr<EditorStyleSet> editorStyleSet = std::static_pointer_cast<EditorStyleSet>(EditorStyleSet::getStyleSet());

		GuGuUtf8Str themeName = editorStyleSet->getCurrentThemeName();
		//get theme
		Theme loadedTheme = editorStyleSet->getTheme(themeName);
		for (int32_t index = 0; index < loadedTheme.m_colors.size(); ++index)
		{
			GuGuUtf8Str themeKeysName = Theme::ThemeKeysToStr(ThemeKeys(index));
			m_themeVerticalBox->addSlot()
			.StretchHeight(1.0f)
			(
				WIDGET_NEW(HorizontalBox)
				+ HorizontalBox::Slot()
				.FixedWidth()
				(
					WIDGET_NEW(TextBlockWidget)
					.textColor(EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9"))
					.text(themeKeysName)
				)
				+ HorizontalBox::Slot()
				.FixedWidth()
				(
					WIDGET_NEW(ColorBlock)
					.OnMouseButtonDown(this, &ThemePanel::onMouseButtonDownColorBlock, themeKeysName)
					.color(this, &ThemePanel::onGetColorForColorBlock, themeKeysName)
					.Size(math::float2(70.0f, 20.0f))
					.cornerRadius(math::float4(4.0f, 4.0f, 4.0f, 4.0f))
				)
			);
		}
	}

	Reply ThemePanel::onMouseButtonDownColorBlock(const WidgetGeometry& widgetGeometry, const PointerEvent& pointerEvent, GuGuUtf8Str themeKeys)
	{
		createColorPicker(true, themeKeys);//open color picker
		return Reply::Handled();
	}

	Color ThemePanel::onGetColorForColorBlock(GuGuUtf8Str themeKeys) const
	{
		std::shared_ptr<EditorStyleSet> editorStyleSet = std::static_pointer_cast<EditorStyleSet>(EditorStyleSet::getStyleSet());
		Theme loadedTheme = editorStyleSet->getTheme(editorStyleSet->getCurrentThemeName());
		return loadedTheme.m_colors[Theme::StrToThemeKeys(themeKeys)];
	}

	void ThemePanel::createColorPicker(bool bUseAlpha, GuGuUtf8Str themeKeys)
	{
		Color initialColor;
		std::shared_ptr<EditorStyleSet> editorStyleSet = std::static_pointer_cast<EditorStyleSet>(EditorStyleSet::getStyleSet());
		GuGuUtf8Str themeName = editorStyleSet->getCurrentThemeName();
		//get theme
		Theme loadedTheme = editorStyleSet->getTheme(themeName);
		initialColor = loadedTheme.m_colors[Theme::StrToThemeKeys(themeKeys)];

		ColorPickerArgs pickerArgs;
		{
			pickerArgs.m_bUseAlpha = bUseAlpha;
			pickerArgs.m_onColorCommitted = [=](Color inColor){
				//edit color
				std::shared_ptr<EditorStyleSet> editorStyleSet = std::static_pointer_cast<EditorStyleSet>(EditorStyleSet::getStyleSet());
				GuGuUtf8Str themeName = editorStyleSet->getCurrentThemeName();
				//get theme
				Theme& loadedTheme = editorStyleSet->getTheme(themeName);
				loadedTheme.m_colors[Theme::StrToThemeKeys(themeKeys)] = inColor.toFloat4();
			};
			pickerArgs.m_parentWidget = shared_from_this();//todo:fix this
			pickerArgs.m_initialColorOverride = initialColor;
			pickerArgs.m_bOpenAsMenu = false;
		}
		openColorPicker(pickerArgs);
	}


}