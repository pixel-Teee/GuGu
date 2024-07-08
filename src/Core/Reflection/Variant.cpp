#include <pch.h>

#include "Variant.h"

namespace GuGu {
	namespace meta {
		Variant::Variant(void)
			: m_isConst(true)
			, m_base(nullptr)
		{
		}
		Variant::~Variant(void)
		{
			delete m_base;
		}
		bool Variant::IsValid(void) const
		{
			return m_base != nullptr;
		}
		bool Variant::IsConst(void) const
		{
			return m_isConst;
		}
		bool Variant::IsArray(void) const
		{
			return m_base ? m_base->IsArray() : false;
		}
		void* Variant::getPtr(void) const
		{
			return m_base ? m_base->GetPtr() : nullptr;
		}
	}
}


