#include <pch.h>

#include "DetailCategoryBuilderImpl.h"
#include "DetailItemNode.h"
#include "DetailPropertyRow.h"
#include "DetailCategoryTableRow.h"
#include "IDetailsViewPrivate.h"
#include "DetailLayoutBuilderImpl.h"
#include "PropertyNode.h"
#include "ObjectPropertyNode.h"
#include <Core/UI/Button.h>
#include <Core/Reflection/Object.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/World.h>
#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {

	DetailCategoryImpl::DetailCategoryImpl(const GuGuUtf8Str& categoryName, std::shared_ptr<DetailLayoutBuilderImpl> inDetailLayoutBuilderImpl)
		: m_headerContentWidget(nullptr)
		, m_bShowOnlyChildren(false)
	{
		m_categoryName = categoryName;
		m_detailLayoutBuilder = inDetailLayoutBuilderImpl;
	}

	DetailCategoryImpl::~DetailCategoryImpl()
	{

	}

	void DetailCategoryImpl::generateLayout()
	{
		m_simpleChildNodes.clear();
		m_inlinePropertyNode.reset();
		generateChildrenForLayouts();
	}

	void DetailCategoryImpl::addPropertyNode(std::shared_ptr<PropertyNode> propertyNode, GuGuUtf8Str instanceName)
	{
		DetailLayoutCustomization newCustomization;
		newCustomization.m_propertyRow = std::make_shared<DetailPropertyRow>(propertyNode, shared_from_this());
		addDefaultLayout(newCustomization, instanceName);
	}

	void DetailCategoryImpl::addDefaultLayout(const DetailLayoutCustomization& defaultLayoutInfo, GuGuUtf8Str instanceName)
	{
		getLayoutForInstance(instanceName).addLayout(defaultLayoutInfo);
	}

	DetailLayout& DetailCategoryImpl::getLayoutForInstance(GuGuUtf8Str instanceName)
	{
		return m_layoutMap.findOrAdd(instanceName);
	}

	void DetailCategoryImpl::getChildren(DetailNodeList& outChildren)
	{
		getGeneratedChildren(outChildren, true, false);
	}

	void DetailCategoryImpl::getGeneratedChildren(DetailNodeList& outChildren, bool bIgnoreVisibility, bool bIgnoreAdvancedDropdown) //todo:修复这个
	{
		for (std::shared_ptr<DetailTreeNode>& child : m_simpleChildNodes)
		{
			if (bIgnoreVisibility)
			{
				//是否儿子有自定义的儿子构建器来构建布局
				if (child->shouldShowOnlyChildren())
				{
					child->getChildren(outChildren);
				}
				else
				{
					outChildren.push_back(child);
				}
			}
		}
	}

	void DetailCategoryImpl::setDisplayName(GuGuUtf8Str inCategoryName)
	{
		m_displayName = inCategoryName;
	}

	IDetailsViewPrivate* DetailCategoryImpl::getDetailsView() const
	{
		std::shared_ptr<DetailLayoutBuilderImpl> detailLayoutBuilderPtr = m_detailLayoutBuilder.lock();
		if (detailLayoutBuilderPtr)
		{
			return detailLayoutBuilderPtr->getDetailsView();
		}
		return nullptr;
	}

	IDetailsView* DetailCategoryImpl::getNodeDetailsView() const
	{
		return getDetailsView();
	}

	bool DetailCategoryImpl::shouldShowOnlyChildren() const
	{
		return m_bShowOnlyChildren;
	}

	//std::shared_ptr<Widget> DetailCategoryImpl::onGetContextMenuContent() const
	//{
	//	GuGuUtf8Str categoryName = getCategoryName();
	//	//menu
	//	if (m_detailLayoutBuilder.lock())
	//	{
	//		std::shared_ptr<ComplexPropertyNode> propertyNode = m_detailLayoutBuilder.lock()->getRootNode();
	//		if (propertyNode)
	//		{
	//			ObjectPropertyNode* objectPropertyNode = propertyNode->asObjectNode();
	//			if (objectPropertyNode)
	//			{
	//				for (int32_t i = 0; i < objectPropertyNode->getNumChildNodes(); ++i)
	//				{
	//					meta::Object* object = objectPropertyNode->getInstanceAsObject(i);
	//					if (object->GetType() == meta::Type::GetFromName(categoryName))
	//					{
	//						//delete this object and refresh details view
	//						return WIDGET_NEW(Button)
	//							.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
	//							.ClickedLambda([=]() {
	//								//meta::Variant startVarint = ObjectVariant(object);
	//								Component* comp = static_cast<Component*>(object);
	//								if (comp != nullptr)
	//								{
	//									std::shared_ptr<GameObject> parentGameObject = comp->getParentGameObject().lock();
	//									//meta::Variant owner = objectPropertyNode->getParentNode()->getOwnerFieldVarint(startVarint);
	//									if (parentGameObject)
	//									{
	//										//delete component
	//										parentGameObject->deleteComponent(comp->GetType().GetName());
	//										//refresh details view
	//										std::vector<GameObject*> gameObjects;
	//										gameObjects.push_back(parentGameObject.get());
	//										getDetailsView()->setObjects(gameObjects, true);
	//										//World::getWorld()->m_onLevelChanged = std::bind(&ObjectModeInteractive::onLevelChanged, this);
	//									}
	//								}
	//								
	//								return Reply::Handled();
	//							})
	//							.Content
	//							(
	//								WIDGET_NEW(TextBlockWidget)
	//								.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
	//								.text("delete component")
	//							);
	//					}
	//				}
	//			}
	//		}
	//	}
	//	return NullWidget::getNullWidget();
	//}

	std::shared_ptr<ITableRow> DetailCategoryImpl::generateWidgetForTableView(const std::shared_ptr<TableViewBase>& ownerTable)
	{
		std::shared_ptr<Widget> headerContent = m_headerContentWidget;

		return WIDGET_NEW(DetailCategoryTableRow, shared_from_this(), ownerTable, m_detailLayoutBuilder.lock())
			   .displayName(getDisplayName())
			   .headerContent(headerContent);
			   //.onContextMenuOpening(this, &DetailCategoryImpl::onGetContextMenuContent);
	}

	void DetailCategoryImpl::generateChildrenForLayouts()
	{
		for (int32_t layoutIndex = 0; layoutIndex < m_layoutMap.size(); ++layoutIndex)
		{
			const DetailLayout& layout = m_layoutMap[layoutIndex];
			generateNodesFromCustomizations(layout.getSimpleLayouts(), m_simpleChildNodes);
		}
	}

	void DetailCategoryImpl::generateNodesFromCustomizations(const std::vector<DetailLayoutCustomization>& inCustomizationList, 
		 DetailNodeList& outNodeList)
	{
		std::vector<DetailLayoutCustomization> customizationList = inCustomizationList;
		for (const DetailLayoutCustomization& customization : customizationList)
		{
			std::shared_ptr<DetailItemNode> newNode = std::make_shared<DetailItemNode>(customization, shared_from_this());

			newNode->initialize();

			outNodeList.push_back(newNode);
		}
	}

	DetailLayoutCustomization::DetailLayoutCustomization()
	{

	}

	GuGuUtf8Str DetailLayoutCustomization::getName() const
	{
		return "";//todo:
	}

	std::shared_ptr<GuGu::PropertyNode> DetailLayoutCustomization::getPropertyNode() const
	{
		return m_propertyRow ? m_propertyRow->getPropertyNode() : nullptr;
	}

	DetailWidgetRow DetailLayoutCustomization::getWidgetRow() const
	{
		if (hasPropertyNode())
		{
			return m_propertyRow->getWidgetRow();
		}
	}

	void DetailLayout::addLayout(const DetailLayoutCustomization& layout)
	{
		m_simpleLayouts.push_back(layout);
	}

}