#pragma once

#include <Core/UI/Splitter.h>

namespace GuGu {
	class DetailColumnSizeData
	{
	public:
		DetailColumnSizeData();

		Attribute<float> m_nameColumnWidth;
		Attribute<float> m_valueColumnWidth;
		Attribute<float> m_propertyColumnWidth;
		Splitter::OnSlotResized m_onNameColumnResized;
		Splitter::OnSlotResized m_onValueColumnResized;

	private:
		float m_valueColumnWidthValue;
		float m_rightColumnWidthValue;

		float getNameColumnWidth() const { return 1.0f - (m_valueColumnWidthValue + m_rightColumnWidthValue); }
		float getValueColumnWidth() const { return m_valueColumnWidthValue; }
	};
	class DetailWidgetRow;
	class IDetailPropertyRow
	{
	public:
		virtual ~IDetailPropertyRow() {}

		virtual DetailWidgetRow& customWidget(bool bShowChildren = false) = 0;
	};
}