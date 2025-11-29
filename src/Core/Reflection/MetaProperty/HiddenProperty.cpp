#include <pch.h>

#include "HiddenProperty.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {

	namespace meta {
		static bool registermetaHiddenProperty()
		{
			auto& db = meta::ReflectionDatabase::Instance();
			auto id = db.AllocateType("meta::HiddenProperty");
			auto& type = db.types[id];
			meta::TypeInfo<meta::HiddenProperty>::Register(id, type, true, "896611C5-9216-4507-B180-F8B629696DED");
			return true;
		}

		IMPLEMENT_INITIAL_BEGIN(HiddenProperty)
			ADD_INITIAL_FUNCTION_WITH_PRIORITY(registermetaHiddenProperty)
		IMPLEMENT_INITIAL_END

		HiddenProperty::HiddenProperty()
		{

		}

		HiddenProperty::HiddenProperty(const HiddenProperty& rhs)
		{

		}

		void HiddenProperty::Update(float fElapsedTimeSeconds)
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

		meta::Type HiddenProperty::GetType() const
		{
			//throw std::logic_error("The method or operation is not implemented.");
			return typeof(HiddenProperty);
		}

		meta::Object* HiddenProperty::Clone(void) const
		{
			HiddenProperty* hiddenProperty = new HiddenProperty(*this);
			return hiddenProperty;
		}

		void HiddenProperty::PostLoad()
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

	}
}