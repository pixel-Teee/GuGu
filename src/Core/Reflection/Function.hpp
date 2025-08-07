#pragma once

namespace GuGu {
	namespace meta {
		template<typename ReturnType, typename... ArgTypes>
		Function::Function(
			const GuGuUtf8Str& name,
			ReturnType(*function)(ArgTypes...),
			Type parentType
		)
			: Invokable(name)
			, m_parentType(parentType)
			, m_invoker(new FunctionInvoker<ReturnType, ArgTypes...>(function))
		{
			TypeUnpacker<ArgTypes...>::Apply(m_signature);
		}

		template<typename... Args>
		Variant Function::Invoke(Args&&... args) const
		{
			ArgumentList arguments{ std::forward<Args>(args)... };

			return InvokeVariadic(arguments);
		}
	}
}