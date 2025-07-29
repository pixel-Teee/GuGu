#pragma once

#include "IPropertyTypeCustomization.h"

#include <Core/UI/BasicElement.h> //Orientation
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class GuGuUtf8Str;
	class IDetailPropertyRow;
	class AnchorDataCustomization : public IPropertyTypeCustomization
	{
	public:
		static std::shared_ptr<AnchorDataCustomization> create();

		virtual void customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow) override;

		virtual void cutomizeChildren(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder) override;

		void createEditorWithDynamicLabel(IDetailPropertyRow& propertyRow, Attribute<GuGuUtf8Str> textAttribute);
	private:
		void customizeLayoutData(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder);

		void customizeAnchors(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder);

		void customizeOffsets(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder);

		static GuGuUtf8Str getOffsetLabel(std::shared_ptr<IPropertyHandle> propertyHandle, Orientation orientation, const GuGuUtf8Str& nonStretchingLabel, const GuGuUtf8Str& stretchingLabel);
	};
}