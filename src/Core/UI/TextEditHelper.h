#pragma once

namespace GuGu {
	class TextInfo;
	class TextEditHelper
	{
	public:

		static float getFontHeight(const TextInfo& fontInfo);

		static float calculateCaretWidth(const float fontMaxCharHeight);
	};
}