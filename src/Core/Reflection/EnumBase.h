#pragma once

#include "Type.h"

#include <string>
#include <vector>

namespace GuGu {
	namespace meta {
		class Variant;
		class Argument;

		class EnumBase
		{
		public:
			EnumBase(const GuGuUtf8Str& name, TypeID owner);

			virtual ~EnumBase(void) {}

			virtual Type GetType(void) const = 0;
			virtual Type GetUnderlyingType(void) const = 0;

			virtual const std::vector<GuGuUtf8Str>& GetKeys(void) const = 0;
			virtual std::vector<Variant> GetValues(void) const = 0;

			virtual GuGuUtf8Str GetKey(const Argument& value) const = 0;
			virtual Variant GetValue(const GuGuUtf8Str& key) const = 0;

			Type GetParentType(void) const;

			const GuGuUtf8Str& GetName(void) const;
		private:
			Type m_parentType;

			GuGuUtf8Str m_name;
		};
	}
}