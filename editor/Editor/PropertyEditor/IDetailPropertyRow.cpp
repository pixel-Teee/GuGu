#include <pch.h>

#include "IDetailPropertyRow.h"

namespace GuGu {
	DetailColumnSizeData::DetailColumnSizeData()
	{
		m_valueColumnWidthValue = 0.4f;
		m_rightColumnWidthValue = 0.0f;

		m_nameColumnWidth.bindRaw(this, &DetailColumnSizeData::getNameColumnWidth);
		m_valueColumnWidth.bindRaw(this, &DetailColumnSizeData::getValueColumnWidth);
	}

}

