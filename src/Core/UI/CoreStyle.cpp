#include <pch.h>

#include "CoreStyle.h"
#include "Brush.h"
#include "StyleSetCenter.h"

namespace GuGu {
	CoreStyle::CoreStyle()
	{
		//repeat sampler
		//auto defaultSamplerDesc = nvrhi::SamplerDesc()
		//	.setAllFilters(false)
		//	.setAllAddressModes(nvrhi::SamplerAddressMode::Clamp);
		//m_defaultSeampler = m_device->createSampler(defaultSamplerDesc);
		//
		//auto repeatSamplerDesc = nvrhi::SamplerDesc()
		//	.setAllFilters(false)
		//	.setAllAddressModes(nvrhi::SamplerAddressMode::Repeat);
		//m_repeatSeampler = m_device->createSampler(repeatSamplerDesc);
		std::shared_ptr<Brush> white = std::make_shared<Brush>();
		white->m_tiling = true;
		white->m_texturePath = u8"asset/white.png";
		m_brushes.insert({ u8"white", white });

		std::shared_ptr<Brush> checkerBoard = std::make_shared<Brush>();
		checkerBoard->m_tiling = true;
		checkerBoard->m_texturePath = u8"asset/CheckerBoard.png";
		m_brushes.insert({ u8"CheckerBoard", checkerBoard });

		std::shared_ptr<Brush> fun = std::make_shared<Brush>();
		fun->m_tiling = false;
		fun->m_texturePath = u8"asset/fun.jpg";
		m_brushes.insert({ u8"fun", fun });

		std::shared_ptr<Brush> biscuit = std::make_shared<Brush>();
		biscuit->m_tiling = false;
		biscuit->m_texturePath = u8"asset/biscuit.jpg";
		m_brushes.insert({ u8"biscuit", biscuit });

		std::shared_ptr<Brush> textBlockHighlightShape = std::make_shared<Brush>();
		textBlockHighlightShape->m_tiling = false;
		textBlockHighlightShape->m_texturePath = u8"asset/MinimumWindow/TextBlockHighlightShape.png";
		m_brushes.insert({ u8"textBlockHighlightShape", textBlockHighlightShape });

		std::shared_ptr<TextInfo> textInfo = std::make_shared<TextInfo>();
		textInfo->m_size = 18;//12pt
		textInfo->m_textPath = u8"asset/NotoSansSC-Regular.ttf";
		textInfo->m_name = u8"NotoSansSC-Regular.ttf";
		std::shared_ptr<TextBlockStyle> textBlockStyle = std::make_shared<TextBlockStyle>(textInfo, math::float4(0.59f, 0.98f, 0.59f, 1.0f), math::float2(1.0f, 0.0f), math::float4(0.0f, 1.0f, 0.0f, 0.4f));
		m_styles.insert({ "normalText", textBlockStyle });
		textBlockStyle->setHighLightShape(textBlockHighlightShape);

		//minimum window
		std::shared_ptr<Brush> closeButtonNormal = std::make_shared<Brush>();
		closeButtonNormal->m_tiling = false;
		closeButtonNormal->m_texturePath = u8"asset/MinimumWindow/CloseButton.png";
		m_brushes.insert({ u8"CloseButtonNormal", closeButtonNormal });

		std::shared_ptr<Brush> closeButtonPressed = std::make_shared<Brush>();
		closeButtonPressed->m_tiling = false;
		closeButtonPressed->m_texturePath = u8"asset/MinimumWindow/CloseButtonPressed.png";
		m_brushes.insert({ u8"CloseButtonPressed", closeButtonPressed });

		std::shared_ptr<Brush> background = std::make_shared<Brush>();
		background->m_tiling = false;
		background->m_texturePath = u8"asset/MinimumWindow/background.png";
		m_brushes.insert({ u8"background", background });

		std::shared_ptr<Brush> headerBackground = std::make_shared<Brush>();
		headerBackground->m_tiling = false;
		headerBackground->m_texturePath = u8"asset/MinimumWindow/headerBackground.png";
		m_brushes.insert({ u8"headerBackground", headerBackground });

		std::shared_ptr<ButtonStyle> closeButtonStyle = std::make_shared<ButtonStyle>();
		closeButtonStyle->setNormal(closeButtonNormal);
		closeButtonStyle->setPressed(closeButtonPressed);
		m_styles.insert({ "closeButton", closeButtonStyle });

		//check box
		std::shared_ptr<Brush> checkboxBackground = std::make_shared<Brush>();
		checkboxBackground->m_tiling = false;
		checkboxBackground->m_texturePath = u8"asset/MinimumWindow/checkbackground.png";
		m_brushes.insert({ u8"checkboxBackground", checkboxBackground });

		std::shared_ptr<Brush> checkedImage = std::make_shared<Brush>();
		checkedImage->m_tiling = false;
		checkedImage->m_texturePath = u8"asset/MinimumWindow/checked.png";
		m_brushes.insert({ u8"checkedImage", checkedImage });

		std::shared_ptr<Brush> uncheckedImage = std::make_shared<Brush>();
		uncheckedImage->m_tiling = false;
		uncheckedImage->m_texturePath = u8"asset/MinimumWindow/unchecked.png";
		m_brushes.insert({ u8"uncheckedImage", uncheckedImage });

		std::shared_ptr<CheckBoxStyle> checkBoxStyle = std::make_shared<CheckBoxStyle>();
		checkBoxStyle->setBackgroundImage(checkboxBackground);
		checkBoxStyle->setCheckedImage(checkedImage);
		checkBoxStyle->setUncheckedImage(uncheckedImage);
		m_styles.insert({ "checkBox", checkBoxStyle });

		std::shared_ptr<Brush> normalBarImage = std::make_shared<Brush>();
		normalBarImage->m_tiling = true;
		normalBarImage->m_texturePath = u8"asset/MinimumWindow/bar.png";
		normalBarImage->m_drawAs = BrushDrawType::RoundedBox;
		normalBarImage->m_outlineSettings = BrushOutlineSettings(math::float4(4.4f, 4.4f, 4.4f, 4.4f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
		normalBarImage->m_margin = Padding(0.0f, 0.0f, 0.0f, 0.0f);
		m_brushes.insert({ u8"normalBarImage", normalBarImage });

		std::shared_ptr<Brush> thumbImage = std::make_shared<Brush>();
		thumbImage->m_tiling = true;//rounded box 需要(0.0, 1.0)的UV
		thumbImage->m_texturePath = u8"asset/MinimumWindow/thumb.png";
		thumbImage->m_drawAs = BrushDrawType::RoundedBox;
		//corner radius
		//outline color
		//outline width
		thumbImage->m_outlineSettings = BrushOutlineSettings(math::float4(7.4f, 7.4f, 7.4f, 7.4f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 1.5f);
		thumbImage->m_margin = Padding(0.0f, 0.0f, 0.0f, 0.0f);
		m_brushes.insert({ u8"thumbImage", thumbImage });

		std::shared_ptr<SliderStyle> sliderStyle = std::make_shared<SliderStyle>();
		sliderStyle->setNormalBarImage(normalBarImage);
		sliderStyle->setNormalThumbImage(thumbImage);
		sliderStyle->setBarThickness(10.0f);
		m_styles.insert({ "slider", sliderStyle });

		//editable text
		std::shared_ptr<Brush> selectionBackground = std::make_shared<Brush>();
		selectionBackground->m_tiling = false;
		selectionBackground->m_texturePath = u8"asset/MinimumWindow/thumb.png";
		m_brushes.insert({ "selectionBackground", selectionBackground });

		//editable text Box
		std::shared_ptr<Brush> editableTextBoxBackgroundImageNormal = std::make_shared<Brush>();
		editableTextBoxBackgroundImageNormal->m_tiling = false;
		editableTextBoxBackgroundImageNormal->m_texturePath = u8"asset/MinimumWindow/headerBackground.png";
		m_brushes.insert({ "editableTextBoxBack.backgroundImageNormal", editableTextBoxBackgroundImageNormal });

		std::shared_ptr<EditableTextBoxStyle> editableTextBoxStyle = std::make_shared<EditableTextBoxStyle>();
		editableTextBoxStyle->setBackgroundImageNormal(editableTextBoxBackgroundImageNormal);
		m_styles.insert({ "EditableTextBox", editableTextBoxStyle });

		//spin box
		std::shared_ptr<Brush> spinBoxBackgroundBrushImage = std::make_shared<Brush>();
		spinBoxBackgroundBrushImage->m_tiling = false;
		spinBoxBackgroundBrushImage->m_texturePath = u8"asset/MinimumWindow/SpinBoxBackground.png";
		m_brushes.insert({ "spinBoxBackground", spinBoxBackgroundBrushImage });

		std::shared_ptr<SpinBoxStyle> spinBoxStyle = std::make_shared<SpinBoxStyle>();
		spinBoxStyle->setBackgroundBrush(spinBoxBackgroundBrushImage);
		m_styles.insert({ "SpinBox", spinBoxStyle });

		//scroll bar
		std::shared_ptr<Brush> normalThumbImage = std::make_shared<Brush>();
		normalThumbImage->m_tiling = false;
		normalThumbImage->m_texturePath = u8"asset/MinimumWindow/thumb.png";
		normalThumbImage->m_drawAs = BrushDrawType::RoundedBox;
		normalThumbImage->m_outlineSettings = BrushOutlineSettings(math::float4(4.4f, 4.4f, 4.4f, 4.4f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
		normalThumbImage->m_margin = Padding(0.0f, 0.0f, 0.0f, 0.0f);
		m_brushes.insert({ "ScrollBarNormalThumbImage", normalThumbImage });

		std::shared_ptr<ScrollBarStyle> scrollBarStyle = std::make_shared<ScrollBarStyle>();
		scrollBarStyle->setNormalThumbImage(normalThumbImage);
		scrollBarStyle->setThickness(8.0f);
		m_styles.insert({ "ScrollBar", scrollBarStyle });

		//no appearance
		std::shared_ptr<Brush> noResource = std::make_shared<Brush>();
		noResource->m_tiling = false;
		noResource->m_texturePath = "";
		noResource->m_drawAs = BrushDrawType::NoDrawType;
		noResource->m_actualSize = math::int2(0, 0);
		noResource->m_margin = Padding(0.0f, 0.0f, 0.0f, 0.0f);
		m_brushes.insert({ "NoBorder" , noResource });

		std::shared_ptr<ButtonStyle> noBorder = std::make_shared<ButtonStyle>();
		noBorder->setNormal(noResource);
		noBorder->setHovered(noResource);
		noBorder->setDisabled(noResource);
		noBorder->setPressed(noResource);
		noBorder->setPressedPadding(Padding(0, 0, 0, 0));

		m_styles.insert({ "NoBorder", noBorder });

		//tree arrow
		std::shared_ptr<Brush> expandedArrow = std::make_shared<Brush>();
		expandedArrow->m_tiling = false;
		expandedArrow->m_texturePath = u8"asset/MinimumWindow/expandedArrow.png";//16x16
		m_brushes.insert({ u8"expandedArrow", expandedArrow });

		std::shared_ptr<Brush> expandedArrowCollapsed = std::make_shared<Brush>();
		expandedArrowCollapsed->m_tiling = false;
		expandedArrowCollapsed->m_texturePath = u8"asset/MinimumWindow/expandedArrowCollapsed.png";//16x16
		m_brushes.insert({ u8"expandedArrowCollapsed", expandedArrowCollapsed });

		//combo button
		std::shared_ptr<Brush> comboButtonButtonBrush = std::make_shared<Brush>();
		comboButtonButtonBrush->m_tintColor = math::float4(0.3f, 0.4f, 0.2f, 1.0f);
		std::shared_ptr<ButtonStyle> comboButtonButton = std::make_shared<ButtonStyle>();
		comboButtonButton->setPressedPadding(Padding(8.0f, 2.0f, 8.0f, 0.0f));
		comboButtonButton->setNormal(comboButtonButtonBrush);
		comboButtonButton->setPressed(comboButtonButtonBrush);

		std::shared_ptr<ComboButtonStyle> comboButtonStyle = std::make_shared<ComboButtonStyle>();
		comboButtonStyle->setButtonStyle(comboButtonButton);
		comboButtonStyle->setContentPadding(Padding(0.0f));
		comboButtonStyle->setMenuBorderPadding(0.0f);
		comboButtonStyle->setDownArrowImage(expandedArrow);
		comboButtonStyle->setDownArrowAlignment(VerticalAlignment::Center);
		m_styles.insert({ "comboButton", comboButtonStyle });

		//combo row
		std::shared_ptr<Brush> eventRow = std::make_shared<Brush>();
		eventRow->m_tintColor = math::float4(0.3f, 0.4f, 0.2f, 1.0f);
		std::shared_ptr<Brush> oddRow = std::make_shared<Brush>();
		oddRow->m_tintColor = math::float4(0.3f, 0.6f, 0.2f, 1.0f);
		std::shared_ptr<TableRowStyle> comboRow = std::make_shared<TableRowStyle>();
		comboRow->setEvenRowBackgroundBrush(eventRow);
		comboRow->setOddRowBackgroundBrush(oddRow);
		m_styles.insert({ "comboBox.Row", comboRow });
		m_styles.insert({ "tableView.Row", comboRow });

		//combo box
		std::shared_ptr<ComboBoxStyle> comboBox = std::make_shared<ComboBoxStyle>();
		comboBox->setComboButtonStyle(comboButtonStyle);
		m_styles.insert({ "comboBox", comboBox });

		//color wheel
		std::shared_ptr<Brush> colorWheel = std::make_shared<Brush>();
		colorWheel->m_tiling = false;
		colorWheel->m_texturePath = u8"asset/MinimumWindow/ColorWheel.png";
		m_brushes.insert({ u8"colorWheel.HueValueCircle", colorWheel });

		std::shared_ptr<Brush> colorSelector = std::make_shared<Brush>();
		colorSelector->m_tiling = false;
		colorSelector->m_texturePath = u8"asset/MinimumWindow/Circle.png";
		m_brushes.insert({ u8"colorWheel.Selector", colorSelector });

		//color block
		//std::shared_ptr<Brush> colorBlock = std::make_shared<Brush>();
		m_brushes.insert({ u8"colorPicker.AlphaBckground", checkerBoard });

		//splitter style
		{
			//gray color
			math::float4 grayColor = math::float4(0.33f, 0.33f, 0.35f, 1.0f);

			//gray highlight color
			math::float4 grayHightlightColor = math::float4(0.24f, 0.24f, 0.24f, 1.0f);

			std::shared_ptr<SplitterStyle> splitterStyle = std::make_shared<SplitterStyle>();
			std::shared_ptr<Brush> normalBrush = std::make_shared<Brush>();
			normalBrush->m_tiling = false;
			normalBrush->m_texturePath = u8"asset/white.png";
			normalBrush->m_tintColor = grayColor;
			normalBrush->m_drawAs = BrushDrawType::Type::Image;

			std::shared_ptr<Brush> highlightBrush = std::make_shared<Brush>();
			highlightBrush->m_tiling = false;
			highlightBrush->m_texturePath = u8"asset/white.png";
			highlightBrush->m_tintColor = grayHightlightColor;
			highlightBrush->m_drawAs = BrushDrawType::Type::Image;

			m_styles.insert({ u8"splitter", splitterStyle });
		}
	}
	CoreStyle::~CoreStyle()
	{
	}
	std::shared_ptr<StyleSet> CoreStyle::getStyleSet()
	{
		//static std::shared_ptr<StyleSet> coreStyleSet = std::make_shared<CoreStyle>();
		//return coreStyleSet;
		std::shared_ptr<StyleSet> coreStyleSet = StyleSetCenter::GetStyle("CoreStyleSet");
		return coreStyleSet;
	}
}