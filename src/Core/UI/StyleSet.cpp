#include <pch.h>

#include "Brush.h"
#include "StyleSet.h"

namespace GuGu {
	StyleSet::StyleSet()
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
        
		std::shared_ptr<TextInfo> textInfo = std::make_shared<TextInfo>();
		textInfo->m_size = 18;//12pt
		textInfo->m_textPath = u8"asset/NotoSansSC-Regular.ttf";
		textInfo->m_name = u8"NotoSansSC-Regular.ttf";
		std::shared_ptr<TextBlockStyle> textBlockStyle = std::make_shared<TextBlockStyle>(textInfo, math::float4(0.59f, 0.98f, 0.59f, 1.0f), math::float2(1.0f, 0.0f), math::float4(0.0f, 1.0f, 0.0f, 0.4f));
		m_styles.insert({ "normalText", textBlockStyle });

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

		//checkbox
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
		normalBarImage->m_tiling = false;
		normalBarImage->m_texturePath = u8"asset/MinimumWindow/bar.png";
		m_brushes.insert({ u8"normalBarImage", normalBarImage });

		std::shared_ptr<Brush> thumbImage = std::make_shared<Brush>();
		thumbImage->m_tiling = false;
		thumbImage->m_texturePath = u8"asset/MinimumWindow/thumb.png";
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
	}
	StyleSet::~StyleSet()
	{
	}
	std::shared_ptr<Brush> StyleSet::getBrush(const GuGuUtf8Str& str)
	{
		auto it = m_brushes.find(str);
		if (it != m_brushes.end())
			return it->second;

		GuGu_LOGE("don't find brush!");

		return nullptr;
	}
	std::shared_ptr<Brush> StyleSet::getNoBrush()
	{
		return std::make_shared<Brush>();
	}
	void StyleSet::getBrush(std::vector<std::shared_ptr<Brush>>& brush)
	{
		for (auto& it : m_brushes)
		{
			brush.push_back(it.second);
		}
	}
	std::shared_ptr<StyleSet> StyleSet::getStyle()
	{
		static std::shared_ptr<StyleSet> styleInsance = std::make_shared<StyleSet>();
		return styleInsance;
	}
	void StyleSet::clear()
	{
		m_brushes.clear();
		m_styles.clear();
	}
}