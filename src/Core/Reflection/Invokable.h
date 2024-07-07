#pragma once

#include "InvokableConfig.h"
#include "ArgumentConfig.h"

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	namespace meta {
		class Invokable
		{
		public:
			Invokable(const GuGuUtf8Str& name = "INVALID");

			template<typename... Types>
			static InvokableSignature CreateSignature(void);

			const InvokableSignature& GetSignature(void) const;

			const GuGuUtf8Str& GetName(void) const;
		protected:
			GuGuUtf8Str m_name;

			InvokableSignature m_signature;
		};
	}
}

#include "Invokable.hpp"