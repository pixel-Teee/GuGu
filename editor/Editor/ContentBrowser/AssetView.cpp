#include <pch.h>

#include "AssetView.h"

namespace GuGu {
	void AssetView::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		//m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		//create path view

	}
}