#pragma once

namespace GuGu {
	namespace meta {
		//TypeID是一个32位无符号整数
		typedef uint32_t TypeID; 

		const TypeID InvalidTypeID = 0;

		//估计每个类型都要特化
		template<typename T>
		struct TypeIDs
		{
			static TypeID ID;
		};

		template<typename T>
		TypeID TypeIDs<T>::ID = InvalidTypeID;
	}
}