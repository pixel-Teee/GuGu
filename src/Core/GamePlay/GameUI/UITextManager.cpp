#include <pch.h>

#include "UITextManager.h"
#include <fstream>
#include <Core/GamePlay/GameUI/GFont.h>
#include <Core/AssetManager/AssetManager.h>

namespace GuGu {

	UITextManager::UITextManager()
	{
		m_freeTypeLibrary = new FT_Library();
		if (FT_Init_FreeType(m_freeTypeLibrary))
			GuGu_LOGE("could not init free type library");
	}

	UITextManager::~UITextManager()
	{
		FT_Done_FreeType(*m_freeTypeLibrary);
		delete m_freeTypeLibrary;
		m_freeTypeLibrary = nullptr;
	}

	std::shared_ptr<UITextManager> UITextManager::getUITextManager()
	{
		static std::shared_ptr<UITextManager> uiTextManager = std::make_shared<UITextManager>();
		return uiTextManager;
	}

	nlohmann::json UITextManager::loadFontFile(const GuGuUtf8Str& fontFilePhysicalFilePath)
	{
		std::ifstream in(fontFilePhysicalFilePath.getStr(), std::ios::binary); //todo:fix this, use file system

		std::vector<uint8_t> fileData;
		in.seekg(0, std::ios::end);
		fileData.reserve(in.tellg());
		in.seekg(0, std::ios::beg);
		char ch;
		while (in.get(ch))
		{
			fileData.push_back(ch);
		}

		std::shared_ptr<GFont> gFont = std::make_shared<GFont>();
		gFont->m_data = fileData;

		nlohmann::json fontJson = AssetManager::getAssetManager().serializeJson(gFont);
		return fontJson;
	}

	void UITextManager::initFreeTypeFace(FT_Face& newFace, std::vector<uint8_t>& fontData)
	{
		bool error = FT_New_Memory_Face(*m_freeTypeLibrary,
			fontData.data(),
			fontData.size(),
			0,
			&newFace);	
		
		if (error == FT_Err_Ok)
		{
			GuGu_LOGD("init free type face successful");
		}
		else
		{
			GuGu_LOGE("init free type face error, error code: %d", error);
		};

		FT_Select_Charmap(newFace, ft_encoding_unicode);
	}

	void UITextManager::setCurrentFontSize(FT_Face& newFace, float fontPoint)
	{
		FT_Set_Pixel_Sizes(newFace, 0, 16);
	}

	void UITextManager::loadCurrentCharacter(FT_Face& newFace, GuGuUtf8Str inCharacter)
	{
		std::vector<uint32_t> unicode = inCharacter.getUnicode();
		if(unicode.size() > 0)
			FT_Load_Char(newFace, unicode[0], FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL);
	}

	math::float2 UITextManager::queryGlyphWidthAndHeight(FT_Face& newFace)
	{
		return math::float2(newFace->glyph->bitmap.width, newFace->glyph->bitmap.rows);
	}

	math::float2 UITextManager::queryGlyphBearing(FT_Face& newFace)
	{
		return math::float2(newFace->glyph->bitmap_left, newFace->glyph->bitmap_top);
	}

	float UITextManager::queryGlyphAdvance(FT_Face& newFace)
	{
		return newFace->glyph->advance.x;
	}

	std::vector<uint8_t> UITextManager::getGlyphPixelData(FT_Face& newFace)
	{
		//generate gray
		FT_Render_Glyph(newFace->glyph, FT_RENDER_MODE_NORMAL);

		FT_Bitmap bitmap = newFace->glyph->bitmap;

		std::vector<uint8_t> pixelData;
		pixelData.resize(bitmap.rows * bitmap.width);

		for (int32_t y = 0; y < bitmap.rows; ++y)
		{
			for (int32_t x = 0; x < bitmap.width; ++x)
			{
				unsigned char pixel = bitmap.buffer[y * bitmap.pitch + x];
				pixelData.push_back(pixel);
			}
		}
		return pixelData;
	}

}