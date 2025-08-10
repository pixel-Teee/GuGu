#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Core/Math/MyMath.h>

//struct FT_Face;
//struct FT_Library;
namespace GuGu {

	class UITextManager
	{
	public:
		UITextManager();

		~UITextManager();

		static std::shared_ptr<UITextManager> getUITextManager();

		nlohmann::json loadFontFile(const GuGuUtf8Str& fontFilePhysicalFilePath);

		void initFreeTypeFace(FT_Face& newFace, std::vector<uint8_t>& fontData);

		void setCurrentFontSize(FT_Face& newFace, float fontPoint);

		void loadCurrentCharacter(FT_Face& newFace, GuGuUtf8Str inCharacter);

		math::float2 queryGlyphWidthAndHeight(FT_Face& newFace);

		math::float2 queryGlyphBearing(FT_Face& newFace);

		float queryGlyphAdvance(FT_Face& newFace);

		std::vector<uint8_t> getGlyphPixelData(FT_Face& newFace);
	private:

		FT_Library* m_freeTypeLibrary;
	};
}