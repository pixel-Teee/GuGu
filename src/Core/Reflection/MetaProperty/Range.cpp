#include <pch.h>

#include "Range.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {

	namespace meta {
		static bool registermetaRange()
		{
			auto& db = meta::ReflectionDatabase::Instance();
			auto id = db.AllocateType("meta::Range");
			auto& type = db.types[id];
			meta::TypeInfo<meta::Range>::Register(id, type, true, "F50D477C-925E-4773-93EC-038032572F09");
			return true;
		}

		IMPLEMENT_INITIAL_BEGIN(Range)
			ADD_INITIAL_FUNCTION_WITH_PRIORITY(registermetaRange)
		IMPLEMENT_INITIAL_END

		Range::Range(float inMinValue, float inMaxValue)
		: m_minValue(inMinValue), m_maxValue(inMaxValue)
		{

		}

		Range::Range(const Range& rhs)
		{
			m_minValue = rhs.m_minValue;
			m_maxValue = rhs.m_maxValue;
		}

		void Range::Update(float fElapsedTimeSeconds)
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

		meta::Type Range::GetType() const
		{
			//throw std::logic_error("The method or operation is not implemented.");
			return typeof(Range);
		}

		meta::Object* Range::Clone(void) const
		{
			Range* newRange = new Range(*this);
			newRange->m_minValue = m_minValue;
			newRange->m_maxValue = m_maxValue;
			return newRange;
		}

		void Range::PostLoad()
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

	}
}