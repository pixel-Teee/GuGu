#pragma once

#include <Core/Reflection/MetaProperty.h>
#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	namespace meta {
		class DisplayName : public MetaProperty
		{
		public:
			DECLARE_INITIAL
			DisplayName(const char* inDisplayName);

			DisplayName(const DisplayName& rhs);

			virtual ~DisplayName() { }

			GuGuUtf8Str m_displayName;

			void Update(float fElapsedTimeSeconds) override;

			meta::Type GetType() const override;

			Object* Clone(void) const override;

			void PostLoad() override;
		};
	}
}