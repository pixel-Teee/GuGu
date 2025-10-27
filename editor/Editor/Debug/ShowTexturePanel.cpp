#include <pch.h>

#include "ShowTexturePanel.h"
#include <Editor/StyleSet/EditorStyleSet.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/ColorBlock.h>
#include <Editor/UI/ColorPicker.h>
//#include <Core/UI/ComplexGradient.h>
#include <Core/UI/Border.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/ImageWidget.h>
//#include <Core/UI/Button.h>
//#include <Core/UI/WindowWidget.h>
//#include <Editor/StyleSet/EditorStyleSet.h>
//
//#include <Application/Application.h>

namespace GuGu {

	ShowTexturePanel::ShowTexturePanel()
	{
		m_debugFontBrush = std::make_shared<Brush>();
		std::shared_ptr<FontCache> fontCache = FontCache::getFontCache();
		m_debugFontBrush->m_texture = fontCache->getFontAtlasTexture();
		m_debugFontBrush->m_startUV = math::double2(0.0f, 0.0f);
		m_debugFontBrush->m_sizeUV = math::double2(1.0f, 1.0f);
		m_debugFontBrush->m_actualSize = math::int2(512, 512);
		m_debugFontBrush->m_margin = Padding(0, 0, 0, 0);
		m_debugFontBrush->m_tiling = false;
		m_debugFontBrush->m_drawAs = BrushDrawType::Type::Image;
		m_debugFontBrush->m_outlineSettings = BrushOutlineSettings(0.0f, math::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
		m_debugFontBrush->m_tintColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
		m_debugFontBrush->m_tintColorStr = "";
	}

	ShowTexturePanel::~ShowTexturePanel()
	{

	}

	void ShowTexturePanel::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		std::shared_ptr<Overlay> overlay = WIDGET_NEW(Overlay)
			+ Overlay::Slot()
			(
				WIDGET_NEW(Border)
				.BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("ColorLevel1");
			}))
			.Content
			(
				NullWidget::getNullWidget()
			)
			)
			+ Overlay::Slot()
			(
				WIDGET_ASSIGN_NEW(VerticalBox, m_showTextureVerticalBox)
			);

			//loop texture vertical box
			initShowTextureVerticalBox();

			m_childWidget->m_childWidget = overlay;
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());

			m_visibilityAttribute = Visibility::Visible;
	}

	void ShowTexturePanel::initShowTextureVerticalBox()
	{
		
		//show texture
		m_showTextureVerticalBox->addSlot()
		.FixedHeight()
		(
			WIDGET_NEW(ImageWidget)
			.brush(m_debugFontBrush)
		);
	}

}