#pragma once

#include "TypeUnpacker.hpp"

namespace GuGu {
	namespace meta {
		template<class ClassType, typename ReturnType, typename... ArgTypes>
		Method::Method(
			const GuGuUtf8Str& name,
			ReturnType(ClassType::* method)(ArgTypes...)
		)
			: Invokable(name)
			, m_isConst(false)
			, m_classType(typeof(ClassType))
			, m_invoker(new MethodInvoker<ClassType, ReturnType, ArgTypes...>(method))
		{
			TypeUnpacker<ArgTypes...>::Apply(m_signature);
		}
	}
}