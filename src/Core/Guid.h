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