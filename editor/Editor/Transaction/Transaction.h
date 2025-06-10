#pragma once

namespace GuGu {
	namespace meta {
		class Object;
	}
	struct TrackObject
	{
		bool isRoot = false;
		std::shared_ptr<meta::Object> m_object;

		bool isDelete = false;

		bool operator<(const TrackObject& rhs) const
		{
			return m_object < rhs.m_object;
		}
	};

	struct ModifyState
	{
		//只有 root object 才存这个
		std::map<int32_t, meta::Object*> m_indexToObjects;

		nlohmann::json m_beforeState;
		nlohmann::json m_afterState;
	};

	//单个事务
	struct Transaction
	{
		//每个map包含了一个对象的修改(是一个object tree)
		std::vector<std::map<TrackObject, ModifyState>> m_currentObjects;
	};
}