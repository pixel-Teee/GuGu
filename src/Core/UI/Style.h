#pragma once

#include <unordered_map>

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class Brush;
	class Style
	{
	public:
		Style();

		virtual ~Style();

		std::shared_ptr<Brush> getBrush(const GuGuUtf8Str& str);
	public:
		std::unordered_map<GuGuUtf8Str, std::shared_ptr<Brush>> m_styles;
	};
}