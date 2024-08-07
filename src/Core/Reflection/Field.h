#pragma once

#include "MetaContainer.h"
#include "Variant.h"

#include "FieldGetter.h"
#include "FieldSetter.h"

namespace GuGu {
	namespace meta {
		class Method;
		class Field : public MetaContainer
		{
		public:
			//调用setter，将参数value填入setter，获取得到instance
			static bool SetValue(Variant& instance, const Variant& value, const Method& setter);

			Field(void);

			Field(
				const GuGuUtf8Str& name,
				Type type,
				Type classType,
				FieldGetterBase* getter,
				FieldSetterBase* setter
			);

			static const Field& Invalid(void);

			bool IsValid(void) const;
			bool IsReadOnly(void) const;

			Type GetType(void) const;
			Type GetClassType(void) const;

			const GuGuUtf8Str& GetName(void) const;

			Variant GetValue(const Variant& instance) const;
			Variant GetValueReference(const Variant& instance) const;

			bool SetValue(Variant& instance, const Variant& value) const;
		private:
			friend struct TypeData;

			Type m_type;
			Type m_classType;

			GuGuUtf8Str m_name;

			std::shared_ptr<FieldGetterBase> m_getter;
			std::shared_ptr<FieldSetterBase> m_setter;
		};
	}
}