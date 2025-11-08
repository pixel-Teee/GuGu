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

		std::shared_ptr<PropertyNode> getChildNode(const GuGuUtf8Str& childName, bool bRecurse) const;
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

		virtual std::shared_ptr<IPropertyHandle> getChildHandle(const GuGuUtf8Str& childName, bool bRecurse = true) const override;

		virtual PropertyAccess::Result setValueFromFormattedString(const GuGuUtf8Str& inValue) override;

		virtual PropertyAccess::Result getValueAsFormattedString(GuGuUtf8Str& outValue) const override;

		virtual const meta::Field* getField() const override;

		virtual std::shared_ptr<PropertyNode> getPropertyNode() const;

		virtual void getOuterObjects(std::vector<meta::Object*>& outerObjects) const;

		DECLARE_PROPERTY_ACCESSOR(float)
		DECLARE_PROPERTY_ACCESSOR(double)
		DECLARE_PROPERTY_ACCESSOR(math::double3)
		DECLARE_PROPERTY_ACCESSOR(AssetData)
		DECLARE_PROPERTY_ACCESSOR(int8_t)
		DECLARE_PROPERTY_ACCESSOR(int16_t)
		DECLARE_PROPERTY_ACCESSOR(int32_t)
		DECLARE_PROPERTY_ACCESSOR(int64_t)
		DECLARE_PROPERTY_ACCESSOR(uint8_t)
		DECLARE_PROPERTY_ACCESSOR(uint16_t)
		DECLARE_PROPERTY_ACCESSOR(uint32_t)
		DECLARE_PROPERTY_ACCESSOR(uint64_t)
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

	class PropertyHandleRotator : public PropertyHandleBase
	{
	public:
		PropertyHandleRotator(std::shared_ptr<PropertyNode> propertyNode);
		static bool supports(std::shared_ptr<PropertyNode> propertyNode);
		virtual PropertyAccess::Result getValue(math::double3& outValue) const override;
		virtual PropertyAccess::Result setValue(const math::double3& inValue) override;
	};

	class PropertyHandleInt : public PropertyHandleBase
	{
	public:
		PropertyHandleInt(std::shared_ptr<PropertyNode> propertyNode);
		static bool supports(std::shared_ptr<PropertyNode> propertyNode);

		virtual PropertyAccess::Result getValue(int8_t& outValue) const override;
		virtual PropertyAccess::Result setValue(const int8_t& inValue) override;

		virtual PropertyAccess::Result getValue(int16_t& outValue) const override;
		virtual PropertyAccess::Result setValue(const int16_t& inValue) override;

		virtual PropertyAccess::Result getValue(int32_t& outValue) const override;
		virtual PropertyAccess::Result setValue(const int32_t& inValue) override;

		virtual PropertyAccess::Result getValue(int64_t& outValue) const override;
		virtual PropertyAccess::Result setValue(const int64_t& inValue) override;

		virtual PropertyAccess::Result getValue(uint8_t& outValue) const override;
		virtual PropertyAccess::Result setValue(const uint8_t& inValue) override;

		virtual PropertyAccess::Result getValue(uint16_t& outValue) const override;
		virtual PropertyAccess::Result setValue(const uint16_t& inValue) override;

		virtual PropertyAccess::Result getValue(uint32_t& outValue) const override;
		virtual PropertyAccess::Result setValue(const uint32_t& inValue) override;

		virtual PropertyAccess::Result getValue(uint64_t& outValue) const override;
		virtual PropertyAccess::Result setValue(const uint64_t& inValue) override;
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