#include <pch.h>

#include "Slot.h"

namespace GuGu {
	Slot::Slot()
	{
	}
	Slot::~Slot()
	{
	}
	std::shared_ptr<Widget> Slot::getChildWidget()
	{
		return m_childWidget;
	}
}