#include <pch.h>

#include "Invokable.h"

namespace GuGu {
	namespace meta {
		Invokable::Invokable(const GuGuUtf8Str& name)
			: m_name(name)
		{
		}
		const InvokableSignature& Invokable::GetSignature(void) const
		{
			return m_signature;
		}
		const GuGuUtf8Str& Invokable::GetName(void) const
		{
			return m_name;
		}
	}
}