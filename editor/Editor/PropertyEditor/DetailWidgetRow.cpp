#include <pch.h>

#include "DetailWidgetRow.h"

namespace GuGu {

	DetailWidgetRow::DetailWidgetRow()
		: m_nameWidget(*this, 0.0f, 0.0f, HorizontalAlignment::Left, VerticalAlignment::Center)
		, m_valueWidget(*this, 0.0f, 0.0f, HorizontalAlignment::Left, VerticalAlignment::Center)
	{

	}

	DetailWidgetRow::~DetailWidgetRow()
	{

	}

}