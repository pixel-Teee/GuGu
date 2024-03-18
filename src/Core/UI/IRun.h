#pragma once

#include <unordered_map>

#include <Core/GuGuUtf8Str.h>

#include "FontCache.h"//TextShapingMethod

#include "TextRange.h"

namespace GuGu {
	struct RunInfo
	{
		RunInfo()
			: name()
			, metaData()
		{}

		RunInfo(GuGuUtf8Str inName)
			: name(inName)
			, metaData()
		{}
		GuGuUtf8Str name;
		std::unordered_map<GuGuUtf8Str, GuGuUtf8Str> metaData;
	};
	/*当塑形一个text的run的时候的基础数据*/
	struct ShapedTextContext
	{
		ShapedTextContext(const TextShapingMethod inTextShapingMethod)
			: m_textShapingMethod(inTextShapingMethod)
		{}

		bool operator==(const ShapedTextContext& other) const
		{
			return m_textShapingMethod == other.m_textShapingMethod;
		}

		TextShapingMethod m_textShapingMethod;
	};
	class ShapedTextCache;
	/*当执行文本操作在文本的一个run的时候的上下文*/
	struct RunTextContext : public ShapedTextContext
	{
		RunTextContext(const TextShapingMethod inTextShapingMethod, std::shared_ptr<ShapedTextCache> inShapedTextCache) :
			ShapedTextContext(inTextShapingMethod)
			, m_shapedTextCache(inShapedTextCache){}

		/*塑形的文本cache被使用于文本的这一行*/
		std::shared_ptr<ShapedTextCache> m_shapedTextCache;
	};
	class IRun
	{
	public:
		virtual ~IRun();

		virtual TextRange getTextRange() const = 0;
	};
}

namespace std
{
	template<>
	struct hash<GuGu::ShapedTextContext>
	{
		size_t operator()(const GuGu::ShapedTextContext& s) const
		{
			size_t keyHash = 0;
			GuGu::hash_combine(keyHash, s.m_textShapingMethod);
			return keyHash;
		}
	};
}