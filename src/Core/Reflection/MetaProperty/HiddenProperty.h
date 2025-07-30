#pragma once

#include <Core/Reflection/MetaProperty.h>
#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	namespace meta {
		class HiddenProperty : public MetaProperty
		{
		public:
			DECLARE_INITIAL
			HiddenProperty();

			HiddenProperty(const HiddenProperty& rhs);

			virtual ~HiddenProperty() { }

			void Update(float fElapsedTimeSeconds) override;

			meta::Type GetType() const override;

			Object* Clone(void) const override;

			void PostLoad() override;
		};
	}
}