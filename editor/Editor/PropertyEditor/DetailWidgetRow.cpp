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

	bool DetailWidgetRow::hasNameContent() const
	{
		return m_nameWidget.m_widget != NullWidget::getNullWidget();
	}

	bool DetailWidgetRow::hasValueContent() const
	{
		return m_valueWidget.m_widget != NullWidget::getNullWidget();
	}

	bool DetailWidgetRow::hasColumns() const
	{
		return hasNameContent() || hasValueContent();
	}

	bool DetailWidgetRow::hasAnyContent() const
	{
		return hasColumns();
	}

}