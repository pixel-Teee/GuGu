#pragma once

#include "DetailsViewBase.h"
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class DetailsView : public DetailsViewBase
	{
	public:
		struct BuilderArguments : public Arguments<DetailsView>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~DetailsView();

	};
}