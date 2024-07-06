#pragma once

namespace GuGu {
	namespace variant_policy
	{
		//包裹一个object指针，而不是拷贝值
		//这个保证了基类指针可以解析它的派生类型
		struct WrapObject {};

		//使用一个引用到一个值，而不是拷贝它
		struct NoCopy {};
	}
}