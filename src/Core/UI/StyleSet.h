#pragma once

#include <unordered_map>

#include <Core/GuGuUtf8Str.h>
#include <Renderer/nvrhi.h>

#include "Style.h"

namespace GuGu {
	class Brush;
	//class TextInfo;
	class StyleSet
	{
	public:
		StyleSet();

		virtual ~StyleSet();

		std::shared_ptr<Brush> getBrush(const GuGuUtf8Str& str);

		void getBrush(std::vector<std::shared_ptr<Brush>>& brush);//useful for load texture

		static std::shared_ptr<StyleSet> getStyle();

		//std::shared_ptr<TextInfo> getTextInfo(const GuGuUtf8Str& str);

		template<typename StyleType>
		std::shared_ptr<StyleType> getStyle(const GuGuUtf8Str& styleName)
		{
			if (m_styles.find(styleName) != m_styles.end())
				return std::static_pointer_cast<StyleType>(m_styles[styleName]);
			return nullptr;
		}

		void clear();
	private:
		//nvrhi::DeviceHandle m_device;

		std::unordered_map<GuGuUtf8Str, std::shared_ptr<Brush>> m_brushes;

		//nvrhi::SamplerHandle m_repeatSeampler;
		//nvrhi::SamplerHandle m_defaultSeampler;
		std::unordered_map<GuGuUtf8Str, std::shared_ptr<Style>> m_styles;
	};
}