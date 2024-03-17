#pragma once

#include "Style.h"//TextBlockStyle depends this header file

namespace GuGu {
	class Widget;
	class ITextLayoutMarshaller;
	class TextBlockLayout
	{
	public:
		TextBlockLayout(Widget* inOwner, TextBlockStyle inDefaultTextStyle, std::shared_ptr<ITextLayoutMarshaller> inMarshaller);

		virtual ~TextBlockLayout();
	private:
	};
}