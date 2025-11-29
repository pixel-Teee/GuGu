#include <pch.h>

#include "CustomDeserializeField.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registermetaCustomDeserializeField()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("meta::CustomDeserializeField");
		auto& type = db.types[id];
		meta::TypeInfo<meta::CustomDeserializeField>::Register(id, type, true, "26FC0934-E661-4A49-A2AF-86B073111DD3");
		return true;
	}

	namespace meta {
		IMPLEMENT_INITIAL_BEGIN(CustomDeserializeField)
			ADD_INITIAL_FUNCTION_WITH_PRIORITY(registermetaCustomDeserializeField)
		IMPLEMENT_INITIAL_END

		CustomDeserializeField::CustomDeserializeField(const std::function<meta::Variant(const GuGuUtf8Str& inFieldName, const meta::Type& inFieldType, const nlohmann::json& inValue)>& inDeserializeFieldCallback)
		{
			m_deserializeFieldCallback = inDeserializeFieldCallback;
		}

		CustomDeserializeField::CustomDeserializeField(const CustomDeserializeField& rhs)
		{
			m_deserializeFieldCallback = rhs.m_deserializeFieldCallback;
		}

		void CustomDeserializeField::Update(float fElapsedTimeSeconds)
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

		meta::Type CustomDeserializeField::GetType() const
		{
			//throw std::logic_error("The method or operation is not implemented.");
			return typeof(CustomDeserializeField);
		}

		meta::Object* CustomDeserializeField::Clone(void) const
		{
			CustomDeserializeField* newCustomDeserializeField = new CustomDeserializeField(*this);
			newCustomDeserializeField->m_deserializeFieldCallback = m_deserializeFieldCallback;
			return newCustomDeserializeField;
		}

		void CustomDeserializeField::PostLoad()
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

		meta::Variant CustomDeserializeField::invokeCallback(const GuGuUtf8Str& inFieldName, const meta::Type& inFieldType, const nlohmann::json& inValue) const
		{
			//nlohmann::json result;
			//if (m_deserializeFieldCallback)
			//{
			//	result = m_deserializeFieldCallback(inValue);
			//}
			//return result;
			if (m_deserializeFieldCallback)
			{
				return m_deserializeFieldCallback(inFieldName, inFieldType, inValue);
			}
			return meta::Variant();//not valid
		}

	}
}