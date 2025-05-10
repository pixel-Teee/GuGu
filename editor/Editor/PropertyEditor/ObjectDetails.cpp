#include <pch.h>

#include "ObjectDetails.h"
#include "DetailsView.h"//details view

namespace GuGu {

	void ObjectDetails::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		std::shared_ptr<DetailsView> detailsView = WIDGET_NEW(DetailsView);

		m_detailsView = detailsView;
		m_childWidget->m_childWidget = detailsView;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	ObjectDetails::~ObjectDetails()
	{

	}

	void ObjectDetails::setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh /*= false*/)
	{
		m_detailsView->setObjects(inObjects, bForceRefresh);
	}

}