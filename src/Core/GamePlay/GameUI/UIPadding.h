#pragma once

namespace GuGu {
	struct UIPadding
	{
		float left;

		float top;

		float right;

		float bottom;

		UIPadding() {
			left = top = right = bottom = 0;
		}

		UIPadding(float inLeft, float inTop, float inRight, float inBottom)
			: left(inLeft)
			, top(inTop)
			, right(inRight)
			, bottom(inBottom)
		{}
	};
}