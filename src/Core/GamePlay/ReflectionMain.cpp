#include <pch.h>

#include "ReflectionMain.h"
#include "Priority.h"

namespace GuGu {
	std::vector<ReflectionMain::Element>* ReflectionMain::m_pInitialTypeArray;

	void ReflectionMain::addInitialTypeFunction(FunctionType func, Priority* pPriority)
	{
		if (!func)
			return;
		if (!m_pInitialTypeArray)
			m_pInitialTypeArray = new std::vector<Element>();
		Element element;
		element.m_func = func;
		element.m_pPriority = pPriority;
		m_pInitialTypeArray->push_back(element);
	}

	bool ReflectionMain::initialize()
	{
		std::vector<Element>& funcs = *m_pInitialTypeArray;
		std::sort(funcs.begin(), funcs.end(), [&](const Element& lhs, const Element& rhs) {
			return lhs.m_pPriority < rhs.m_pPriority;
		});
		for (uint32_t i = 0; i < m_pInitialTypeArray->size(); ++i)
		{
			Element element = (*m_pInitialTypeArray)[i];
			if (!element.m_func())
				return false;
		}

		m_pInitialTypeArray->clear();
		delete m_pInitialTypeArray;
		return true;
	}

	void ReflectionMain::terminate()
	{
		
	}

	//------Element------
	ReflectionMain::Element::Element()
	{
		m_func = nullptr;
		m_pPriority = nullptr;
	}

	ReflectionMain::Element::~Element()
	{
		m_func = nullptr;
		m_pPriority = nullptr;
	}

	bool ReflectionMain::Element::operator>(const Element& e) const
	{
		static Priority pr1;
		static Priority pr2;

		Priority* p1 = nullptr;
		Priority* p2 = nullptr;
		if (m_pPriority)
			p1 = m_pPriority;
		else
			p1 = &pr1;
		if (e.m_pPriority)
			p2 = e.m_pPriority;
		else
			p2 = &pr2;
		return (*p1) > (*p2);
	}

	bool ReflectionMain::Element::operator<(const Element& e) const
	{
		static Priority pr1;
		static Priority pr2;

		Priority* p1 = nullptr;
		Priority* p2 = nullptr;
		if (m_pPriority)
			p1 = m_pPriority;
		else
			p1 = &pr1;
		if (e.m_pPriority)
			p2 = e.m_pPriority;
		else
			p2 = &pr2;
		return (*p1) < (*p2);
	}

	bool ReflectionMain::Element::operator==(const Element& e) const
	{
		static Priority pr1;
		static Priority pr2;

		Priority* p1 = nullptr;
		Priority* p2 = nullptr;
		if (m_pPriority)
			p1 = m_pPriority;
		else
			p1 = &pr1;
		if (e.m_pPriority)
			p2 = e.m_pPriority;
		else
			p2 = &pr2;
		return (*p1) == (*p2);
	}
	//------Element------
}