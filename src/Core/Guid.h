#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class GGuid
	{
	public:
		GGuid();

		GGuid(GuGuUtf8Str str);

		~GGuid();

		static GGuid generateGuid();

		GuGuUtf8Str getGuid() const;

		bool operator==(const GGuid& rhs) const;
	private:
		GuGuUtf8Str m_guid;
	};
}

namespace std
{
	template<>
	struct hash<GuGu::GGuid>
	{
		size_t operator()(const GuGu::GGuid& s) const
		{
			size_t val = 0;
			int32_t totalByteCount = s.getGuid().getTotalByteCount();
			GuGu::GuGuUtf8Str guid = s.getGuid();
			const char* str = guid.getStr();
			for (size_t i = 0; i < totalByteCount; ++i)
			{
				val ^= static_cast<size_t>(str[i]);
			}
			return val;
		}
	};
}