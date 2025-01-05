#include <pch.h>

#include "EditorStyleSet.h"

#include <Core/UI/StyleSetCenter.h>
#include <Core/UI/Brush.h>

namespace GuGu {
#define ADD_COLOR(color) m_colors.insert({#color, color})
#define ADD_IMAGE(texture) \
	std::shared_ptr<Brush> texture = std::make_shared<Brush>(); \
	texture->m_tiling = false;\
	texture->m_texturePath = u8"asset/EditorAsset/"#texture".png"; \
	m_brushes.insert({ u8""#texture"", texture });

	EditorStyleSet::EditorStyleSet()
	{
		//register editor style set

		//blue color series
		math::float4 white = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
		math::float4 blueLevel1 = math::float4(0.88f, 0.92f, 0.97f, 1.0f);
		math::float4 blueLevel2 = math::float4(0.77f, 0.83f, 0.94f, 1.0f);
		math::float4 blueLevel3 = math::float4(0.65f, 0.75f, 0.91f, 1.0f);
		math::float4 blueLevel4 = math::float4(0.55f, 0.67f, 0.88f, 1.0f);
		math::float4 blueLevel5 = math::float4(0.29f, 0.47f, 0.82f, 1.0f);
		math::float4 blueLevel6 = math::float4(0.18f, 0.31f, 0.56f, 1.0f);
		math::float4 blueLevel7 = math::float4(0.09f, 0.16f, 0.28f, 1.0f);
		math::float4 black = math::float4(0.0f, 0.0f, 0.0f, 0.0f);
		ADD_COLOR(blueLevel1);
		ADD_COLOR(blueLevel2);
		ADD_COLOR(blueLevel3);
		ADD_COLOR(blueLevel4);
		ADD_COLOR(blueLevel5);
		ADD_COLOR(blueLevel6);
		ADD_COLOR(blueLevel7);
		ADD_COLOR(white);
		ADD_COLOR(black);

		//beige color series
		math::float4 beige1 = math::float4(0.91f, 0.89f, 0.87f, 1.0f);
		math::float4 beige2 = math::float4(0.87f, 0.84f, 0.81f, 1.0f);
		math::float4 beige3 = math::float4(0.83f, 0.79f, 0.75f, 1.0f);
		math::float4 beige4 = math::float4(0.79f, 0.74f, 0.68f, 1.0f);
		math::float4 beige5 = math::float4(0.75f, 0.69f, 0.62f, 1.0f);
		math::float4 beige6 = math::float4(0.73f, 0.63f, 0.55f, 1.0f);
		math::float4 beige7 = math::float4(0.65f, 0.56f, 0.45f, 1.0f);
		math::float4 beige8 = math::float4(0.38f, 0.32f, 0.25f, 1.0f);
		math::float4 beige9 = math::float4(0.19f, 0.16f, 0.13f, 1.0f);
		ADD_COLOR(beige1);
		ADD_COLOR(beige2);
		ADD_COLOR(beige3);
		ADD_COLOR(beige4);
		ADD_COLOR(beige5);
		ADD_COLOR(beige6);
		ADD_COLOR(beige7);
		ADD_COLOR(beige8);
		ADD_COLOR(beige9);

		//gray color
		math::float4 grayColor = math::float4(0.33f, 0.33f, 0.35f, 1.0f);
		ADD_COLOR(grayColor);

		//gray highlight color
		math::float4 grayHightlightColor = math::float4(0.24f, 0.24f, 0.24f, 1.0f);
		ADD_COLOR(grayHightlightColor);

		//light blue color series
		math::float4 lightBlueLevel1 = math::float4(0.520f, 0.80f, 0.89f, 1.0f);//(132, 204, 226)
		math::float4 lightBlueLevel2 = math::float4(0.42f, 0.65f, 0.83f, 1.0f);//(107, 165, 211)
		ADD_COLOR(lightBlueLevel1);
		ADD_COLOR(lightBlueLevel2);

		//no appearance
		std::shared_ptr<Brush> noResource = std::make_shared<Brush>();
		noResource->m_tiling = false;
		noResource->m_texturePath = "";
		noResource->m_drawAs = BrushDrawType::NoDrawType;
		noResource->m_actualSize = math::int2(0, 0);
		noResource->m_margin = Padding(0.0f, 0.0f, 0.0f, 0.0f);
		m_brushes.insert({ "NoBorder" , noResource });

		ADD_IMAGE(CloseButton_Normal);
		ADD_IMAGE(CloseButton_Hover);
		ADD_IMAGE(CloseButton_Pressed);

		std::shared_ptr<ButtonStyle> CloseButton = std::make_shared<ButtonStyle>();
		CloseButton->setNormal(CloseButton_Normal);
		CloseButton->setHovered(CloseButton_Hover);
		CloseButton->setPressed(CloseButton_Pressed);
		CloseButton->setDisabled(noResource);
		m_styles.insert({ u8"CloseButton", CloseButton });

		ADD_IMAGE(MinimizeButton_Normal);
		ADD_IMAGE(MinimizeButton_Hover);
		ADD_IMAGE(MinimizeButton_Pressed);

		std::shared_ptr<ButtonStyle> MinimizeButton = std::make_shared<ButtonStyle>();
		MinimizeButton->setNormal(MinimizeButton_Normal);
		MinimizeButton->setHovered(MinimizeButton_Hover);
		MinimizeButton->setPressed(MinimizeButton_Pressed);
		MinimizeButton->setDisabled(noResource);
		m_styles.insert({ u8"MinimizeButton", MinimizeButton });

		//normal blue button
		{
			std::shared_ptr<Brush> normalBlueColor = std::make_shared<Brush>();
			normalBlueColor->m_tiling = true; //rounded box 需要 (0.0f, 1.0f) 的 uv
			normalBlueColor->m_texturePath = u8"asset/white.png";
			normalBlueColor->m_tintColor = lightBlueLevel1;
			normalBlueColor->m_drawAs = BrushDrawType::Type::RoundedBox;
			//corner radius
			//outline color
			//outline width
			normalBlueColor->m_outlineSettings = BrushOutlineSettings(math::float4(4.4f, 4.4f, 4.4f, 4.4f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
			m_brushes.insert({ u8"normalBlueColor", normalBlueColor });
			std::shared_ptr<Brush> normalHoveredColor = std::make_shared<Brush>();
			normalHoveredColor->m_tiling = true;
			normalHoveredColor->m_texturePath = u8"asset/white.png";
			normalHoveredColor->m_tintColor = blueLevel3;
			normalHoveredColor->m_drawAs = BrushDrawType::Type::RoundedBox;
			normalHoveredColor->m_outlineSettings = BrushOutlineSettings(math::float4(4.4f, 4.4f, 4.4f, 4.4f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
			m_brushes.insert({ u8"normalHoveredColor", normalHoveredColor });
			std::shared_ptr<Brush> normalPressedColor = std::make_shared<Brush>();
			normalPressedColor->m_tiling = true;
			normalPressedColor->m_texturePath = u8"asset/white.png";
			normalPressedColor->m_tintColor = blueLevel6;
			normalPressedColor->m_drawAs = BrushDrawType::Type::RoundedBox;
			normalPressedColor->m_outlineSettings = BrushOutlineSettings(math::float4(4.4f, 4.4f, 4.4f, 4.4f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
			m_brushes.insert({ u8"normalPressedColor", normalPressedColor });
			std::shared_ptr<ButtonStyle> normalBlueButton = std::make_shared<ButtonStyle>();
			normalBlueButton->setNormal(normalBlueColor);
			normalBlueButton->setHovered(normalHoveredColor);
			normalBlueButton->setPressed(normalPressedColor);
			normalBlueButton->setDisabled(noResource);
			m_styles.insert({ u8"normalBlueButton", normalBlueButton });
		}

		//path view
		//close folder
		ADD_IMAGE(CloseFolder);
		ADD_IMAGE(OpenFolder);

		//table row style
		{
			std::shared_ptr<TableRowStyle> tableRowStyle = std::make_shared<TableRowStyle>();
			std::shared_ptr<Brush> activeBrush = std::make_shared<Brush>();
			activeBrush->m_tiling = false;
			activeBrush->m_texturePath = u8"asset/white.png";
			activeBrush->m_tintColor = beige8;
			activeBrush->m_drawAs = BrushDrawType::Type::Image;
			
			tableRowStyle->setActiveBrush(activeBrush); //选中，并且有键盘焦点
			tableRowStyle->setActiveHoveredBrush(activeBrush); //选中，并且有键盘焦点，并且悬浮
			tableRowStyle->setInactiveBrush(activeBrush); //选中，没有键盘焦点
			tableRowStyle->setInactiveHoveredBrush(activeBrush);  //选中，没有键盘焦点，并且悬浮

			std::shared_ptr<Brush> evenRowBackgroundBrush = std::make_shared<Brush>();
			evenRowBackgroundBrush->m_tiling = false;
			evenRowBackgroundBrush->m_texturePath = u8"asset/white.png";
			evenRowBackgroundBrush->m_tintColor = beige4;
			evenRowBackgroundBrush->m_drawAs = BrushDrawType::Type::Image;

			std::shared_ptr<Brush> oddRowBackgroundBrush = std::make_shared<Brush>();
			oddRowBackgroundBrush->m_tiling = false;
			oddRowBackgroundBrush->m_texturePath = u8"asset/white.png";
			oddRowBackgroundBrush->m_tintColor = beige7;
			oddRowBackgroundBrush->m_drawAs = BrushDrawType::Type::Image;

			tableRowStyle->setEvenRowBackgroundBrush(evenRowBackgroundBrush);
			tableRowStyle->setEvenRowBackgroundHoveredBrush(evenRowBackgroundBrush);

			tableRowStyle->setOddRowBackgroundBrush(oddRowBackgroundBrush);
			tableRowStyle->setOddRowBackgroundHoveredBrush(oddRowBackgroundBrush);

			m_styles.insert({ u8"tablerow.beige", tableRowStyle });
		}
		{
			//asset view table row
			std::shared_ptr<TableRowStyle> tableRowStyle = std::make_shared<TableRowStyle>();
			std::shared_ptr<Brush> activeBrush = std::make_shared<Brush>();
			activeBrush->m_tiling = false;
			activeBrush->m_texturePath = u8"asset/white.png";
			activeBrush->m_tintColor = beige8;
			activeBrush->m_drawAs = BrushDrawType::Type::Image;

			std::shared_ptr<Brush> hoverBrush = std::make_shared<Brush>();
			hoverBrush->m_tiling = false;
			hoverBrush->m_texturePath = u8"asset/white.png";
			hoverBrush->m_tintColor = beige6;
			hoverBrush->m_drawAs = BrushDrawType::Type::Image;

			tableRowStyle->setActiveBrush(activeBrush); //选中，并且有键盘焦点
			tableRowStyle->setActiveHoveredBrush(activeBrush); //选中，并且有键盘焦点，并且悬浮
			tableRowStyle->setInactiveBrush(activeBrush); //选中，没有键盘焦点
			tableRowStyle->setInactiveHoveredBrush(activeBrush);  //选中，没有键盘焦点，并且悬浮

			std::shared_ptr<Brush> evenRowBackgroundBrush = std::make_shared<Brush>();
			evenRowBackgroundBrush->m_tiling = false;
			evenRowBackgroundBrush->m_texturePath = u8"asset/white.png";
			evenRowBackgroundBrush->m_tintColor = beige4;
			evenRowBackgroundBrush->m_drawAs = BrushDrawType::Type::Image;

			std::shared_ptr<Brush> oddRowBackgroundBrush = std::make_shared<Brush>();
			oddRowBackgroundBrush->m_tiling = false;
			oddRowBackgroundBrush->m_texturePath = u8"asset/white.png";
			oddRowBackgroundBrush->m_tintColor = beige4;
			oddRowBackgroundBrush->m_drawAs = BrushDrawType::Type::Image;

			tableRowStyle->setEvenRowBackgroundBrush(evenRowBackgroundBrush);
			tableRowStyle->setEvenRowBackgroundHoveredBrush(hoverBrush);

			tableRowStyle->setOddRowBackgroundBrush(oddRowBackgroundBrush);
			tableRowStyle->setOddRowBackgroundHoveredBrush(hoverBrush);

			m_styles.insert({ u8"tablerow.assetview", tableRowStyle });
		}
	}
	EditorStyleSet::~EditorStyleSet()
	{
	}
	std::shared_ptr<StyleSet> EditorStyleSet::getStyleSet()
	{
		std::shared_ptr<StyleSet> editorStyleSet = StyleSetCenter::GetStyle("EditorStyleSet");
		return editorStyleSet;
	}
}