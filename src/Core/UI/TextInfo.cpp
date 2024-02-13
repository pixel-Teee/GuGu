#include <pch.h>

#include "TextInfo.h"

namespace GuGu {
	TextInfo::TextInfo()
	{
	}
	TextInfo::TextInfo(GuGuUtf8Str inTextPath, float inSize)
		: m_textPath(inTextPath)
		, m_size(inSize)
	{
		m_name = inTextPath;//todo:fix
	}
	TextInfo::~TextInfo()
	{
	}
	GuGuUtf8Str TextInfo::getName() const
	{
		return m_name;
	}
	GuGuUtf8Str TextInfo::getFilePath() const
	{
		return m_textPath;
	}
	float TextInfo::getSize() const
	{
		return m_size;
	}
	bool TextInfo::operator==(const TextInfo& rhs) const
	{
		return (m_size == rhs.m_size) && (m_name == rhs.m_name);
	}
}