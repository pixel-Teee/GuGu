#pragma once

namespace GuGu {
	namespace meta {
		class Object;
	}
	struct DiffContext
	{
		std::map<int32_t, std::shared_ptr<meta::Object>> deletedObjects;
		std::map<int32_t, std::shared_ptr<meta::Object>> addedObjects;
	};
	//单个事务
	struct Transaction
	{
		//std::vector<uint8_t> m_beforeState;
		//std::vector<uint8_t> m_afterState;
		//m_beforeState[0]，m_beforeState[1]，m_beforeState[2]是一个个meta::Object的状态
		std::vector<std::vector<uint8_t>> m_beforeState;
		std::vector<std::vector<uint8_t>> m_afterState;

		std::vector<std::weak_ptr<meta::Object>> m_currentObjects;

		std::vector<DiffContext> m_trackObjects;
	};
}