#pragma once

#include "MetaContainer.h"
#include "Invokable.h"

#include "MethodInvoker.h"

namespace GuGu {
	namespace meta
	{
		class MethodInvokerBase;
		class Method : public MetaContainer, public Invokable //Invokable 存储了参数的列表
		{
		public:
			Method(void);

			template<class ClassType, typename ReturnType, typename... ArgTypes>
			Method(const GuGuUtf8Str& name, ReturnType(ClassType::* method)(ArgTypes...));

			//detect const-ness
			template<class ClassType, typename ReturnType, typename... ArgTypes>
			Method(const GuGuUtf8Str& name, ReturnType(ClassType::* method)(ArgTypes...) const);

			static const Method& Invalid(void);

			Type GetClassType(void) const;

			bool IsValid(void) const;
			bool IsConst(void) const;

			Variant Invoke(Variant& instance, ArgumentList& arguments) const;

			template<typename... Args>
			Variant Invoke(Variant& instance, Args&&... args) const;

		private:
			bool m_isConst;

			Type m_classType;

			//add method invoker base
			std::shared_ptr<MethodInvokerBase> m_invoker;
		};
	}
}

#include "Method.hpp"