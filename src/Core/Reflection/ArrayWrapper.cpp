#include <pch.h>

#include "ArrayWrapper.h"

#include "Variant.h"

namespace GuGu {
	namespace meta {
		ArrayWrapper::ArrayWrapper(void)
			: m_isConst(true)
			, m_base(nullptr)
		{
		}
		Variant ArrayWrapper::GetValue(size_t index) const
		{
			return m_base ? m_base->GetValue(index) : Variant();
		}
		void ArrayWrapper::SetValue(size_t index, const Argument& value)
		{
			assert(!m_isConst);

			if (m_base)
				m_base->SetValue(index, value);
		}
		void ArrayWrapper::Insert(size_t index, const Argument& value)
		{
			assert(!m_isConst);

			if (m_base)
				m_base->Insert(index, value);
		}
		void ArrayWrapper::Remove(size_t index)
		{
			assert(!m_isConst);

			if (m_base)
				m_base->Remove(index);
		}
		size_t ArrayWrapper::Size(void) const
		{
			return m_base ? m_base->Size() : 0;
		}
		bool ArrayWrapper::IsValid(void) const
		{
			return m_base != nullptr;
		}
		bool ArrayWrapper::IsConst(void) const
		{
			return m_isConst;
		}
	}
}