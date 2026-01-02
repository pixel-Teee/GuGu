#include <pch.h>

#include "EditorStyleSet.h"

#include <Core/UI/StyleSetCenter.h>
#include <Core/UI/Brush.h>
#include <Application/Application.h>
#include <Core/FileSystem/FileSystem.h>

namespace GuGu {
#define ADD_COLOR(color) m_colors.insert({#color, color})
#define ADD_IMAGE(texture) \
	std::shared_ptr<Brush> texture = std::make_shared<Brush>(); \
	texture->m_tiling = false;\
	texture->m_texturePath = u8"asset/EditorAsset/"#texture".png"; \
	m_brushes.insert({ u8""#texture"", texture });

#define ADD_IMAGE_WITH_COLOR(texture, colorStr) \
	std::shared_ptr<Brush> texture = std::make_shared<Brush>(); \
	texture->m_tiling = false; \
	texture->m_texturePath = u8"asset/EditorAsset/"#texture".png"; \
	texture->m_tintColorStr = colorStr;\
	m_brushes.insert({ u8""#texture"", texture });
	EditorStyleSet::EditorStyleSet()
	{
		//register editor style set
		Theme defaultTheme;

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
		defaultTheme.m_colors[ColorLevel1] = blueLevel1;
		defaultTheme.m_colors[ColorLevel2] = blueLevel2;
		defaultTheme.m_colors[ColorLevel3] = blueLevel3;
		defaultTheme.m_colors[ColorLevel4] = blueLevel4; 
		defaultTheme.m_colors[ColorLevel5] = blueLevel5;
		defaultTheme.m_colors[ColorLevel6] = blueLevel6;
		defaultTheme.m_colors[ColorLevel7] = blueLevel7;
		//ADD_COLOR(blueLevel1);
		//ADD_COLOR(blueLevel2);
		//ADD_COLOR(blueLevel3);
		//ADD_COLOR(blueLevel4);
		//ADD_COLOR(blueLevel5);
		//ADD_COLOR(blueLevel6);
		//ADD_COLOR(blueLevel7);
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
		defaultTheme.m_colors[SecondaryColorLevel1] = beige1;
		defaultTheme.m_colors[SecondaryColorLevel2] = beige2;
		defaultTheme.m_colors[SecondaryColorLevel3] = beige3;
		defaultTheme.m_colors[SecondaryColorLevel4] = beige4;
		defaultTheme.m_colors[SecondaryColorLevel5] = beige5;
		defaultTheme.m_colors[SecondaryColorLevel6] = beige6;
		defaultTheme.m_colors[SecondaryColorLevel7] = beige7;
		defaultTheme.m_colors[SecondaryColorLevel8] = beige8;
		defaultTheme.m_colors[SecondaryColorLevel9] = beige9;
		//ADD_COLOR(beige1);
		//ADD_COLOR(beige2);
		//ADD_COLOR(beige3);
		//ADD_COLOR(beige4);
		//ADD_COLOR(beige5);
		//ADD_COLOR(beige6);
		//ADD_COLOR(beige7);
		//ADD_COLOR(beige8);
		//ADD_COLOR(beige9);

		defaultTheme.m_colors[WhiteColor] = white;

		//gray color
		math::float4 grayColor = math::float4(0.33f, 0.33f, 0.35f, 1.0f);
		//ADD_COLOR(grayColor);

		//gray highlight color
		math::float4 grayHightlightColor = math::float4(0.24f, 0.24f, 0.24f, 1.0f);
		//ADD_COLOR(grayHightlightColor);

		defaultTheme.m_colors[GrayColor] = grayColor;
		defaultTheme.m_colors[GrayHighLightColor] = grayHightlightColor;

		//light blue color series
		math::float4 lightBlueColorLevel1 = math::float4(0.520f, 0.80f, 0.89f, 1.0f);//(132, 204, 226)
		math::float4 lightBlueColorLevel2 = math::float4(0.42f, 0.65f, 0.83f, 1.0f);//(107, 165, 211)
		//ADD_COLOR(LightColorLevel1);
		//ADD_COLOR(LightColorLevel2);

		defaultTheme.m_colors[LightColorLevel1] = lightBlueColorLevel1;
		defaultTheme.m_colors[LightColorLevel2] = lightBlueColorLevel2;

		//m_currentTheme = "defaultTheme";
		//m_themes.insert({ "defaultTheme", defaultTheme });

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

		ADD_IMAGE(MaximizeButton_Normal);
		ADD_IMAGE(MaximizeButton_Hover);
		ADD_IMAGE(MaximizeButton_Pressed);

		std::shared_ptr<ButtonStyle> MaximizeButton = std::make_shared<ButtonStyle>();
		MaximizeButton->setNormal(MaximizeButton_Normal);
		MaximizeButton->setHovered(MaximizeButton_Hover);
		MaximizeButton->setPressed(MaximizeButton_Pressed);
		MaximizeButton->setDisabled(noResource);
		m_styles.insert({ u8"MaximizeButton", MaximizeButton });

		ADD_IMAGE(RestoreButton_Normal);
		ADD_IMAGE(RestoreButton_Hover);
		ADD_IMAGE(RestoreButton_Pressed);

		std::shared_ptr<ButtonStyle> RestoreButton = std::make_shared<ButtonStyle>();
		RestoreButton->setNormal(RestoreButton_Normal);
		RestoreButton->setHovered(RestoreButton_Hover);
		RestoreButton->setPressed(RestoreButton_Pressed);
		RestoreButton->setDisabled(noResource);
		m_styles.insert({ u8"RestoreButton", RestoreButton });

		//normal blue button
		{
			std::shared_ptr<Brush> normalBlueColor = std::make_shared<Brush>();
			normalBlueColor->m_tiling = true; //rounded box 需要 (0.0f, 1.0f) 的 uv
			normalBlueColor->m_texturePath = u8"asset/white.png";
			normalBlueColor->m_tintColorStr = Theme::ThemeKeysToStr(ColorLevel1);
			normalBlueColor->m_drawAs = BrushDrawType::Type::RoundedBox;
			//corner radius
			//outline color
			//outline width
			normalBlueColor->m_outlineSettings = BrushOutlineSettings(math::float4(4.4f, 4.4f, 4.4f, 4.4f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
			m_brushes.insert({ u8"normalBlueColor", normalBlueColor });
			std::shared_ptr<Brush> normalHoveredColor = std::make_shared<Brush>();
			normalHoveredColor->m_tiling = true;
			normalHoveredColor->m_texturePath = u8"asset/white.png";
			normalHoveredColor->m_tintColorStr = Theme::ThemeKeysToStr(ColorLevel3);
			normalHoveredColor->m_drawAs = BrushDrawType::Type::RoundedBox;
			normalHoveredColor->m_outlineSettings = BrushOutlineSettings(math::float4(4.4f, 4.4f, 4.4f, 4.4f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);
			m_brushes.insert({ u8"normalHoveredColor", normalHoveredColor });
			std::shared_ptr<Brush> normalPressedColor = std::make_shared<Brush>();
			normalPressedColor->m_tiling = true;
			normalPressedColor->m_texturePath = u8"asset/white.png";
			normalPressedColor->m_tintColorStr = Theme::ThemeKeysToStr(ColorLevel6);
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

		ADD_IMAGE(CloseFolder_128x128)

		//table row style
		{
			std::shared_ptr<TableRowStyle> tableRowStyle = std::make_shared<TableRowStyle>();
			std::shared_ptr<Brush> activeBrush = std::make_shared<Brush>();
			activeBrush->m_tiling = false;
			activeBrush->m_texturePath = u8"asset/white.png";
			activeBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel8);
			activeBrush->m_drawAs = BrushDrawType::Type::Image;
			
			tableRowStyle->setActiveBrush(activeBrush); //选中，并且有键盘焦点
			tableRowStyle->setActiveHoveredBrush(activeBrush); //选中，并且有键盘焦点，并且悬浮
			tableRowStyle->setInactiveBrush(activeBrush); //选中，没有键盘焦点
			tableRowStyle->setInactiveHoveredBrush(activeBrush);  //选中，没有键盘焦点，并且悬浮

			std::shared_ptr<Brush> evenRowBackgroundBrush = std::make_shared<Brush>();
			evenRowBackgroundBrush->m_tiling = false;
			evenRowBackgroundBrush->m_texturePath = u8"asset/white.png";
			evenRowBackgroundBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel4);
			evenRowBackgroundBrush->m_drawAs = BrushDrawType::Type::Image;

			std::shared_ptr<Brush> oddRowBackgroundBrush = std::make_shared<Brush>();
			oddRowBackgroundBrush->m_tiling = false;
			oddRowBackgroundBrush->m_texturePath = u8"asset/white.png";
			oddRowBackgroundBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel7);
			oddRowBackgroundBrush->m_drawAs = BrushDrawType::Type::Image;

			tableRowStyle->setEvenRowBackgroundBrush(evenRowBackgroundBrush);
			tableRowStyle->setEvenRowBackgroundHoveredBrush(evenRowBackgroundBrush);

			tableRowStyle->setOddRowBackgroundBrush(oddRowBackgroundBrush);
			tableRowStyle->setOddRowBackgroundHoveredBrush(oddRowBackgroundBrush);

			m_styles.insert({ u8"tablerow.editor", tableRowStyle });
		}
		{
			//asset view table row
			std::shared_ptr<TableRowStyle> tableRowStyle = std::make_shared<TableRowStyle>();
			std::shared_ptr<Brush> activeBrush = std::make_shared<Brush>();
			activeBrush->m_tiling = false;
			activeBrush->m_texturePath = u8"asset/white.png";
			activeBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel8);
			activeBrush->m_drawAs = BrushDrawType::Type::Image;

			std::shared_ptr<Brush> hoverBrush = std::make_shared<Brush>();
			hoverBrush->m_tiling = false;
			hoverBrush->m_texturePath = u8"asset/white.png";
			hoverBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel6);
			hoverBrush->m_drawAs = BrushDrawType::Type::Image;

			tableRowStyle->setActiveBrush(activeBrush); //选中，并且有键盘焦点
			tableRowStyle->setActiveHoveredBrush(activeBrush); //选中，并且有键盘焦点，并且悬浮
			tableRowStyle->setInactiveBrush(activeBrush); //选中，没有键盘焦点
			tableRowStyle->setInactiveHoveredBrush(activeBrush);  //选中，没有键盘焦点，并且悬浮

			std::shared_ptr<Brush> evenRowBackgroundBrush = std::make_shared<Brush>();
			evenRowBackgroundBrush->m_tiling = false;
			evenRowBackgroundBrush->m_texturePath = u8"asset/white.png";
			evenRowBackgroundBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel4);
			evenRowBackgroundBrush->m_drawAs = BrushDrawType::Type::Image;

			std::shared_ptr<Brush> oddRowBackgroundBrush = std::make_shared<Brush>();
			oddRowBackgroundBrush->m_tiling = false;
			oddRowBackgroundBrush->m_texturePath = u8"asset/white.png";
			oddRowBackgroundBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel4);
			oddRowBackgroundBrush->m_drawAs = BrushDrawType::Type::Image;

			tableRowStyle->setEvenRowBackgroundBrush(evenRowBackgroundBrush);
			tableRowStyle->setEvenRowBackgroundHoveredBrush(hoverBrush);

			tableRowStyle->setOddRowBackgroundBrush(oddRowBackgroundBrush);
			tableRowStyle->setOddRowBackgroundHoveredBrush(hoverBrush);

			m_styles.insert({ u8"tablerow.assetview", tableRowStyle });
		}

		ADD_IMAGE(ImportModel_Icon)
		ADD_IMAGE(ImportTexture_Icon)
		ADD_IMAGE(ImportFont_Icon)
		ADD_IMAGE(ImportAnimation_Icon)
		ADD_IMAGE(MeshAssetIcon)
		ADD_IMAGE(LevelIcon)
		ADD_IMAGE(TextureAssetIcon)
		ADD_IMAGE(FontAssetIcon)
		ADD_IMAGE(AnimationAssetIcon)
		ADD_IMAGE(PrefabAssetIcon)

		ADD_IMAGE(Start_Normal)
		ADD_IMAGE(Start_Hover)
		ADD_IMAGE(Pause_Normal)
		ADD_IMAGE(Pause_Hover)
		{
			//check box style
			std::shared_ptr<CheckBoxStyle> checkBoxStyle = std::make_shared<CheckBoxStyle>();
			checkBoxStyle->setCheckedImage(Pause_Normal);
			checkBoxStyle->setUncheckedImage(Start_Normal);
			checkBoxStyle->setCheckedHoveredImage(Pause_Hover);
			checkBoxStyle->setUncheckedHoveredImage(Start_Hover);
			checkBoxStyle->setBackgroundImage(noResource);

			m_styles.insert({ u8"StartAndStop", checkBoxStyle });
		}

		ADD_IMAGE_WITH_COLOR(CheckBox_Checked, Theme::ThemeKeysToStr(ColorLevel4))
		ADD_IMAGE_WITH_COLOR(CheckBox_Checked_Hovered, Theme::ThemeKeysToStr(ColorLevel4))
		ADD_IMAGE_WITH_COLOR(CheckBox_Unchecked, Theme::ThemeKeysToStr(ColorLevel4))
		ADD_IMAGE_WITH_COLOR(CheckBox_Unchecked_Hovered, Theme::ThemeKeysToStr(ColorLevel4))
		{
			//normal box style
			std::shared_ptr<CheckBoxStyle> checkBoxStyle = std::make_shared<CheckBoxStyle>();
			checkBoxStyle->setCheckedImage(CheckBox_Checked);
			checkBoxStyle->setUncheckedImage(CheckBox_Unchecked);
			checkBoxStyle->setCheckedHoveredImage(CheckBox_Checked_Hovered);
			checkBoxStyle->setUncheckedHoveredImage(CheckBox_Unchecked_Hovered);
			checkBoxStyle->setBackgroundImage(noResource);

			m_styles.insert({ u8"EditorNormalCheckBox", checkBoxStyle });
		}

		{
			std::shared_ptr<TableColumnHeaderStyle> columnStyle = std::make_shared<TableColumnHeaderStyle>();
			std::shared_ptr<Brush> normalColumnBrush = std::make_shared<Brush>();
			normalColumnBrush->m_tiling = false;
			normalColumnBrush->m_texturePath = u8"asset/white.png";
			normalColumnBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel3);
			normalColumnBrush->m_drawAs = BrushDrawType::Type::Image;
			columnStyle->setNormalBrush(normalColumnBrush);
			columnStyle->setHoveredBrush(normalColumnBrush);
			columnStyle->setSortPrimaryAscendingImage(noResource);
			columnStyle->setSortPrimaryDescendingImage(noResource);
			columnStyle->setSortSecondaryAscendingImage(noResource);
			columnStyle->setSortSecondaryDescendingImage(noResource);
			columnStyle->setMenuDropDownImage(noResource);
			columnStyle->setMenuDropDownNormalBorderBrush(noResource);
			columnStyle->setMenuDropDownHoveredBorderBrush(noResource);

			std::shared_ptr<TableColumnHeaderStyle> lastColumnStyle = columnStyle;

			std::shared_ptr<SplitterStyle> splitterStyle = std::make_shared<SplitterStyle>();
			std::shared_ptr<Brush> normalBrush = std::make_shared<Brush>();
			normalBrush->m_tiling = false;
			normalBrush->m_texturePath = u8"asset/white.png";
			normalBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel8);
			normalBrush->m_drawAs = BrushDrawType::Type::Image;

			std::shared_ptr<Brush> highlightBrush = std::make_shared<Brush>();
			normalBrush->m_tiling = false;
			normalBrush->m_texturePath = u8"asset/white.png";
			normalBrush->m_tintColorStr = Theme::ThemeKeysToStr(SecondaryColorLevel5);
			normalBrush->m_drawAs = BrushDrawType::Type::Image;

			splitterStyle->setHandleNormalBrush(normalBrush);
			splitterStyle->setHandleHighlightBrush(highlightBrush);

			//header row style
			std::shared_ptr<HeaderRowStyle> headerRowStyle = std::make_shared<HeaderRowStyle>();
			headerRowStyle->setBackgroundBrush(noResource);
			headerRowStyle->setColumnStyle(columnStyle);
			headerRowStyle->setLastColumnStyle(lastColumnStyle);
			headerRowStyle->setColumnSplitterStyle(splitterStyle);	

			m_styles.insert({ u8"SceneOutliner.header", headerRowStyle });
		}

		//undo/redo
		ADD_IMAGE(Undo_Normal)
		ADD_IMAGE(Undo_Hover)
		ADD_IMAGE(Undo_Pressed)
		ADD_IMAGE(Redo_Normal)
		ADD_IMAGE(Redo_Hover)
		ADD_IMAGE(Redo_Pressed)
		std::shared_ptr<ButtonStyle> undoButton = std::make_shared<ButtonStyle>();
		undoButton->setNormal(Undo_Normal);
		undoButton->setHovered(Undo_Hover);
		undoButton->setPressed(Undo_Pressed);
		undoButton->setDisabled(noResource);
		m_styles.insert({ u8"undoButton", undoButton });
		std::shared_ptr<ButtonStyle> redoButton = std::make_shared<ButtonStyle>();
		redoButton->setNormal(Redo_Normal);
		redoButton->setHovered(Redo_Hover);
		redoButton->setPressed(Redo_Pressed);
		redoButton->setDisabled(noResource);
		m_styles.insert({ u8"redoButton", redoButton });

		//brush
		{
			//10x10 size
			std::shared_ptr<Brush> anchorGridBrush = std::make_shared<Brush>();
			anchorGridBrush->m_tiling = true;
			anchorGridBrush->m_texturePath = u8"asset/EditorAsset/anchorGrid.png";
			anchorGridBrush->m_tintColorStr = Theme::ThemeKeysToStr(GrayColor);
			//anchorGridBrush->m_drawAs = BrushDrawType::Type::Image;
			m_brushes.insert({ u8"anchorGrid" , anchorGridBrush });

			//32x32 size
			std::shared_ptr<Brush> anchorWidgetBrush = std::make_shared<Brush>();
			anchorWidgetBrush->m_tiling = false;
			anchorWidgetBrush->m_texturePath = u8"asset/EditorAsset/anchorWidget.png";
			anchorWidgetBrush->m_tintColorStr = Theme::ThemeKeysToStr(GrayColor);
			anchorWidgetBrush->m_drawAs = BrushDrawType::Type::Box;
			anchorWidgetBrush->m_margin = Padding(0.4f);
			m_brushes.insert({ u8"anchorWidget" , anchorWidgetBrush });
		}

		//get current theme and replace color
		//if (m_themes.find(m_currentTheme) != m_themes.end())
		//{
		//	Theme loadedTheme = m_themes.find(m_currentTheme)->second;
		//
		//	for (int32_t i = 0; i < loadedTheme.m_colors.size(); ++i)
		//	{
		//		m_colors.insert({ Theme::ThemeKeysToStr(static_cast<ThemeKeys>(i)), loadedTheme.m_colors[i] });
		//	}
		//}

		GuGuUtf8Str assetPath = Application::GetDirectoryWithExecutable();

		std::shared_ptr<NativeFileSystem> nativeFileSystem = std::make_shared<NativeFileSystem>(assetPath);
		m_rootFileSystem = std::make_shared<RootFileSystem>();
		m_rootFileSystem->mount("asset", nativeFileSystem);

		writeTheme("defaultTheme", defaultTheme);
		loadTheme("green2Theme");
	}
	EditorStyleSet::~EditorStyleSet()
	{
	}
	std::shared_ptr<StyleSet> EditorStyleSet::getStyleSet()
	{
		static std::shared_ptr<StyleSet> editorStyleSet = StyleSetCenter::GetStyle("EditorStyleSet");
		return editorStyleSet;
	}

	void EditorStyleSet::loadTheme(const GuGuUtf8Str& themeName)
	{
		m_currentTheme = themeName;
		//get current theme and replace color
		if (m_themes.find(m_currentTheme) != m_themes.end())
		{
			Theme loadedTheme = m_themes.find(m_currentTheme)->second;

			for (int32_t i = 0; i < loadedTheme.m_colors.size(); ++i)
			{
				m_colors.insert({ Theme::ThemeKeysToStr(static_cast<ThemeKeys>(i)), loadedTheme.m_colors[i] });
			}
		}
		else
		{
			//from disk to load theme
			std::vector<uint8_t> fileContent;
			m_rootFileSystem->OpenFile("asset/Themes/" + themeName + ".json", GuGuFile::FileMode::OnlyRead);
			int32_t fileLength = m_rootFileSystem->getFileSize();
			fileContent.resize(fileLength);
			int32_t haveReadedLength = 0;
			m_rootFileSystem->ReadFile(fileContent.data(), fileLength, haveReadedLength);
			m_rootFileSystem->CloseFile();

			Theme newTheme;
			nlohmann::json themeJson = nlohmann::json::parse(fileContent);
			for (auto& element : themeJson.items())
			{
				GuGuUtf8Str keyStr = element.key();
				ThemeKeys key = Theme::StrToThemeKeys(keyStr);
				math::float4 color;
				color.x = static_cast<float>(element.value()["r"]) / 256;
				color.y = static_cast<float>(element.value()["g"]) / 256;
				color.z = static_cast<float>(element.value()["b"]) / 256;
				color.w = static_cast<float>(element.value()["a"]) / 256;
				newTheme.m_colors[key] = color;
			}

			m_themes.insert({ themeName, newTheme });

			for (int32_t i = 0; i < newTheme.m_colors.size(); ++i)
			{
				m_colors.insert({ Theme::ThemeKeysToStr(static_cast<ThemeKeys>(i)), newTheme.m_colors[i] });
			}
		}
	}

	void EditorStyleSet::writeTheme(const GuGuUtf8Str& themeName, Theme theme)
	{
		nlohmann::json themeJson = nlohmann::json();
		for (int32_t i = 0; i < theme.m_colors.size(); ++i)
		{
			GuGuUtf8Str keyStr = Theme::ThemeKeysToStr(static_cast<ThemeKeys>(i));
			nlohmann::json writedColorJson;
			writedColorJson["r"] = (int32_t)(theme.m_colors[i].x * 256);
			writedColorJson["g"] = (int32_t)(theme.m_colors[i].y * 256);
			writedColorJson["b"] = (int32_t)(theme.m_colors[i].z * 256);
			writedColorJson["a"] = (int32_t)(theme.m_colors[i].w * 256);
			themeJson[keyStr.getStr()] = writedColorJson;
		}
		GuGuUtf8Str jsonContent = themeJson.dump();
		m_rootFileSystem->OpenFile("asset/Themes/" + themeName + ".json", GuGuFile::FileMode::OnlyWrite);
		m_rootFileSystem->WriteFile((void*)jsonContent.getStr(), jsonContent.getTotalByteCount());
		m_rootFileSystem->CloseFile();
	}

	std::map<GuGuUtf8Str, Theme>& EditorStyleSet::getThemeMap()
	{
		return m_themes;
	}

	GuGuUtf8Str EditorStyleSet::getCurrentThemeName()
	{
		return m_currentTheme;
	}

	Theme& EditorStyleSet::getTheme(const GuGuUtf8Str& themeName)
	{
		if (m_themes.find(m_currentTheme) != m_themes.end())
		{
			return m_themes.find(m_currentTheme)->second;
		}
		GuGu_LOGE("get error theme");
		return Theme();
	}

}