#pragma once

#include <Core/Reflection/Type.h>

namespace GuGu {
	using FunctionType = bool(*)();
	class Priority;
	//struct ReflectionMain::Element;

	class ReflectionMain
	{
	public:
		static void addInitialTypeFunction(FunctionType func, Priority* pPriority);

		static bool initialize();

		static void terminate();
	private:
		struct Element
		{
		public:
			Element();
			~Element();

			FunctionType m_func;
			Priority* m_pPriority;

			bool operator>(const Element& e) const;
			bool operator<(const Element& e) const;
			bool operator==(const Element& e) const;
		};
		static std::vector<Element>* m_pInitialTypeArray;
	};
}