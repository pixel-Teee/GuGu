#pragma once

#include "EnumBase.h"

namespace GuGu {
	namespace meta {
		class Enum
		{
		public:
			bool IsValid(void) const;

			operator bool(void) const;

			bool operator ==(const Enum& rhs) const;
			bool operator !=(const Enum& rhs) const;

			GuGuUtf8Str GetName(void) const;

			Type GetType(void) const;
			Type GetParentType(void) const;
			Type GetUnderlyingType(void) const;

			std::vector<GuGuUtf8Str> GetKeys(void) const;
			std::vector<Variant> GetValues(void) const;

			GuGuUtf8Str GetKey(const Argument& value) const;
			Variant GetValue(const GuGuUtf8Str& key) const;
		private:
			friend struct TypeData;

			Enum(const EnumBase* base);

			std::shared_ptr<const EnumBase> m_base;
		};
	}
}