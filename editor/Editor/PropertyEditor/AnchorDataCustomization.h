#pragma once

#include "IPropertyTypeCustomization.h"

namespace GuGu {
	class AnchorDataCustomization : public IPropertyTypeCustomization
	{
	public:
		static std::shared_ptr<AnchorDataCustomization> create();

		virtual void customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow) override;

		virtual void cutomizeChildren(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder) override;

	private:
		void customizeLayoutData(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder);

		void customizeAnchors(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder);
	};
}