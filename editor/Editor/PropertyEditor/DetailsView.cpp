#include <pch.h>

#include "DetailsView.h"
#include <Core/UI/NullWidget.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>
#include "DetailsViewObjectFilter.h"//DetailsViewObjectRoot
#include "DetailsViewGenericObjectFilter.h"

namespace GuGu {

	void DetailsView::init(const BuilderArguments& arguments)
	{
		m_objectFilter = std::make_shared<DetailsViewDefaultObjectFilter>();

		std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox);

		verticalBox->addSlot()
		.StretchHeight(1)
		.setPadding(Padding(0, 0, 0, 0))
		(
			WIDGET_NEW(Overlay)
			+ Overlay::Slot()
			(
				constructTreeView()
			)
		);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = verticalBox;
	}

	DetailsView::~DetailsView()
	{

	}

	void DetailsView::setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh /*= false*/, bool bOverrideLock /*= false*/)
	{
		//m_unfilteredSelectedObjects.reserve(inObjects.size());
		//for (GameObject* inObject : inObjects)
		//{
		//	if (inObject)
		//	{
		//		m_unfilteredSelectedObjects.push_back()
		//	}
		//}
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

	void DetailsView::setObjectArrayPrivate(const std::vector<GameObject*>& inObjects)
	{
		const std::vector<DetailsViewObjectRoot> roots = m_objectFilter->filterObjects(inObjects);
	}

	std::shared_ptr<DetailTree> DetailsView::constructTreeView()
	{
		return WIDGET_ASSIGN_NEW(DetailTree, m_detailTree)
			   .treeItemSource(&m_rootTreeNodes)
			   .onGenerateRow(this, &DetailsView::onGenerateRowForDetailTree)
			   .onGetChildren(this, &DetailsView::onGetChildrenForDetailTree);
	}

}