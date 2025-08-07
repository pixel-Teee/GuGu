#pragma once

#include "MetaContainer.h"
#include "Argument.h"
#include "Invokable.h"
#include "Variant.h"
#include "FunctionInvoker.h" //FunctionInvoker

namespace GuGu {
	namespace meta {
		class Function : public MetaContainer, public Invokable
		{
		public:
			using Invoker = std::function<Variant(ArgumentList&)>;

			Function();

			template<typename ReturnType, typename... ArgTypes>
			Function(const GuGuUtf8Str& name, ReturnType(*function)(ArgTypes...), Type parentType = Type::Invalid());

			static const Function& Invalid();

			Type GetParentType() const;

			bool IsValid() const;

			Variant InvokeVariadic(ArgumentList& arguments) const;

			template<typename... Args>
			Variant Invoke(Args&&... args) const;

		private:
			Type m_parentType;

			std::shared_ptr<FunctionInvokerBase> m_invoker;
		};
	}
}

#include "Function.hpp"