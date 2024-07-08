#include <pch.h>

#include "Variant.h"

namespace GuGu {
	namespace meta {
		Variant::Variant(void)
			: m_isConst(true)
			, m_base(nullptr)
		{
		}
		Variant::Variant(Variant&& rhs)
			: m_isConst(rhs.m_isConst)
			, m_base(rhs.m_base)
		{
			rhs.m_isConst = true;
			rhs.m_base = nullptr;
		}
		Variant::~Variant(void)
		{
			delete m_base;
		}
		Variant& Variant::operator=(const Variant& rhs)
		{
			Variant(rhs).Swap(*this);

			return *this;
		}
		Variant& Variant::operator=(Variant&& rhs)
		{
			rhs.Swap(*this);

			Variant().Swap(rhs);

			return *this;
		}
		Variant::Variant(const Variant& rhs)
			: m_isConst(rhs.m_isConst)
			, m_base(rhs.m_base ? rhs.m_base->Clone() : nullptr)
		{
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


