#pragma once

#include "StyleSet.h"

namespace GuGu {
	class CoreStyle : public StyleSet
	{
	public:
		CoreStyle();

		virtual ~CoreStyle();

		static std::shared_ptr<StyleSet> getStyle();
	private:

	};
}