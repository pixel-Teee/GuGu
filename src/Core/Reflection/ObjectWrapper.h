#pragma once

#include "VariantBase.h"

namespace GuGu {
	namespace meta {
		class Object;

		class ObjectWrapper : public VariantBase
		{
		public:
			ObjectWrapper(Object* instance);

			Type GetType(void) const override;
			void* GetPtr(void) const override;

			int32_t ToInt(void) const override;
			bool ToBool(void) const override;
			float ToFloat(void) const override;
			double ToDouble(void) const override;
			GuGuUtf8Str ToString(void) const override;

			VariantBase* Clone(void) const override;

			void OnSerialize(nlohmann::json& output) const override;
			void OnDeserialize(const nlohmann::json& input) override;
		private:
			Object* m_object;
		};
	}
}