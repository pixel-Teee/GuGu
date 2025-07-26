#pragma once

namespace GuGu {
	class IPropertyHandle;
	class DetailWidgetRow;
	class IDetailChildrenBuilder;
	class IPropertyTypeCustomization : std::enable_shared_from_this<IPropertyTypeCustomization>
	{
	public:
		virtual ~IPropertyTypeCustomization() {}

		virtual void customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow) = 0;

		virtual void cutomizeChildren(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder) = 0;
	private:

	};
}