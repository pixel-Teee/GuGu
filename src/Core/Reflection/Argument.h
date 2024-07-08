#pragma once

#include "TypeConfig.h"//一些类型萃取机制

#include <vector>

namespace GuGu {
	namespace meta {
		class Type;
		class Variant;

		class Argument
		{
		public:
			Argument(void);
			Argument(const Argument& rhs);
			Argument(Variant& obj);
			Argument(const Variant& obj);

			template<typename T>
			Argument(const T& data);

			template<typename T>
			Argument(T& data);

			Argument& operator=(const Argument& rhs);

			Type GetType(void) const;

			void* GetPtr(void) const;

			template<typename T>
			T& GetValue(void) const;
		private:
			const TypeID m_typeID;
			const bool m_isArray;

			const void* m_data;
		};
	}
}

#include "Argument.hpp"