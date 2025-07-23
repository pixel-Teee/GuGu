#pragma once

#include <Core/Reflection/MetaProperty.h>
#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	namespace meta {
		class Range : public MetaProperty
		{
		public:
			DECLARE_INITIAL
			Range(float inMinValue, float inMaxValue);

			Range(const Range& rhs);

			virtual ~Range() { }

			void Update(float fElapsedTimeSeconds) override;

			meta::Type GetType() const override;

			Object* Clone(void) const override;

			void PostLoad() override;

			//range
			float m_minValue;

			float m_maxValue;
		};
	}
}