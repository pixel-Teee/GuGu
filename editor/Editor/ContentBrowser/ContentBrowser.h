#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class ContentBrowser : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<ContentBrowser>
		{
			BuilderArguments(){}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~ContentBrowser();
	private:
	};
}