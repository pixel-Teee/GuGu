#pragma once

namespace GuGu {
	namespace meta {
		//TypeID是一个32位无符号整数
		typedef uint32_t TypeID; 

		const TypeID InvalidTypeID = 0;

		//每个类型都要特化，以类型为索引，获取ID
		template<typename T>
		struct TypeIDs
		{
			static TypeID ID;
		};

		template<typename T>
		TypeID TypeIDs<T>::ID = InvalidTypeID;
	}
}