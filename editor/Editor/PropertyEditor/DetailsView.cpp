#include <pch.h>

#include "DetailsView.h"
#include <Core/UI/NullWidget.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>
#include <Editor/StyleSet/EditorStyleSet.h>
#include "DetailsViewObjectFilter.h"//DetailsViewObjectRoot
#include "DetailsViewGenericObjectFilter.h"
#include "ObjectPropertyNode.h"
#include <Core/GamePlay/GameObject.h>

namespace GuGu {

	void DetailsView::init(const BuilderArguments& arguments)
	{
		m_objectFilter = std::make_shared<DetailsViewDefaultObjectFilter>();

		std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox);

		verticalBox->addSlot()
		.StretchHeight(1)
		//.FixedHeight()
		.setPadding(Padding(10, 10, 10, 10))
		(
			WIDGET_NEW(Overlay)
			+ Overlay::Slot()
			(
				WIDGET_NEW(Border) //背景
				.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel5"))
				.verticalAlignment(VerticalAlignment::Stretch)
				.Content
				(
					constructTreeView()
				)
			)
		);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = verticalBox;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	DetailsView::~DetailsView()
	{

	}

	RootPropertyNodeList& DetailsView::getRootNodes()
	{
		return m_rootPropertyNodes;
	}

	void DetailsView::setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh /*= false*/, bool bOverrideLock /*= false*/)
	{
// 		m_unfilteredSelectedObjects.reserve(inObjects.size());
// 		for (GameObject* inObject : inObjects)
// 		{
// 			if (inObject)
// 			{
// 				m_unfilteredSelectedObjects.push_back()
// 			}
// 		}

		setObjectArrayPrivate(inObjects);
	}

	void DetailsView::setObjects(const std::vector<std::weak_ptr<GameObject>>& inObjects, bool bForceRefresh /*= false*/, bool bOverrideLock /*= false*/)
	{
		m_unfilteredSelectedObjects.reserve(inObjects.size());

		std::vector<GameObject*> objects;
		for (std::weak_ptr<GameObject> inObject : inObjects)
		{
			if (inObject.lock())
			{
				m_unfilteredSelectedObjects.push_back(inObject);
				objects.push_back(inObject.lock().get());//raw pointer
			}
		}
		setObjectArrayPrivate(objects);
	}

	DetailColumnSizeData& DetailsView::getColumnSizeData()
	{
		return m_columnSizeData;
	}

	void DetailsView::setObjectArrayPrivate(const std::vector<GameObject*>& inObjects)
	{
		const std::vector<DetailsViewObjectRoot> roots = m_objectFilter->filterObjects(inObjects);//一个 details view object root 可以存放多个objects

		preSetObject(roots.size());//创建 root property nodes(由 object property node 创建的数组)

		for (int32_t rootIndex = 0; rootIndex < roots.size(); ++rootIndex)
		{
			const DetailsViewObjectRoot& root = roots[rootIndex];

			ObjectPropertyNode* rootNode = m_rootPropertyNodes[rootIndex]->asObjectNode();

			for (GameObject* object : root.m_objects)
			{
				if (object != nullptr) //把 objects 放入 property node 里面
				{
					const Array<std::shared_ptr<Component>>& components = object->getComponents();
					for (int32_t i = 0; i < components.size(); ++i)
					{
						rootNode->addObject(components[i].get()); //属性节点
					}
				}
			}
		}

		postSetObjects(roots);
	}

	void DetailsView::preSetObject(int32_t inNewNumObjects)
	{
		m_rootPropertyNodes.clear();
		m_rootPropertyNodes.reserve(inNewNumObjects);
		for (int32_t newRootIndex = 0; newRootIndex < inNewNumObjects; ++newRootIndex)
		{
			m_rootPropertyNodes.push_back(std::make_shared<ObjectPropertyNode>());
		}
	}

	void DetailsView::postSetObjects(const std::vector<DetailsViewObjectRoot>& roots)
	{
		PropertyNodeInitParams initParams;
		initParams.m_parentNode = nullptr;
		initParams.m_property = nullptr;
		//后续增加数组

		for (std::shared_ptr<ComplexPropertyNode>& complexRootNode : m_rootPropertyNodes)
		{
			ObjectPropertyNode* rootPropertyNode = complexRootNode->asObjectNode();
			rootPropertyNode->initNode(initParams);
		}

		updatePropertyMaps();
		updateFilteredDetails();//获取由updatePropertyMaps生成的root tree nodes，并刷新树
	}

	std::shared_ptr<DetailTree> DetailsView::constructTreeView()
	{
		return WIDGET_ASSIGN_NEW(DetailTree, m_detailTree)
			   .visibility(Visibility::Visible)
			   .treeItemSource(&m_rootTreeNodes)
			   .onGenerateRow(this, &DetailsView::onGenerateRowForDetailTree)
			   .onGetChildren(this, &DetailsView::onGetChildrenForDetailTree);
	}

}