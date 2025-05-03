#include <pch.h>

#include "DetailLayoutHelpers.h"
#include "DetailLayoutBuilderImpl.h"
#include "PropertyNode.h"
#include "ObjectPropertyNode.h"
#include "CategoryPropertyNode.h"
#include "DetailCategoryBuilderImpl.h"
#include "PropertyEditorManager.h"
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
					//check is struct
					const bool bIsStruct = property->GetType().IsStruct();

					//检查是否是自定义控件的结构体
					bool bIsCustomizedStruct = false;
					bool bIsChildOfCustomizedStruct = false;

					if(property)
						bIsCustomizedStruct = PropertyEditorManager::getPropertyEditorManager()->isCustomizedStruct(property);
					if(parentField)
						bIsChildOfCustomizedStruct = PropertyEditorManager::getPropertyEditorManager()->isCustomizedStruct(parentField);

					//是否去推入结构体的属性去他们自己的目录或者展示它们在一个可展开的结构体的内部
					const bool bPushOutStructProps = bIsStruct && !bIsCustomizedStruct;

					GuGuUtf8Str instanceName = "";
					if (curObjectNode && curObjectNode->getParentNode())
					{
						instanceName = curObjectNode->getParentNode()->getField()->GetName();
					}
					else if (parentField)
					{
						instanceName = parentField->GetName();
					}
					if (!bPushOutStructProps)//自定义结构体
					{
						GuGuUtf8Str categoryName = curCategory;//todo：后续会用Meta里面的目录

						DetailCategoryImpl& categoryImpl = detailLayout.defaultCategory(categoryName);
						categoryImpl.addPropertyNode(childNodePtr, instanceName);
						//categoryName是TransformComponent，遍历到(x, y, z, w)等字段的时候，先查找instance name，即m_translation
						//生成一个layout map对应的m_translation，再在这个layout map下面，生成(x, y, z, w)等customization
					}

					bool bRecurseIntoChildren = !bIsChildOfCustomizedStruct //不递归进内建结构体的儿子，因为我们已经知道如何去显示他们了
						&& (!bIsStruct || bPushOutStructProps);//只递归进结构体属性，如果他们可以显示为单独的属性在目录中，而不是在一个目录中的一个可展开区域
					if (bRecurseIntoChildren)
					{
						updateSinglePropertyMapRecursive(childNode, curCategory, curObjectNode, childArgs);
					}
				}
			}
		}

	}
}