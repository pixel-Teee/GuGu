#pragma once

#include "VariantBase.h"

namespace GuGu {
	//class ArrayWrapper;

	namespace meta {
		template<typename T, typename StorageType>
		class MapVariantContainer : public VariantBase
		{
		public:
			MapVariantContainer(StorageType& rhs);

			Type GetType(void) const override;
			void* GetPtr(void) const override;

			int ToInt(void) const override;
			bool ToBool(void) const override;
			float ToFloat(void) const override;
			double ToDouble(void) const override;
			GuGuUtf8Str ToString(void) const override;

			bool IsArray(void) const override;
			ArrayWrapper GetArray(void) const override;

			VariantBase* Clone(void) const override;

		private:
			StorageType m_map;//map
		};
	}
}

#include "MapVariantContainer.hpp"