#include <pch.h>

#include "GFont.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameUI/UITextManager.h>
#include <Core/Texture/GTexture.h>

namespace GuGu {
	static bool registerGuGuGFont()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::GFont");
		auto& type = db.types[id];
		meta::TypeInfo<GFont>::Register(id, type, true, "BCB10F47-146C-46E0-877C-46982C3E34C5");

		auto typeID = typeidof(GFont);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GFont>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<GFont>();

			type.AddConstructor<GFont, false, false>({});

			type.AddConstructor<GFont, true, true>({});

			type.LoadBaseClasses(db, id, { typeof(meta::Object) });

			meta::TypeInfo<GFont>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::GFont>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<GFont>>::Register(id, type, false, "7271CBF1-695B-4C0C-8C83-628D14ED5421");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::GFont>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<GFont>>::Register(id, type, false, "03B0BD65-0435-455C-8AC7-2B335263AD33");
		}
		return true;
	}
	static bool registerGuGuGFontFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(GFont).GetID()];

		type.AddField<GFont, Array<uint8_t>>("m_data",
			(meta::FieldGetter<GFont, Array<uint8_t>, false>::Signature) & GFont::m_data,
			(meta::FieldSetter<GFont, Array<uint8_t>, false>::Signature) & GFont::m_data, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GFont)
		ADD_PRIORITY(Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGFont)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GFont)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGFontFields)
	IMPLEMENT_INITIAL_FIELDS_END
	GFont::GFont()
	{
		
	}
	GFont::~GFont()
	{
	}

	void GFont::PostLoad()
	{
		if (m_atlas == nullptr)
		{
			//create atlas
			m_atlas = std::make_shared<UIAtlas>();
			m_atlas->initializeFontAtlas(1024, 1024, nvrhi::Format::R8_UNORM);
		}
	}

	void GFont::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type GFont::GetType() const
	{
		return typeof(GFont);
	}

	meta::Object* GFont::Clone(void) const
	{
		GFont* fontFile = new GFont();
		fontFile->m_data = m_data;
		return fontFile;
	}

	void GFont::OnSerialize(nlohmann::json& output) const
	{

	}

	void GFont::OnDeserialize(const nlohmann::json& input)
	{

	}

	GFontCharacterMetrics GFont::getFontMetrices(GuGuUtf8Str character, float fontPoint, float scale)
	{
		float neededSize = fontPoint / scale;

		std::shared_ptr<UITextManager> textManager = UITextManager::getUITextManager();

		GFontCharacter fontCharacter;
		fontCharacter.m_char = character;
		fontCharacter.m_pointSize = neededSize;
		auto it = m_characterMetricsMap.find(fontCharacter);
		if (it != m_characterMetricsMap.end())
			return it->second;

		GFontCharacterMetrics metrices;
		metrices.m_uvSize = math::float2(0, 0);
		metrices.m_uvPosition = math::float2(0, 0);
		metrices.m_size = math::float2(0, 0);
		metrices.m_bearing = math::float2(0, 0);
		metrices.m_advance = 0;

		//m_bNeedToUpdateAtlas = true;
		////insert to atlas
		//if (m_bHaveFontAtlas == false)
		//{
		//	m_bHaveFontAtlas = true;
		//	m_fontAtlasHeight = 1024;
		//	m_fontAtlasHeight = 1024;
		//	m_fontAtlasData.resize(1024 * 1024); //gray
		//}

		if (m_atlas == nullptr)
		{
			//create atlas
			m_atlas = std::make_shared<UIAtlas>();
			m_atlas->initializeFontAtlas(1024, 1024, nvrhi::Format::R8_UNORM);
		}

		textManager->initFreeTypeFace(m_ftFace, m_data);
		textManager->setCurrentFontSize(m_ftFace, neededSize);
		textManager->loadCurrentCharacter(m_ftFace, character);

		metrices.m_size = textManager->queryGlyphWidthAndHeight(m_ftFace);
		metrices.m_bearing = math::float2(textManager->queryGlyphBearing(m_ftFace));
		metrices.m_advance = textManager->queryGlyphAdvance(m_ftFace);

		std::vector<uint8_t> fontPixelData = textManager->getGlyphPixelData(m_ftFace);
		std::shared_ptr<UIAtlasTextureSlot> slot = m_atlas->copyTextureToAtlas(metrices.m_size.x, metrices.m_size.y, fontPixelData);

		//calculate uv info
		metrices.m_uvSize = math::float2(slot->width / 1024.0f, slot->height / 1024.0f);
		metrices.m_uvPosition = math::float2(slot->x / 1024.0f, slot->y / 1024.0f);

		m_characterMetricsMap.insert({ fontCharacter, metrices });

		return metrices;
	}

}