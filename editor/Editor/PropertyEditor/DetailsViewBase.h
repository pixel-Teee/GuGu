#pragma once

#include "IDetailsViewPrivate.h"

namespace GuGu {
	class DetailsViewBase : public IDetailsViewPrivate
	{
	public:
		DetailsViewBase();
		virtual ~DetailsViewBase();
	};
}