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

		void setChildWidget(std::shared_ptr<Widget> widget);

		Padding getPadding();

		HorizontalAlignment getHorizontalAlignment();

		VerticalAlignment getVerticalAlignment();

		void setHorizontalAlignment(HorizontalAlignment horizontalAlignment);

		void setVerticalAlignment(VerticalAlignment verticalAlignment);

		void setPadding(Padding newPadding);
	private:
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;
		Padding m_padding;
		std::shared_ptr<Widget> m_childWidget;
	};
}