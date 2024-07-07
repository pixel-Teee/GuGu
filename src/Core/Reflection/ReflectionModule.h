#pragma once

#include "ReflectionDatabase.h"

#define DECLARE_REFLECTION_MODULE(module) \
	namespace meta_generated \
	{						 \
		class Module##module : public GuGu::meta::ReflectionModule \
		{														   \
		public:													   \
			Module##module(GuGu::meta::ReflectionDatabase& db)	   \
			~Module##module(void);								   \
		};														   \
	}															   \

#define UsingModule(module) meta_generated::Module##module _##module(db)

namespace GuGu {
	namespace meta {
		class ReflectionModule
		{
		public:
			ReflectionModule(ReflectionDatabase& db);
		protected:
			ReflectionDatabase& db;
		};
	}
}