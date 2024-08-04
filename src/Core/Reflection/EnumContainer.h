#pragma once

#include "EnumBase.h"

namespace GuGu {
	namespace meta {
		template<typename EnumType>
		class EnumContainer : public EnumBase
		{
		public:
			typedef std::initializer_list<std::pair<GuGuUtf8Str, EnumType>> Initializer;
			typedef std::unordered_map<GuGuUtf8Str, EnumType> Table;

			EnumContainer(const GuGuUtf8Str& name, const Initializer& initializer);

			EnumContainer(
				const GuGuUtf8Str& name,
				const Initializer& initializer,
				TypeID owner
			);

			Type GetType(void) const override;
			Type GetUnderlyingType(void) const override;

			const std::vector<GuGuUtf8Str>& GetKeys(void) const override;
			std::vector<Variant> GetValues(void) const override;

			GuGuUtf8Str GetKey(const Argument& value) const override;
			Variant GetValue(const GuGuUtf8Str& key) const override;

		private:
			Table m_keyToValue;
			std::vector<GuGuUtf8Str> m_keys;
		};
	}
}

#include "EnumContainer.hpp"