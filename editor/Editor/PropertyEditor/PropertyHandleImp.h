#pragma once

#include "PropertyHandle.h"

namespace GuGu {
	namespace meta {
		class Variant;
	}
	class PropertyNode;
	class GuGuUtf8Str;
	struct AssetData;
	class PropertyValueImpl
	{
	public:
		PropertyValueImpl(std::shared_ptr<PropertyNode> inPropertyNode);

		PropertyAccess::Result getValueData(meta::Variant& fieldValue);

		PropertyAccess::Result importText(const GuGuUtf8Str& inValue);

		PropertyAccess::Result importText(const GuGuUtf8Str& inValue, PropertyNode* inPropertyNode);

		PropertyAccess::Result setValueAsString(const GuGuUtf8Str& inValue);

		PropertyAccess::Result getValueAsDisplayString(GuGuUtf8Str& outString);

		std::shared_ptr<PropertyNode> getPropertyNode() const { return m_propertyNode.lock(); }

		int32_t getNumChildren() const;

		std::shared_ptr<PropertyNode> getChildNode(int32_t childIndex) const;
	protected:
		std::weak_ptr<PropertyNode> m_propertyNode;
	};

	class PropertyNode;
#define DECLARE_PROPERTY_ACCESSOR(ValueType) \
	virtual PropertyAccess::Result setValue(ValueType const& inValue) override;\
	virtual PropertyAccess::Result getValue(ValueType& outValue) const override;

	class Widget;

	class PropertyHandleBase : public IPropertyHandle
	{
	public:
		PropertyHandleBase(std::shared_ptr<PropertyNode> propertyNode);

		virtual std::shared_ptr<Widget> createPropertyNameWidget(const GuGuUtf8Str& nameOverride) override;

		virtual PropertyAccess::Result getNumChildren(uint32_t& outNumChildren) const override;

		virtual std::shared_ptr<IPropertyHandle> getChildHandle(uint32_t index) const override;

		virtual PropertyAccess::Result setValueFromFormattedString(const GuGuUtf8Str& inValue) override;

		virtual PropertyAccess::Result getValueAsFormattedString(GuGuUtf8Str& outValue) const override;

		virtual const meta::Field* getField() const override;

		DECLARE_PROPERTY_ACCESSOR(float)
		DECLARE_PROPERTY_ACCESSOR(double)
		DECLARE_PROPERTY_ACCESSOR(math::double3)
		DECLARE_PROPERTY_ACCESSOR(AssetData)
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

	class PropertyHandleVector3 : public PropertyHandleBase
	{
	public:
		PropertyHandleVector3(std::shared_ptr<PropertyNode> propertyNode);
		static bool supports(std::shared_ptr<PropertyNode> propertyNode);
		virtual PropertyAccess::Result getValue(math::double3& outValue) const override;
		virtual PropertyAccess::Result setValue(const math::double3& inValue) override;
	};

	class PropertyHandleDouble : public PropertyHandleBase
	{
	public:
		PropertyHandleDouble(std::shared_ptr<PropertyNode> propertyNode);
		static bool supports(std::shared_ptr<PropertyNode> propertyNode);
		virtual PropertyAccess::Result getValue(double& outValue) const override;
		virtual PropertyAccess::Result setValue(const double& inValue) override;
	};
	
	class PropertyHandleObject : public PropertyHandleBase
	{
	public:
		PropertyHandleObject(std::shared_ptr<PropertyNode> propertyNode);
		static bool supports(std::shared_ptr<PropertyNode> propertyNode);
		virtual PropertyAccess::Result getValue(AssetData& outValue) const override;
		virtual PropertyAccess::Result setValue(const AssetData& inValue) override;
	};
}