#pragma once

#include <Core/Math/MyMath.h>
#include "PropertyEditorManager.h"//PropertyAccess::Result

namespace GuGu {
	class Widget;
	class GuGuUtf8Str;
	namespace meta {
		class Field;
	}
	class IPropertyHandle : public std::enable_shared_from_this<IPropertyHandle>
	{
	public:
		virtual ~IPropertyHandle() {}

		virtual std::shared_ptr<Widget> createPropertyNameWidget(const GuGuUtf8Str& nameOverride) = 0;

		virtual PropertyAccess::Result setValue(const float& inValue) = 0;
		virtual PropertyAccess::Result getValue(float& outValue) const = 0;

		virtual PropertyAccess::Result setValue(const double& inValue) = 0;
		virtual PropertyAccess::Result getValue(double& outValue) const = 0;

		virtual PropertyAccess::Result setValue(const math::double3& inValue) = 0;
		virtual PropertyAccess::Result getValue(math::double3& outValue) const = 0;

		virtual PropertyAccess::Result getNumChildren(uint32_t& outNumChildren) const = 0;

		virtual PropertyAccess::Result setValueFromFormattedString(const GuGuUtf8Str& inValue) = 0;
		virtual PropertyAccess::Result getValueAsFormattedString(GuGuUtf8Str& outValue) const = 0;

		virtual std::shared_ptr<IPropertyHandle> getChildHandle(uint32_t index) const = 0;

		virtual const meta::Field* getField() const = 0;
	};
}