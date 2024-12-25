#pragma once

namespace GuGu {
	//一个对象句柄，但是回收是主动来处理
	template<typename T>
	class ObjectPtr
	{
	public:
		ObjectPtr() {}

		ObjectPtr(const ObjectPtr& rhs) : m_object(rhs.m_object) { ++m_referenceCount; }

		virtual ~ObjectPtr() {}

		ObjectPtr<T> operator=(const ObjectPtr& rhs)
		{
			if (m_object != rhs.m_object)
			{
				m_object = rhs.m_object;
				++m_referenceCount;
			}
			return *this;
		}

		//由管理器主动回收
		void clear()
		{
			if (m_referenceCount == 0)
				delete m_object;
		}

	private:
		T* m_object;
		static int32_t m_referenceCount;
	};
}