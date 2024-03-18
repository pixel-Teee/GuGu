#pragma once

#include "TextLayout.h"
#include "Style.h"

namespace GuGu {
	class Widget;
	class GuGuTextLayout : public TextLayout
	{
	public:
		GuGuTextLayout(Widget* inOwner, TextBlockStyle inDefaultTextStyle);

		virtual ~GuGuTextLayout();

		const TextBlockStyle& getDefaultTextStyle() const;

	private:
		TextBlockStyle m_defaultTextStyle;

		//todo:handle this
		//std::shared_ptr<Widget> m_children;
	};
}