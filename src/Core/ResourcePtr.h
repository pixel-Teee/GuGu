#pragma once

namespace GuGu {
	template<typename T>
	class ResourcePtr
	{
	public:
		ResourcePtr() {}

		virtual ~ResourcePtr() {}

		void reset()
		{
			if (m_referenceCount == 0)
				delete m_object;
		}


	private:
		T* m_object;
		int32_t m_referenceCount;
	};
}