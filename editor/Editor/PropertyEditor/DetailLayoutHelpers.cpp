#include <pch.h>

#include "DetailLayoutHelpers.h"
#include "DetailLayoutBuilderImpl.h"
#include "PropertyNode.h"
#include "ObjectPropertyNode.h"
#include "CategoryPropertyNode.h"
#include "DetailCategoryBuilderImpl.h"
#include <Core/Reflection/Field.h>

namespace GuGu {
	namespace DetailLayoutHelpers
	{

		//inNode首次传入函数的时候，是ObjectPropertyNode
		void updateSinglePropertyMapRecursive(PropertyNode& inNode, GuGuUtf8Str curCategory, ComplexPropertyNode* curObjectNode, UpdatePropertyMapArgs& inUpdateArgs)
		{
			DetailLayoutData& layoutData = *inUpdateArgs.m_layoutData;
			DetailLayoutBuilderImpl& detailLayout = *layoutData.m_detailLayout;

			const meta::Field* parentField = inNode.getField();
			for (int32_t childIndex = 0; childIndex < inNode.getNumChildNodes(); ++childIndex)
			{
				std::shared_ptr<PropertyNode> childNodePtr = inNode.getChildNode(childIndex);
				PropertyNode& childNode = *childNodePtr;
				meta::Field* property = childNode.getField();

				UpdatePropertyMapArgs childArgs = inUpdateArgs;

				if (ObjectPropertyNode* objNode = childNode.asObjectNode())
				{
					updateSinglePropertyMapRecursive(childNode, curCategory, objNode, childArgs);
				}
				else if (CategoryPropertyNode* categoryNode = childNode.asCategoryNode())
				{
					GuGuUtf8Str categoryName = curCategory;
					if (categoryName != "")
					{
						DetailCategoryImpl& categoryImpl = detailLayout.defaultCategory(categoryName);
						categoryImpl.addPropertyNode(childNodePtr, "");
					}

					updateSinglePropertyMapRecursive(childNode, categoryNode->getCategoryName(), objNode, childArgs);
				}
				else
				{
					//check is class
					const bool bIsClass = property->GetType().IsClass();

					const bool bPushOutStructProps = bIsClass;

					GuGuUtf8Str instanceName = "";
					if (curObjectNode && curObjectNode->getParentNode())
					{
						instanceName = curObjectNode->getParentNode()->getField()->GetName();
					}
					else if (parentField)
					{
						instanceName = parentField->GetName();
					}
					if (!bPushOutStructProps)
					{
						GuGuUtf8Str categoryName = curCategory;

						DetailCategoryImpl& categoryImpl = detailLayout.defaultCategory(categoryName);
						categoryImpl.addPropertyNode(childNodePtr, instanceName);
					}

					bool bRecurseIntoChildren = bPushOutStructProps;
					if (bRecurseIntoChildren)
					{
						updateSinglePropertyMapRecursive(childNode, curCategory, curObjectNode, childArgs);
					}
				}
			}
		}

	}
}