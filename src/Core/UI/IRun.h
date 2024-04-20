#pragma once

#include <unordered_map>

#include <Core/GuGuUtf8Str.h>

#include "FontCache.h"//TextShapingMethod

#include "TextRange.h"

#include "TextLayout.h"

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
	/*当从一个文本的run创建一个block的时候，被需要的上下文*/
	struct LayoutBlockTextContext : public RunTextContext
	{
		LayoutBlockTextContext(const RunTextContext& inRunTextContext)
			: RunTextContext(inRunTextContext)
		{}
	};
	class ILayoutBlock;
	//class TextLayout;
	class TextBlockStyle;
	struct TextArgs
	{
		TextArgs(const TextLayout::LineView& inLine, const std::shared_ptr<ILayoutBlock>& inBlock, const TextBlockStyle& inDefaultStyle)
			: line(inLine)
			, block(inBlock)
			, defaultStyle(inDefaultStyle)
		{}

		const TextLayout::LineView& line;
		const std::shared_ptr<ILayoutBlock>& block;
		const TextBlockStyle& defaultStyle;
	};
	struct PaintArgs;
	class WidgetGeometry;
	class ElementList;
	class IRun
	{
	public:
		virtual ~IRun();

		virtual TextRange getTextRange() const = 0;

		virtual void setTextRange(const TextRange& value) = 0;

		virtual int16_t getMaxHeight(float scale) const = 0;

		virtual int16_t getBaseLine(float inScale) const = 0;

		virtual math::float2 getLocationAt(const std::shared_ptr<ILayoutBlock>& block, int32_t offset, float scale) const = 0;

		virtual std::shared_ptr<ILayoutBlock> createBlock(int32_t startIndex, int32_t endIndex, math::float2 size, const LayoutBlockTextContext& textContext) = 0;

		virtual math::float2 measure(int32_t startIndex, int32_t endIndex, float scale, const RunTextContext& textContext) const = 0;

		virtual int32_t OnPaint(const PaintArgs& paintArgs, const TextArgs& textArgs, const WidgetGeometry& allottedGeometry, const math::box2& cullingRect,
			ElementList& outDrawElements, int32_t layerId, const Style& inWidgetStyle) const = 0;

		virtual void move(const std::shared_ptr<GuGuUtf8Str>& newText, const TextRange& newRange) = 0;

		virtual std::shared_ptr<IRun> clone() = 0;

		virtual void appendTextTo(GuGuUtf8Str& text) const = 0;

		virtual void appendTextTo(GuGuUtf8Str& text, const TextRange& range) const = 0;

		virtual int32_t getTextIndexAt(const std::shared_ptr<ILayoutBlock>& block, const math::float2& location, float scale) const = 0;
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