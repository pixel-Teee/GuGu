#include <pch.h>

#include "ArrayWrapper.h"

#include "Variant.h"

namespace GuGu {
	namespace meta {
		ArrayWrapper::ArrayWrapper(void)
		{
		}
		Variant ArrayWrapper::GetValue(size_t index) const
		{
			return Variant();
		}
		void ArrayWrapper::SetValue(size_t index, const Argument& value)
		{
		}
		void ArrayWrapper::Insert(size_t index, const Argument& value)
		{
		}
		void ArrayWrapper::Remove(size_t index)
		{
		}
		size_t ArrayWrapper::Size(void) const
		{
			return size_t();
		}
		bool ArrayWrapper::IsValid(void) const
		{
			return false;
		}
		bool ArrayWrapper::IsConst(void) const
		{
			return false;
		}
	}
}