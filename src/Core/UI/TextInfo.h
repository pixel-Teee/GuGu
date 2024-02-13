#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class TextInfo
	{
	public:
		TextInfo();

		TextInfo(GuGuUtf8Str inTextPath, float inSize);

		virtual ~TextInfo();

		GuGuUtf8Str getName() const;

		GuGuUtf8Str getFilePath() const;

		float getSize() const;

		bool operator==(const TextInfo& rhs) const;
	public:
		GuGuUtf8Str m_textPath;
		GuGuUtf8Str m_name;
		float m_size;
	};
}

namespace std
{
	template<>
	struct hash<GuGu::TextInfo>
	{
		size_t operator()(const GuGu::TextInfo& textInfo) const
		{
			size_t val = 0;
			GuGu::GuGuUtf8Str utf8Str = textInfo.getName();
			const char* str = utf8Str.getStr();
			for (size_t i = 0; str[i]; ++i)
			{
				val ^= static_cast<size_t>(str[i]);
			}
			//hash_combine(val, textInfo.getScale());
			val ^= (size_t)textInfo.getSize();//todo:fix this
			return val;
		}
	};
}