#include <pch.h>

#include "GFont.h"
#include <Core/Reflection/TypeInfo.h>

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

		return fontFile;
	}

	void GFont::OnSerialize(nlohmann::json& output) const
	{

	}

	void GFont::OnDeserialize(const nlohmann::json& input)
	{

	}

}