#include <pch.h>

#include "TextLayout.h"
#include "ILayoutBlock.h"
#include "ShapedTextCache.h"
#include "WidgetGeometry.h"

namespace GuGu {
	TextLayout::LineModel::LineModel(const std::shared_ptr<GuGuUtf8Str>& inText)
		: text(inText)
		, m_shapedTextCache(ShapedTextCache::Create(FontCache::getFontCache()))
	{
	}
	TextLayout::RunModel::RunModel(const std::shared_ptr<IRun>& inRun)
		: m_run(inRun)
	{
	}

	TextRange TextLayout::RunModel::getTextRange() const
	{
		return m_run->getTextRange();
	}

	int16_t TextLayout::RunModel::getBaseLine(float inScale) const
	{
		return m_run->getBaseLine(inScale);
	}

	int16_t TextLayout::RunModel::getMaxHeight(float inScale) const
	{
		return m_run->getMaxHeight(inScale);
	}

	std::shared_ptr<ILayoutBlock> TextLayout::RunModel::createBlock(const BlockDefinition& blockDefine, float inScale, const LayoutBlockTextContext& inTextContext) const
	{
		const TextRange& sizeRange = blockDefine.m_actualRange;
		if (m_measuredRanges.size() == 0)
		{
			TextRange runRange = m_run->getTextRange();
			return m_run->createBlock(blockDefine.m_actualRange.m_beginIndex, blockDefine.m_actualRange.m_endIndex, m_run->measure(sizeRange.m_beginIndex, sizeRange.m_endIndex, inScale, inTextContext), inTextContext);
		}
	}

	TextLayout::TextLayout()
	{
	}
	TextLayout::~TextLayout()
	{
	}
	void TextLayout::setScale(float inValue)
	{
		m_scale = inValue;
	}
	void TextLayout::setJustification(TextJustify::Type inValue)
	{
		m_justification = inValue;
	}
	void TextLayout::setMargin(const Padding& inMargin)
	{
		m_margin = inMargin;
	}
	void TextLayout::setWrappingWidth(float inValue)
	{
		m_wrappingWidth = inValue;
	}
	math::float2 TextLayout::getSize() const
	{
		return m_textLayoutSize.getDrawSize() * (1.0f / m_scale);
	}
	void TextLayout::addLines(const std::vector<NewLineData>& newLines)
	{
		for (const auto& newLine : newLines)
		{
			LineModel lineModel(newLine.m_text);
			//todo:添加 transform line text
			for (const auto& run : newLine.m_runs)
			{
				lineModel.runs.push_back(RunModel(run));
			}

			m_lineModels.push_back(lineModel);
		}

		const int32_t firstNewLineModelIndex = m_lineModels.size() - newLines.size();

		for (int32_t lineModelIndex = firstNewLineModelIndex; lineModelIndex < m_lineModels.size(); ++lineModelIndex)
		{
			LineModel& lineModel = m_lineModels[lineModelIndex];

			std::vector<std::shared_ptr<ILayoutBlock>> softLine;

			flowLineLayout(lineModelIndex, getWrappingWidth(), softLine);
			//todo:应用margin到新添加的行
		}
	}

	void TextLayout::flowLineLayout(const int32_t lineModelIndex, const float wrappingDrawWidth, std::vector<std::shared_ptr<ILayoutBlock>>& softLine)
	{
		const LineModel& lineModel = m_lineModels[lineModelIndex];

		int32_t currentRunIndex = 0;
		int32_t previousBlockEnd = 0;

		
		//现在暂时没有wrap的功能，迭代所有它的runs
		createLineViewBlocks(lineModelIndex, -1, 0.0f, std::optional<float>(), /*out*/currentRunIndex, /*out*/previousBlockEnd, softLine);
	}

	void TextLayout::createLineViewBlocks(int32_t lineModelIndex, const int32_t stopIndex, const float wrappedLineWidth, const std::optional<float>& justificationWidth, int32_t& OutRunIndex, int32_t& outPreviousBlockEnd, std::vector<std::shared_ptr<ILayoutBlock>>& outSoftLine)
	{
		const LineModel& lineModel = m_lineModels[lineModelIndex];

		const RunTextContext runTextContext(m_textShapingMethod, lineModel.m_shapedTextCache);

		int16_t maxAboveBaseline = 0;
		int16_t maxBelowBaseline = 0;

		//max int32 and min int32
		TextRange softLineRange = TextRange(((int32_t)0x7fffffff), ((int32_t)0x80000000));
		for (; OutRunIndex < lineModel.runs.size(); )
		{
			const RunModel& run = lineModel.runs[OutRunIndex];
			const TextRange runRange = run.getTextRange();

			int32_t blockBeginIndex = std::max(outPreviousBlockEnd, runRange.m_beginIndex);
			int32_t blockStopIndex = runRange.m_endIndex;

			if (stopIndex != -1)
			{
				blockStopIndex = std::min(stopIndex, blockStopIndex);
			}

			const bool isLastBlock = blockStopIndex == stopIndex;

			//add the new block
			{
				BlockDefinition blockDefine;
				blockDefine.m_actualRange = TextRange(blockBeginIndex, blockStopIndex);

				outSoftLine.push_back(run.createBlock(blockDefine, m_scale, LayoutBlockTextContext(runTextContext)));
				outPreviousBlockEnd = blockStopIndex;

				const TextRange& blockRange = outSoftLine.back()->getTextRange();
				softLineRange.m_beginIndex = std::min(softLineRange.m_beginIndex, blockRange.m_beginIndex);
				softLineRange.m_endIndex = std::max(softLineRange.m_endIndex, blockRange.m_endIndex);
			}

			//获取baseline和翻转它的符号，baseline通常都是负的
			const int16_t baseLine = -(run.getBaseLine(m_scale));

			maxAboveBaseline = std::max(maxAboveBaseline, (int16_t)(run.getMaxHeight(m_scale) - baseLine));
			maxBelowBaseline = std::max(maxBelowBaseline, baseLine);

			if (blockStopIndex == runRange.m_endIndex)
			{
				++OutRunIndex;
			}

			if (isLastBlock)
			{
				break;
			}
		}

		math::float2 lineSize(0.0f, 0.0f);
		math::float2 currentOffset(0, m_textLayoutSize.m_height);

		if (outSoftLine.size() > 0)
		{
			float currentHorizontalPos = 0.0f;
			for (int32_t index = 0; index < outSoftLine.size(); ++index)
			{
				const std::shared_ptr<ILayoutBlock> block = outSoftLine[index];
				const std::shared_ptr<IRun> run = block->getRun();
				
				const int16_t blockBaseLine = run->getBaseLine(m_scale);
				const int16_t verticalOffset = maxAboveBaseline - block->getSize().y - blockBaseLine;
				//todo:添加kerning

				block->setLocationOffset(math::float2(currentOffset.x + currentHorizontalPos, currentOffset.y + verticalOffset));

				currentHorizontalPos += block->getSize().x;
			}

			const float unscaleLineHeight = maxAboveBaseline + maxBelowBaseline;

			lineSize.x = currentHorizontalPos;
			lineSize.y = unscaleLineHeight;//todo:添加line height percentage

			TextLayout::LineView lineView;
			lineView.offset = currentOffset;
			lineView.size = lineSize;
			lineView.textHeight = unscaleLineHeight;
			lineView.justificationWidth = justificationWidth.value_or(lineView.size.x);
			lineView.range = softLineRange;
			lineView.modelIndex = lineModelIndex;
			for (size_t i = 0; i < outSoftLine.size(); ++i)
			{
				lineView.blocks.push_back(outSoftLine[i]);
			}

			m_lineViews.push_back(lineView);
		}

		m_textLayoutSize.m_drawWidth = std::max(m_textLayoutSize.m_drawWidth, lineSize.x);
		m_textLayoutSize.m_wrappedWidth = std::max(m_textLayoutSize.m_wrappedWidth, (stopIndex == -1) ? lineSize.x : wrappedLineWidth);
		m_textLayoutSize.m_height += lineSize.y;
	}

}