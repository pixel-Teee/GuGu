#include <pch.h>

#include "UIPointerData.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {

	static bool registerGuGuUIPointerData()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::UIPointerData");
		auto& type = db.types[id];
		meta::TypeInfo<GuGu::UIPointerData>::Register(id, type, true, "4F50B8AF-D324-47D3-91DC-0213E1F5797A");

		auto typeID = typeidof(GuGu::UIPointerData);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GuGu::UIPointerData>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<GuGu::UIPointerData>();

			type.AddConstructor<GuGu::UIPointerData, false, false>({});

			type.AddConstructor<GuGu::UIPointerData, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

			meta::TypeInfo<GuGu::UIPointerData>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::UIPointerData>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<GuGu::UIPointerData>>::Register(id, type, false, "58939380-0A2B-42B0-8846-9FBF57A093A8");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::UIPointerData>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<GuGu::UIPointerData>>::Register(id, type, false, "13D473AB-E4E8-46B1-ADEA-1435A06C9FCC");
		}
		return true;
	}

	static bool registerGuGuUIPointerDataFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(GuGu::UIPointerData).GetID()];

		type.AddField<UIPointerData, math::float2>("m_screenPosition",
			(meta::FieldGetter<UIPointerData, math::float2&, true>::Signature) & UIPointerData::getScreenPosition,
			(meta::FieldSetter<UIPointerData, math::float2, true>::Signature) & UIPointerData::setScreenPosition, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(UIPointerData)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuUIPointerData)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(UIPointerData)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuUIPointerDataFields)
	IMPLEMENT_INITIAL_FIELDS_END

	UIPointerData::UIPointerData()
	{

	}
	UIPointerData::~UIPointerData()
	{
	}

	void UIPointerData::Update(float fElapsedTimeSeconds)
	{
	}

	meta::Object* UIPointerData::Clone(void) const
	{
		UIPointerData* pointerData = new UIPointerData();
		return pointerData;
	}

	math::float2 UIPointerData::getScreenPosition() const
	{
		return m_screenPosition;
	}

	math::float2& UIPointerData::getScreenPosition()
	{
		return m_screenPosition;
	}

	void UIPointerData::setScreenPosition(const math::float2& inScreenPosition)
	{
		m_screenPosition = inScreenPosition;
	}

	meta::Type UIPointerData::GetType() const
	{
		return meta::Type(typeof(UIPointerData));
	}
}