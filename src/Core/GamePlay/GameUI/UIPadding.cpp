#include <pch.h>

#include "UIPadding.h"

namespace GuGu {

	float& UIPadding::getRight()
	{
		return right;
	}

	void UIPadding::setRight(float inRight)
	{
		right = inRight;
	}

	float& UIPadding::getBottom()
	{
		return bottom;
	}

	void UIPadding::setBottom(float inBottom)
	{
		bottom = inBottom;
	}

}