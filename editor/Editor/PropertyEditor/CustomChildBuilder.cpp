#include <pch.h>

#include "CustomChildBuilder.h"
#include "PropertyHandleImp.h" //PropertyHandleBase
#include "DetailPropertyRow.h"

namespace GuGu {

	CustomChildrenBuilder::CustomChildrenBuilder(std::shared_ptr<DetailCategoryImpl> inParentCategory)
		: m_parentCategory(inParentCategory)
	{

	}

	IDetailPropertyRow& CustomChildrenBuilder::addProperty(std::shared_ptr<IPropertyHandle> propertyHandle)
	{
		DetailLayoutCustomization newCustomization;
		newCustomization.m_propertyRow = std::make_shared<DetailPropertyRow>(std::static_pointer_cast<PropertyHandleBase>(propertyHandle)->getPropertyNode(),
			m_parentCategory.lock());

		m_childCustomization.push_back(newCustomization);

		return *newCustomization.m_propertyRow;
	}

}