#include <pch.h>

#include "CustomSerializeField.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registermetaCustomSerializeField()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("meta::CustomSerializeField");
		auto& type = db.types[id];
		meta::TypeInfo<meta::CustomSerializeField>::Register(id, type, true, "ED405F76-6C63-49BB-97FC-A902B70FFA81");
		return true;
	}

	namespace meta {
		IMPLEMENT_INITIAL_BEGIN(CustomSerializeField)
			ADD_INITIAL_FUNCTION_WITH_PRIORITY(registermetaCustomSerializeField)
		IMPLEMENT_INITIAL_END

		CustomSerializeField::CustomSerializeField(const std::function<nlohmann::json(const GuGuUtf8Str& inFieldName, const meta::Variant& inValue)>& inSerializeFieldCallback)
		{
			m_serializeFieldCallback = inSerializeFieldCallback;
		}

		CustomSerializeField::CustomSerializeField(const CustomSerializeField& rhs)
		{
			m_serializeFieldCallback = rhs.m_serializeFieldCallback;
		}

		void CustomSerializeField::Update(float fElapsedTimeSeconds)
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

		meta::Type CustomSerializeField::GetType() const
		{
			//throw std::logic_error("The method or operation is not implemented.");
			return typeof(CustomSerializeField);
		}

		GuGu::meta::Object* CustomSerializeField::Clone(void) const
		{
			CustomSerializeField* newCustomSerializeField = new CustomSerializeField(*this);
			newCustomSerializeField->m_serializeFieldCallback = m_serializeFieldCallback;
			return newCustomSerializeField;
		}

		void CustomSerializeField::PostLoad()
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

		nlohmann::json CustomSerializeField::invokeCallback(const GuGuUtf8Str& inFieldName, const meta::Variant& inValue) const
		{
			nlohmann::json result;
			if (m_serializeFieldCallback)
			{
				result = m_serializeFieldCallback(inFieldName, inValue);
			}
			return result;
		}

	}
}