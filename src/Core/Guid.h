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
			const char* str = s.getGuid().getStr();
			for (size_t i = 0; str[i]; ++i)
			{
				val ^= static_cast<size_t>(str[i]);
			}
			return val;
		}
	};
}