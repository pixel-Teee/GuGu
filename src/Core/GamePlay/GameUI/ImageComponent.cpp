#include <pch.h>

#include "ImageComponent.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuImageComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::ImageComponent");
		auto& type = db.types[id];
		meta::TypeInfo<ImageComponent>::Register(id, type, true, "D97F59C0-35AB-4436-904E-B2BA72046DE1");

		auto typeID = typeidof(ImageComponent);

		return true;
	}

	static bool registerGuGuImageComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(ImageComponent).GetID()];

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(ImageComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuImageComponent)
		IMPLEMENT_INITIAL_END

		IMPLEMENT_INITIAL_FIELDS_BEGIN(ImageComponent)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuImageComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	ImageComponent::ImageComponent()
	{

	}

	ImageComponent::~ImageComponent()
	{

	}

	meta::Object* ImageComponent::Clone(void) const
	{
		return nullptr;
	}

	void ImageComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type ImageComponent::GetType() const
	{
		return typeof(ImageComponent);
	}

}