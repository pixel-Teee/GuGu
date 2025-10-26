#pragma once

#include <Core/Reflection/MetaProperty.h>
#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	namespace meta {
		class CustomSerializeField : public MetaProperty
		{
		public:
			DECLARE_INITIAL
			CustomSerializeField(const std::function<nlohmann::json(const GuGuUtf8Str& inFieldName, const meta::Variant& inValue)>& inSerializeFieldCallback);

			CustomSerializeField(const CustomSerializeField& rhs);

			virtual ~CustomSerializeField() { }

			std::function<nlohmann::json(const GuGuUtf8Str& inFieldName, const meta::Variant& inValue)> m_serializeFieldCallback;

			void Update(float fElapsedTimeSeconds) override;

			meta::Type GetType() const override;

			Object* Clone(void) const override;

			void PostLoad() override;

			nlohmann::json invokeCallback(const GuGuUtf8Str& inFieldName, const meta::Variant& inValue) const;
		};
	}
}