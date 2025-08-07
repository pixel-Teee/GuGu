#include <pch.h>

#include "Function.h"

namespace GuGu {
	namespace meta {

		Function::Function()
			: Invokable()
			, m_parentType(Type::Invalid())
			, m_invoker(nullptr)
		{

		}

		const Function& Function::Invalid()
		{
			static Function invalid;
			return invalid;
		}

		Type Function::GetParentType() const
		{
			return m_parentType;
		}

		bool Function::IsValid() const
		{
			return m_invoker != nullptr;
		}

		Variant Function::InvokeVariadic(ArgumentList& arguments) const
		{
			return m_invoker->Invoke(arguments);
		}

	}
}