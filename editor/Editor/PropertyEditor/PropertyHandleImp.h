#pragma once

#include "PropertyHandle.h"

namespace GuGu {
	namespace meta {
		class Variant;
	}
	class PropertyNode;
	class GuGuUtf8Str;
	class PropertyValueImpl
	{
	public:
		PropertyValueImpl(std::shared_ptr<PropertyNode> inPropertyNode);

		PropertyAccess::Result getValueData(meta::Variant& fieldValue);

		PropertyAccess::Result importText(const GuGuUtf8Str& inValue);

		PropertyAccess::Result importText(const GuGuUtf8Str& inValue, PropertyNode* inPropertyNode);
	protected:
		std::weak_ptr<PropertyNode> m_propertyNode;
	};

	class PropertyNode;
#define DECLARE_PROPERTY_ACCESSOR(ValueType) \
	virtual PropertyAccess::Result setValue(ValueType const& inValue) override;\
	virtual PropertyAccess::Result getValue(ValueType& outValue) const override;

	class PropertyHandleBase : public IPropertyHandle
	{
	public:
		PropertyHandleBase(std::shared_ptr<PropertyNode> propertyNode);


		DECLARE_PROPERTY_ACCESSOR(float)

	protected:
		std::shared_ptr<PropertyValueImpl> m_implementation;
	};

	class PropertyHandleFloat : public PropertyHandleBase
	{
	public:
		PropertyHandleFloat(std::shared_ptr<PropertyNode> propertyNode);
		static bool supports(std::shared_ptr<PropertyNode> propertyNode);
		virtual PropertyAccess::Result getValue(float& outValue) const;
		virtual PropertyAccess::Result setValue(const float& inValue) override;
	};
}