#include <pch.h>

#include "Guid.h"

#ifdef WIN32
#include <objbase.h>
#endif

namespace GuGu {
	GGuid::GGuid()
	{
	}
	GGuid::GGuid(GuGuUtf8Str str)
	{
		m_guid = str;
	}
	GGuid::~GGuid()
	{

	}
	GGuid GGuid::generateGuid()
	{
#if WIN32
		char buf[64] = { 0 };
		GUID guid;
		if (S_OK == ::CoCreateGuid(&guid))
		{
			sprintf_s(buf, sizeof(buf), "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		}
#endif
	}
	GuGuUtf8Str GGuid::getGuid() const
	{
		return m_guid;
	}
	bool GGuid::operator==(const GGuid& rhs) const
	{
		return m_guid == rhs.m_guid;
	}
}