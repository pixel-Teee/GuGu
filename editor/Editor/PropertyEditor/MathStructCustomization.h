#pragma once

#include "IPropertyTypeCustomization.h"

namespace GuGu {
	class MathStructCustomization : public IPropertyTypeCustomization
	{
	public:
		virtual void customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow);

	protected:
		virtual void makeHeaderRow(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow);
	};
}