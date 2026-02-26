#include <pch.h>

#include "TransformApplier.h"

namespace GuGu {

	VectorApplier::VectorApplier()
	{

	}

	VectorApplier::~VectorApplier()
	{

	}

	void VectorApplier::Apply(std::shared_ptr<meta::Object> inObject, const meta::Variant& value, const GuGuUtf8Str& fieldName)
	{
		meta::Type classType = inObject->GetType();
		//get field
		meta::Field curField = meta::ReflectionDatabase::Instance().types[classType.GetID()].GetField(fieldName.getStr());

		meta::Variant instance = ObjectVariant(inObject.get());

		if (curField.IsValid())
		{
			//set value
			curField.SetValue(instance, value);
		}
		else
		{
			GuGu_LOGE("cut scenes applier error");
		}
	}

}