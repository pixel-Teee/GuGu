#pragma once

#include "Core/GuGuUtf8Str.h"

namespace GuGu {
	struct ComplexType
	{
		int a;
		double b;
		float c;
		GuGuUtf8Str str;

		int gettera() const { return a; }
		double getterb() const { return b; }
		float getterc() const { return c; }
		GuGuUtf8Str getterstr() const { return str; }
		//todo:add array
	};
	void testReflection();
}