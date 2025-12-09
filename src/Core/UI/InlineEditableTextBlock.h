#pragma once


#include <Core/UI/Widget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/WidgetDelegates.h>
#include <Core/UI/CompoundWidget.h>

namespace GuGu {
	//inline editable text(use for rename)
	class InlineEditableTextBlock : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<InlineEditableTextBlock>
		{
			BuilderArguments()
			{

			}

			~BuilderArguments() = default;
		};

	};
}