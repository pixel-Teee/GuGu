#include <pch.h>

#include "Enum.h"

#include "Variant.h"

namespace GuGu {
	namespace meta {
		Enum::Enum(const EnumBase* base)
			: m_base(base) {}

		bool Enum::IsValid(void) const
		{
			return m_base != nullptr;
		}

		Enum::operator bool(void) const
		{
			return m_base != nullptr;
		}

		bool Enum::operator==(const Enum& rhs) const
		{
			return m_base == rhs.m_base;
		}

		bool Enum::operator!=(const Enum& rhs) const
		{
			return m_base != rhs.m_base;
		}

		GuGuUtf8Str Enum::GetName(void) const
		{
			return m_base ? m_base->GetName() : GuGuUtf8Str();
		}

		Type Enum::GetType(void) const
		{
			return m_base ? m_base->GetType() : Type::Invalid();
		}

		Type Enum::GetParentType(void) const
		{
			return m_base ? m_base->GetParentType() : Type::Invalid();
		}

		Type Enum::GetUnderlyingType(void) const
		{
			return m_base ? m_base->GetUnderlyingType() : Type::Invalid();
		}

		std::vector<GuGuUtf8Str> Enum::GetKeys(void) const
		{
			return m_base ? m_base->GetKeys() : std::vector<GuGuUtf8Str>();
		}

		std::vector<Variant> Enum::GetValues(void) const
		{
			return m_base ? m_base->GetValues() : std::vector<Variant>();
		}

		GuGuUtf8Str Enum::GetKey(const Argument& value) const
		{
			return m_base ? m_base->GetKey(value) : GuGuUtf8Str();
		}

		Variant Enum::GetValue(const GuGuUtf8Str& key) const
		{
			return m_base ? m_base->GetValue(key) : Variant();
		}
	}
}