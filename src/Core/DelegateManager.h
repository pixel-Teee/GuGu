#pragma once

#include <functional>
#include <map>
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	template<typename ReturnType, typename... Args>
	class DelegateManager
	{
	public:
		GuGuUtf8Str addDelegate(const std::function<ReturnType(Args...)>& func)
		{
			GuGuUtf8Str id = generateId();
			m_delegates[id] = func;
			return id;
		}

		//remove
		bool remove(const GuGuUtf8Str& id)
		{
			return m_delegates.erase(id) > 0;
		}

		//todo:fix this(need to return value)
		void invokeAll(Args... args)
		{
			for (auto& [id, func] : m_delegates)
			{
				if (func)
				{
					func(args...);//todo:add std::forward
				}
			}
		}

		size_t getSize() const
		{
			return m_delegates.size();
		}

		void clear()
		{
			m_delegates.clear();
		}

	private:
		std::map<GuGuUtf8Str, std::function<ReturnType(Args...)>> m_delegates;

		size_t m_counter = 0;

		GuGuUtf8Str generateId()
		{
			return "delegate_" + std::to_string(++m_counter);
		}
	};
}