#pragma once

#include <unordered_map>

#include <Core/GuGuUtf8Str.h>

#include <Renderer/nvrhi.h>

#include "TextInfo.h"

namespace GuGu {
	class Brush;
	class TextInfo;
	class Style
	{
	public:
		Style();

		virtual ~Style();

		std::shared_ptr<Brush> getBrush(const GuGuUtf8Str& str);

		void getBrush(std::vector<std::shared_ptr<Brush>>& brush);//useful for load texture

		static std::shared_ptr<Style> getStyle();

		std::shared_ptr<TextInfo> getTextInfo(const GuGuUtf8Str& str);
	private:
		//nvrhi::DeviceHandle m_device;

		std::unordered_map<GuGuUtf8Str, std::shared_ptr<Brush>> m_styles;

		//nvrhi::SamplerHandle m_repeatSeampler;
		//nvrhi::SamplerHandle m_defaultSeampler;
		std::unordered_map<GuGuUtf8Str, std::shared_ptr<TextInfo>> m_texts;
	};
}