#pragma once

#include "Type.h" //TypeID
#include "TypeUnpacker.hpp"

namespace std
{
	template<>
	struct hash<GuGu::meta::InvokableSignature>
	{
		size_t operator()(
			const GuGu::meta::InvokableSignature& signature
			) const
		{
			hash<GuGu::meta::TypeID> hasher;

			size_t seed = 0;

			for(auto& type : signature)
				seed ^= hasher(type.GetID()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

			return seed;
		}
	};
}

namespace GuGu {
	namespace meta {
		template<typename... Types>
		InvokableSignature Invokable::CreateSignature(void)
		{
			static InvokableSignature signature; //存储参数的列表

			static auto initial = true;

			if (initial)
			{
				TypeUnpacker<Types...>::Apply(signature);

				initial = false;
			}

			return signature;
		}
	}
}