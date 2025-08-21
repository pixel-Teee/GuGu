#pragma once

#include <Core/Reflection/Object.h>
#include <Core/Reflection/Type.h>
#include <Renderer/nvrhi.h>

#include "UIAtlas.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace GuGu {
	struct GFontCharacter
	{
		GuGuUtf8Str m_char;

		//point size
		float m_pointSize;

		GFontCharacter()
		{
			m_pointSize = 16;
		}

		bool operator<(const GFontCharacter& rhs) const
		{
			if (m_char != rhs.m_char)
				return m_char < rhs.m_char;
			else if (m_char == rhs.m_char)
				return m_pointSize < rhs.m_pointSize;
		}
	};

	//character info
	struct GFontCharacterMetrics
	{
		math::float2 m_uvSize;
		math::float2 m_uvPosition;
		math::float2 m_size;//glyph size(pixel size)
		math::float2 m_bearing;//pixel size
		uint32_t m_advance;//origin to origin(pixel size)
	};

	
	class GTexture;
	//font file
	class GFont : public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		GFont();

		virtual ~GFont();

		virtual void PostLoad() override;

		virtual void Update(float fElapsedTimeSeconds) override;
		virtual meta::Type GetType() const override;
		//拷贝对象
		virtual Object* Clone(void) const override;
		//序列化
		virtual void OnSerialize(nlohmann::json& output) const override;
		//反序列化
		virtual void OnDeserialize(const nlohmann::json& input) override;

		GFontCharacterMetrics getFontMetrices(GuGuUtf8Str character, float fontPoint, float scale);

		float getFontMaxHeightMetrices(float fontPoint, float scale);

		//free type
		FT_Face m_ftFace;

		std::map<GFontCharacter, GFontCharacterMetrics> m_characterMetricsMap;

		//font file data
		Array<uint8_t> m_data;

		std::shared_ptr<UIAtlas> m_atlas;

		bool m_bIsInitialized;
	};
}