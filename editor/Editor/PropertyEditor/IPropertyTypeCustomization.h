#pragma once

namespace GuGu {
	class IPropertyHandle;
	class DetailWidgetRow;
	class IPropertyTypeCustomization : std::enable_shared_from_this<IPropertyTypeCustomization>
	{
	public:
		virtual ~IPropertyTypeCustomization() {}

		virtual void customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow) = 0;

	private:

	};
}