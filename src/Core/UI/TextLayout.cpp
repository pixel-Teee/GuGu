#include <pch.h>

#include "TextLayout.h"
#include "ILayoutBlock.h"
#include "ShapedTextCache.h"

namespace GuGu {
	LineModel::LineModel(const std::shared_ptr<GuGuUtf8Str>& inText)
		: text(inText)
		, m_shapedTextCache(ShapedTextCache::Create(FontCache::getFontCache()))
	{
	}
	RunModel::RunModel(const std::shared_ptr<IRun>& inRun)
		: m_run(inRun)
	{
	}

	TextRange RunModel::getTextRange() const
	{
		return m_run->getTextRange();
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

		}
	}

}