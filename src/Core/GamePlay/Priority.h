#pragma once

#include <vector>

namespace GuGu {

	class Priority
	{
	public:
		Priority();
		Priority(uint32_t priorityNum);
		~Priority();
		bool addPriorityThan(Priority* pPriority);
		uint32_t getPriorityNum();

		Priority& operator=(Priority& rhs);
		bool operator==(Priority& rhs);
		bool operator!=(Priority& rhs);

		bool operator>(Priority& rhs);
		bool operator<(Priority& rhs);

		bool operator>=(Priority& rhs);
		bool operator<=(Priority& rhs);
	private:
		bool checkIsCircle(const Priority* pChecked) const;
		uint32_t checkPriorityNum();

		uint32_t m_uiProrityNum;//这个是优先级别
		std::vector<Priority*> m_pPriorityThanList;
	};
}