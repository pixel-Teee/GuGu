#include <pch.h>

#include "Priority.h"

namespace GuGu {

	//------priority------
	Priority::Priority()
	{
		m_uiProrityNum = 0;
	}

	Priority::Priority(uint32_t priorityNum)
	{
		m_uiProrityNum = priorityNum;
	}

	Priority::~Priority()
	{
		m_pPriorityThanList.clear();
	}

	bool Priority::addPriorityThan(Priority* pPriority)
	{
		if (!pPriority)
			return false;
		if (pPriority->checkIsCircle(this))
			return false;
		m_uiProrityNum = 0;

		m_pPriorityThanList.push_back(pPriority);//依赖的优先级，当前的优先级大于数组内所有元素的优先级
		return true;
	}

	uint32_t Priority::getPriorityNum()
	{
		checkPriorityNum();
		return m_uiProrityNum;
	}

	bool Priority::operator>(Priority& rhs)
	{
		rhs.checkPriorityNum();
		checkPriorityNum();
		return (m_uiProrityNum > rhs.m_uiProrityNum);
	}

	bool Priority::operator<(Priority& rhs)
	{
		rhs.checkPriorityNum();
		checkPriorityNum();
		return (m_uiProrityNum < rhs.m_uiProrityNum);
	}

	bool Priority::operator<=(Priority& rhs)
	{
		rhs.checkPriorityNum();
		checkPriorityNum();
		return (m_uiProrityNum <= rhs.m_uiProrityNum);
	}

	void Priority::setDebugName(const char* inDebugName)
	{
		m_debugName = inDebugName;
	}

	const char* Priority::getDebugName() const
	{
		return m_debugName;
	}

	bool Priority::checkIsCircle(const Priority* pChecked) const
	{
		if (!pChecked)
			return false;
		for (uint32_t i = 0; i < m_pPriorityThanList.size(); ++i)
		{
			if (pChecked == m_pPriorityThanList[i])
				return true;
			else
				return m_pPriorityThanList[i]->checkIsCircle(pChecked);
		}
		return false;
	}

	uint32_t Priority::checkPriorityNum() //计算优先级
	{
		if (m_uiProrityNum || m_pPriorityThanList.empty())
			return m_uiProrityNum;
		else
		{
			uint32_t uiMax = 0;
			for (uint32_t i = 0; i < m_pPriorityThanList.size(); ++i)
			{
				if (uiMax < m_pPriorityThanList[i]->checkPriorityNum())
					uiMax = m_pPriorityThanList[i]->m_uiProrityNum;
			}
			m_uiProrityNum = uiMax + 1;
			return m_uiProrityNum;
		}
	}

	bool Priority::operator>=(Priority& rhs)
	{
		rhs.checkPriorityNum();
		checkPriorityNum();
		return (m_uiProrityNum >= rhs.m_uiProrityNum);
	}

	bool Priority::operator!=(Priority& rhs)
	{
		rhs.checkPriorityNum();
		checkPriorityNum();
		return (m_uiProrityNum != rhs.m_uiProrityNum);
	}

	bool Priority::operator==(Priority& rhs)
	{
		rhs.checkPriorityNum();
		checkPriorityNum();
		return (m_uiProrityNum == rhs.m_uiProrityNum);//优先级是否一致
	}

	Priority& Priority::operator=(const Priority& rhs)
	{
		const_cast<Priority&>(rhs).checkPriorityNum();
		m_uiProrityNum = rhs.m_uiProrityNum;
		m_pPriorityThanList.clear();
		m_pPriorityThanList = rhs.m_pPriorityThanList;
		return *this;
	}
	//------priority------
}