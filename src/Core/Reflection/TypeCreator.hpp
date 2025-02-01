#pragma once

#include "constructor.h"

namespace GuGu {
	namespace meta {
		template<typename ...Args>
		Variant TypeCreator::Create(const Type& type, Args &&...args)
		{
			static InvokableSignature signature;

			static bool initial = true;

			if (initial)
			{
				TypeUnpacker<Args...>::Apply(signature);

				initial = false;
			}

			auto& constructor = type.GetConstructor(signature);

			return constructor.Invoke(std::forward<Args>(args)...);
		}
	}
}