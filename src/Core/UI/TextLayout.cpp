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

	void TextLayout::RunModel::setTextRange(const TextRange& value)
	{
		m_run->setTextRange(value);
	}

	int16_t TextLayout::RunModel::getBaseLine(float inScale) const
	{
		return m_run->getBaseLine(inScale);
	}

	int16_t TextLayout::RunModel::getMaxHeight(float inScale) const
	{
		return m_run->getMaxHeight(inScale);
	}

	void TextLayout::RunModel::appendTextTo(GuGuUtf8Str& text) const
	{
		m_run->appendTextTo(text);
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

	void TextLayout::clearLines()
	{
		m_lineModels.clear();
		m_textLayoutSize = TextLayoutSize();
		m_lineViews.clear();
	}

	bool TextLayout::insertAt(const TextLocation& location, GuGuUtf8Str character)
	{
		const int32_t insertLocation = location.getOffset();//一行的偏移
		const int32_t lineIndex = location.getLineIndex();

		//todo:检查line index 的有效性

		LineModel& lineModel = m_lineModels[lineIndex];

		lineModel.text->insert(insertLocation, character.getStr());

		bool runIsAfterInserLocation = false;
		for (int32_t runIndex = 0; runIndex < lineModel.runs.size(); ++runIndex)
		{
			RunModel& runModel = lineModel.runs[runIndex];
			const TextRange& runRange = runModel.getTextRange();

			const bool bIsLastRun = runIndex == lineModel.runs.size() - 1;
			if (runRange.Contains(insertLocation) || (bIsLastRun && !runIsAfterInserLocation))
			{
				runIsAfterInserLocation = true;
				//todo:修复这里

				runModel.setTextRange(TextRange(runRange.m_beginIndex, runRange.m_endIndex + 1));
			}
			else if (runIsAfterInserLocation)
			{
				TextRange newRange = runRange;
				newRange.offset(1);
				runModel.setTextRange(newRange);
			}
		}
		return true;
	}

	void TextLayout::getAsText(GuGuUtf8Str& displayText, TextOffsetLocations* const outTextOffsetLocations) const
	{
		getAsTextAndOffsets(&displayText, outTextOffsetLocations);
	}

	TextSelection TextLayout::getGraphemeAt(const TextLocation& location) const
	{
		const int32_t lineIndex = location.getLineIndex();
		const int32_t offset = location.getOffset();

		if (!(lineIndex >= 0 && lineIndex < m_lineModels.size()))
			return TextSelection();

		const LineModel& lineModel = m_lineModels[lineIndex];

		//todo:这里以后要使用断词器
		const int32_t previousBreak = (offset < lineModel.text->len()) ? (offset + 1) : lineModel.text->len();
		const int32_t currentBreak = previousBreak - 1;

		return TextSelection(TextLocation(lineIndex, currentBreak), TextLocation(lineIndex, previousBreak));
	}

	void TextLayout::addLineHighlight(const TextLineHighlight& highlight)
	{
		LineModel& lineModel = m_lineModels[highlight.m_lineIdex];

		bool bWasInserted = false;
		for (int32_t index = 0; index < lineModel.m_lineHighlights.size() && !bWasInserted; ++index)
		{
			if (lineModel.m_lineHighlights[index].m_zorder > highlight.m_zorder)
			{
				auto it = lineModel.m_lineHighlights.begin() + (index - 1);
				lineModel.m_lineHighlights.insert(it, highlight);
				bWasInserted = true;
			}
		}

		if (!bWasInserted)
		{
			lineModel.m_lineHighlights.push_back(highlight);
		}
	}

	void TextLayout::removeLineHighlight(const TextLineHighlight& highlight)
	{
		LineModel& lineModel = m_lineModels[highlight.m_lineIdex];

		bool bWasRemoved = false;
		for (int32_t index = 0; index < lineModel.m_lineHighlights.size(); ++index)
		{
			if (lineModel.m_lineHighlights[index] == highlight)
			{
				auto it = lineModel.m_lineHighlights.begin() + index;
				lineModel.m_lineHighlights.erase(it);
				bWasRemoved = true;
				break;
			}
		}
	}

	void TextLayout::flowHighlights()
	{
		for (LineView& lineView : m_lineViews)
		{
			lineView.m_overlayHighlights.clear();

			LineModel& lineModel = m_lineModels[lineView.modelIndex];

			const RunTextContext runTextContext(m_textShapingMethod, lineModel.m_shapedTextCache);

			for (TextLineHighlight& lineHightlight : lineModel.m_lineHighlights)
			{
				if (lineHightlight.m_lineIdex != lineView.modelIndex)
					continue;

				const TextRange lineViewHighlightRange = lineView.range.intersect(lineHightlight.m_range);

				LineViewHighlight lineViewHighlight;
				lineViewHighlight.m_offsetX = 0.0f;
				lineViewHighlight.m_width = 0.0f;
				lineViewHighlight.m_highLighter = lineHightlight.m_highLighter;

				float runningBlockOffset = lineViewHighlight.m_offsetX;

				for (int32_t currentBlockIndex = 0; currentBlockIndex < lineView.blocks.size(); ++currentBlockIndex)
				{
					const std::shared_ptr<ILayoutBlock>& block = lineView.blocks[currentBlockIndex];
					const TextRange& blockTextRange = block->getTextRange();
					const std::shared_ptr<IRun> run = block->getRun();

					const float currentBlockRunningOffset = runningBlockOffset;
					runningBlockOffset += block->getSize().x;

					const TextRange intersectedRange = blockTextRange.intersect(lineViewHighlightRange);

					lineViewHighlight.m_offsetX = currentBlockRunningOffset;

					lineViewHighlight.m_width += run->measure(intersectedRange.m_beginIndex, intersectedRange.m_endIndex, m_scale, runTextContext).x;

					lineViewHighlight.m_offsetX += run->measure(blockTextRange.m_beginIndex, intersectedRange.m_beginIndex, m_scale, runTextContext).x;

					//append line view hight light and reset
					lineView.m_overlayHighlights.push_back(lineViewHighlight);
					lineViewHighlight.m_offsetX = runningBlockOffset;
					lineViewHighlight.m_width = 0.0f;
				}
			}
		}
	}

	void TextLayout::updateLayout()
	{
		//clear view
		m_textLayoutSize = TextLayoutSize();
		m_lineViews.clear();

		std::vector<std::shared_ptr<ILayoutBlock>> softLine;
		for (int32_t lineModelIndex = 0; lineModelIndex < m_lineModels.size(); ++lineModelIndex)
		{
			flowLineLayout(lineModelIndex, m_wrappingWidth, softLine);
		}
	}

	void TextLayout::updateIfNeeded()
	{
		updateLayout();
	}

	TextLocation TextLayout::getTextLocationAt(const math::float2& relative) const
	{
		if (m_lineViews.size() == 0)
			return TextLocation(0, 0);

		int32_t viewIndex;
		for (viewIndex = 0; viewIndex < m_lineViews.size(); ++viewIndex)
		{
			const LineView& lineView = m_lineViews[viewIndex];

			if (lineView.offset.y > relative.y) //view的高度偏移，多个view组成多行，每个view的位置偏移
			{
				viewIndex = std::max(0, viewIndex - 1);
				break;
			}
		}

		if (viewIndex >= m_lineViews.size())
		{
			viewIndex = m_lineViews.size() - 1;
			const LineView& lineView = m_lineViews[viewIndex];
			return getTextLocationAt(lineView, relative);
		}
		else
		{
			const LineView& lineView = m_lineViews[viewIndex];
			if ((lineView.offset.y + lineView.size.y) < relative.y && viewIndex < m_lineViews.size() - 1)
				++viewIndex;
		}

		const LineView& lineView = m_lineViews[viewIndex];
		return getTextLocationAt(lineView, math::float2(relative.x, lineView.offset.y));
	}

	TextLocation TextLayout::getTextLocationAt(const LineView& lineView, const math::float2& relative) const
	{
		for (int32_t blockIndex = 0; blockIndex < lineView.blocks.size(); ++blockIndex)
		{
			const std::shared_ptr<ILayoutBlock>& block = lineView.blocks[blockIndex];
			const int32_t textIndex = block->getRun()->getTextIndexAt(block, math::float2(relative.x, block->getLocationOffset().y), m_scale);

			if(textIndex == -1)
				continue;

			return TextLocation(lineView.modelIndex, textIndex);
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

	void TextLayout::getAsTextAndOffsets(GuGuUtf8Str* const OutDisplayText, TextOffsetLocations* const OutTextOffsetLocations) const
	{
		int32_t displayTextLength = 0;
		if (OutTextOffsetLocations)
		{
			OutTextOffsetLocations->m_offsetData.resize(m_lineModels.size());
		}

		static const int32_t lineTerminatorLength = GuGuUtf8Str("\r\n").len();

		for (int32_t lineModelIndex = 0; lineModelIndex < m_lineModels.size(); ++lineModelIndex)
		{
			const LineModel& lineModel = m_lineModels[lineModelIndex];

			//append line terminator to the end of the previous line
			if (lineModelIndex > 0)
			{
				if (OutDisplayText)
				{
					*OutDisplayText += GuGuUtf8Str("\r\n");
				}
				displayTextLength += lineTerminatorLength;
			}

			int32_t lineLength = 0;
			for (int32_t runIndex = 0; runIndex < lineModel.runs.size(); ++runIndex)
			{
				const RunModel& run = lineModel.runs[runIndex];
				if (OutDisplayText)
				{
					//把run的text追加到OutDisplayText的末尾
					run.appendTextTo(*OutDisplayText);
				}
				lineLength += run.getTextRange().len();
			}

			if (OutTextOffsetLocations)
			{
				OutTextOffsetLocations->m_offsetData.push_back(TextOffsetLocations::OffsetEntry(displayTextLength, lineLength));
			}

			displayTextLength += lineLength;
		}
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