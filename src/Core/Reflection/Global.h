#pragma once

#include "MetaContainer.h"
#include "Variant.h"

#include "GlobalGetter.h"
#include "GlobalSetter.h"

namespace GuGu {
	namespace meta {
		class Global : public MetaContainer
		{
		public:
			Global(void);

			Global(
				const GuGuUtf8Str& name,
				Type type,
				GlobalGetterBase* getter,
				GlobalSetterBase* setter,
				Type parentType = Type::Invalid()
			);

			static const Global& Invalid(void);

			bool IsValid(void) const;
			bool IsReadOnly(void) const;

			Type GetType(void) const;
			Type GetParentType(void) const;

			const GuGuUtf8Str& GetName(void) const;

			Variant GetValue(void) const;
			bool SetValue(const Argument& value) const;

		private:
			friend struct TypeData;

			Type m_type;
			Type m_parentType;

			GuGuUtf8Str m_name;

			std::shared_ptr<GlobalGetterBase> m_getter;
			std::shared_ptr<GlobalSetterBase> m_setter;
		};
	}
}