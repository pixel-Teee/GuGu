#pragma once

#include <Core/Reflection/MetaProperty.h>
#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	namespace meta {
		class CustomDeserializeField : public MetaProperty
		{
		public:
			DECLARE_INITIAL
			CustomDeserializeField(const std::function<meta::Variant(const GuGuUtf8Str& inFieldName, const meta::Type& inFieldType, const nlohmann::json& inValue)>& inDeserializeFieldCallback);

			CustomDeserializeField(const CustomDeserializeField& rhs);

			virtual ~CustomDeserializeField() { }

			std::function<meta::Variant(const GuGuUtf8Str& inFieldName, const meta::Type& inFieldType, const nlohmann::json& inValue)> m_deserializeFieldCallback;

			void Update(float fElapsedTimeSeconds) override;

			meta::Type GetType() const override;

			Object* Clone(void) const override;

			void PostLoad() override;

			meta::Variant invokeCallback(const GuGuUtf8Str& inFieldName, const meta::Type& inFieldType, const nlohmann::json& inValue) const;
		};
	}
}