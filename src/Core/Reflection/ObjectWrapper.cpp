#include <pch.h>

#include "ObjectWrapper.h"

#include "Object.h"
#include "Type.h"

namespace GuGu {
	namespace meta {
		ObjectWrapper::ObjectWrapper(Object* instance)
			: m_object(instance)
		{}

		Type ObjectWrapper::GetType(void) const
		{
			return m_object->GetType();
		}

		void* ObjectWrapper::GetPtr(void) const
		{
			return m_object;
		}

		int32_t ObjectWrapper::ToInt(void) const
		{
			return int();
		}

		bool ObjectWrapper::ToBool(void) const
		{
			return bool();
		}

		float ObjectWrapper::ToFloat(void) const
		{
			return float();
		}

		double ObjectWrapper::ToDouble(void) const
		{
			return double();
		}

		GuGuUtf8Str ObjectWrapper::ToString(void) const
		{
			return GuGuUtf8Str();
		}

		VariantBase* ObjectWrapper::Clone(void) const
		{
			return new ObjectWrapper(m_object);
		}

		void ObjectWrapper::OnSerialize(nlohmann::json& output) const
		{
			m_object->OnSerialize(output);
		}

		void ObjectWrapper::OnDeserialize(const nlohmann::json& input)
		{
			m_object->OnDeserialize(output);
		}

	}
}