#pragma once

#include "BasicElement.h"

namespace GuGu {
	class Widget;
	class Slot
	{
	public:
		Slot();

		virtual ~Slot();
		
		virtual std::shared_ptr<Widget> getChildWidget();
	private:
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;

		std::shared_ptr<Widget> m_childWidget;
	};
}