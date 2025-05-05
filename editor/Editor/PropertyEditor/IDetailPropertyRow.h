#pragma once

#include <Core/UI/Splitter.h>

namespace GuGu {
	class DetailColumnSizeData
	{
	public:
		DetailColumnSizeData()
		{
			m_valueColumnWidth = 0.7f;
			m_rightColumnWidthValue = 0.0f;

			m_nameColumnWidth.bindRaw(this, &DetailColumnSizeData::getNameColumnWidth);
			m_valueColumnWidth.bindRaw(this, &DetailColumnSizeData::getValueColumnWidth);
		}

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
	class IDetailPropertyRow
	{
	public:
		virtual ~IDetailPropertyRow() {}
	};
}