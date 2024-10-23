#include <pch.h>

#include "Method.h"

namespace GuGu {
	namespace meta {
		Method::Method(void)
			: Invokable()
			, m_isConst(true)
			, m_classType(Type::Invalid())
			, m_invoker(nullptr)
		{}

		const Method& Method::Invalid(void)
		{
			static Method invalid;

			return invalid;
		}

		Type Method::GetClassType(void) const
		{
			return m_classType;
		}

		bool Method::IsValid(void) const
		{
			return m_invoker != nullptr;
		}

		bool Method::IsConst(void) const
		{
			return m_isConst;
		}

		Variant Method::Invoke(
			Variant& instance,
			ArgumentList& arguments
		) const
		{
			return m_invoker->Invoke(instance, arguments);
		}
	}
}