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
			CustomDeserializeField(const std::function<nlohmann::json(const meta::Variant& inValue)>& inDeserializeFieldCallback);

			CustomDeserializeField(const CustomDeserializeField& rhs);

			virtual ~CustomDeserializeField() { }

			std::function<nlohmann::json(const meta::Variant& inValue)> m_deserializeFieldCallback;

			void Update(float fElapsedTimeSeconds) override;

			meta::Type GetType() const override;

			Object* Clone(void) const override;

			void PostLoad() override;

			nlohmann::json invokeCallback(const meta::Variant& inValue);
		};
	}
}