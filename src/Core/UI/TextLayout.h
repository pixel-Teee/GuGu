#pragma once

#include "BasicElement.h"
#include "TextRange.h"
#include "FontCache.h"//text shaping method
#include "TextLineHightlight.h"

#include <vector>
namespace GuGu {
	class IRun;
	class GuGuUtf8Str;
	namespace TextJustify
	{
		enum Type
		{
			Left,
			Center,
			Right
		};
	}
	//在text model 中的位置
	struct TextLocation
	{
	public:
		TextLocation(const int32_t inLineIndex = 0, const int32_t inOffset = 0)
			: lineIndex(inLineIndex), offset(inOffset)
		{
		}

		TextLocation(const TextLocation& inLocation, const int32_t inOffset)
			: lineIndex(inLocation.getLineIndex())
			, offset(std::max(inLocation.getOffset() + inOffset, 0))
		{}

		bool operator==(const TextLocation& other) const
		{
			return lineIndex == other.lineIndex && offset == other.offset;
		}

		bool operator!=(const TextLocation& other) const
		{
			return lineIndex != other.lineIndex || offset != other.offset;
		}

		int32_t getLineIndex() const { return lineIndex; }
		int32_t getOffset() const { return offset; }
	private:
		int32_t lineIndex;
		int32_t offset;
	};
	class TextSelection
	{
	public:
		TextLocation m_locationA;
		TextLocation m_locationB;
	public:
		TextSelection()
			: m_locationA(-1)
			, m_locationB(-1)
		{}

		TextSelection(const TextLocation& inLocationA, const TextLocation& inLocationB)
			: m_locationA(inLocationA)
			, m_locationB(inLocationB)
		{}

		const TextLocation& getBeginning() const
		{
			if (m_locationA.getLineIndex() == m_locationB.getLineIndex())
			{
				if (m_locationA.getOffset() < m_locationB.getOffset())
					return m_locationA;

				return m_locationB;
			}
			else if (m_locationA.getLineIndex() < m_locationB.getLineIndex())
			{
				return m_locationA;
			}

			return m_locationB;
		}

		const TextLocation& getEnd() const
		{
			if (m_locationA.getLineIndex() == m_locationB.getLineIndex())
			{
				if (m_locationA.getOffset() > m_locationB.getOffset())
					return m_locationA;

				return m_locationB;
			}
			else if (m_locationA.getLineIndex() > m_locationB.getLineIndex())
			{
				return m_locationA;
			}

			return m_locationB;
		}
	};
	struct TextLayoutSize
	{
		TextLayoutSize()
			: m_drawWidth(0.0f)
			, m_wrappedWidth(0.0f)
			, m_height(0.0f)
		{}

		math::float2 getDrawSize() const
		{
			return math::float2(m_drawWidth, m_height);
		}

		math::float2 getWrappedSize() const
		{
			return math::float2(m_wrappedWidth, m_height);
		}

		//text layout 的宽度，包含任何超出换行边界的行，比如包含大量尾置空白的行
		float m_drawWidth;
		//在文本环绕后的文本宽度
		float m_wrappedWidth;
		//text layout 的高度
		float m_height;
	};
	class RunModel;
	class ShapedTextCache;
	class ILayoutBlock;
	//这个RunModel就是IRun，间接调用了一层
	struct LayoutBlockTextContext;

	struct PaintArgs;
	class WidgetGeometry;
	class ElementList;
	class Style;

	class ILineHighlighter;
	class TextLayout
	{
	public:
		struct BlockDefinition
		{
			/*排除尾置空白的范围，被用来通常去展示和交互文本*/
			TextRange m_actualRange;
		};

		class RunModel
		{
		public:
			RunModel(const std::shared_ptr<IRun>& inRun);

			TextRange getTextRange() const;
			void setTextRange(const TextRange& value);

			int16_t getBaseLine(float inScale) const;

			int16_t getMaxHeight(float inScale) const;

			std::shared_ptr<IRun> getRun() const;

			void appendTextTo(GuGuUtf8Str& text) const;

			void appendTextTo(GuGuUtf8Str& text, const TextRange& range) const;

			std::shared_ptr<ILayoutBlock> createBlock(const BlockDefinition& blockDefine, float inScale, const LayoutBlockTextContext& inTextContext) const;
		private:
			std::shared_ptr<IRun> m_run;
			std::vector<TextRange> m_measuredRanges;
			std::vector<math::float2> m_measuredRangeSizes;
		};

		struct LineModel
		{
			LineModel(const std::shared_ptr<GuGuUtf8Str>& inText);

			std::shared_ptr<GuGuUtf8Str> text;
			std::vector<RunModel> runs;
			std::shared_ptr<ShapedTextCache> m_shapedTextCache;
			std::vector<TextLineHighlight> m_lineHighlights;//画光标用
		};
		
		struct LineViewHighlight
		{
			/*x offset对于这个高亮，相对于FLineView::Offset*/
			float m_offsetX;
			/*这个高亮的宽度，这个高度可以是FLineView::Size.Y或者FLineView::TextHeight*/
			float m_width;
			/*自定义高亮实现*/
			std::shared_ptr<ILineHighlighter> m_highLighter;
		};

		struct LineView
		{
			std::vector<std::shared_ptr<ILayoutBlock>> blocks;
			std::vector<LineViewHighlight> m_underlayHighlights;//绘制在文本底下的高亮
			std::vector<LineViewHighlight> m_overlayHighlights;//绘制在文本上面的高亮，比如比光标
			math::float2 offset;
			math::float2 size;
			float textHeight;
			float justificationWidth;
			TextRange range;
			int32_t modelIndex;
		};

		struct TextOffsetLocations
		{
			friend TextLayout;
		private:
			struct OffsetEntry
			{
				OffsetEntry(const int32_t inFlatStringIndex, const int32_t inDocumentLineLength)
					: flagStringIndex(inFlatStringIndex)
					, documentLineLength(inDocumentLineLength)
				{}

				OffsetEntry() {}

				int32_t flagStringIndex;

				int32_t documentLineLength;
			};

			std::vector<OffsetEntry> m_offsetData;
		};
		
		TextLayout();

		virtual ~TextLayout();

		float getScale() const { return m_scale; }
		void setScale(float inValue);

		TextJustify::Type getJustification() const { return m_justification; }
		void setJustification(TextJustify::Type inValue);

		Padding getMargin() const { return m_margin; }
		void setMargin(const Padding& inMargin);

		float getWrappingWidth() const { return m_wrappingWidth; }
		void setWrappingWidth(float inValue);

		math::float2 getSize() const;

		struct NewLineData
		{
			NewLineData(std::shared_ptr<GuGuUtf8Str> inText, std::vector<std::shared_ptr<IRun>> inRuns)
				: m_text(std::move(inText))
				, m_runs(std::move(inRuns))
			{}
			std::shared_ptr<GuGuUtf8Str> m_text;
			std::vector<std::shared_ptr<IRun>> m_runs;
		};

		void addLines(const std::vector<NewLineData>& newLines);

		void clearLines();

		const std::vector<TextLayout::LineModel>& getLineModels() const { return m_lineModels; }
		const std::vector<TextLayout::LineView>& getLineViews() const { return m_lineViews; }

		bool insertAt(const TextLocation& location, GuGuUtf8Str character, bool isCharacter);

		bool insertAt(const TextLocation& location, const GuGuUtf8Str& text);

		bool splitLineAt(const TextLocation& location);

		void getAsText(GuGuUtf8Str& displayText, TextOffsetLocations* const outTextOffsetLocations = nullptr) const;

		TextSelection getGraphemeAt(const TextLocation& location) const;

		void addLineHighlight(const TextLineHighlight& highlight);

		void removeLineHighlight(const TextLineHighlight& highlight);

		void flowHighlights();

		virtual void updateLayout();

		virtual void updateIfNeeded();

		TextLocation getTextLocationAt(const math::float2& relative) const;

		TextLocation getTextLocationAt(const LineView& lineView, const math::float2& relative) const;

		bool removeAt(const TextLocation& location, int32_t count);

		void getSelectionAsText(GuGuUtf8Str& displayText, const TextSelection& selection) const;

		virtual std::shared_ptr<IRun> createDefaultTextRun(const std::shared_ptr<GuGuUtf8Str>& newText, const TextRange& newRange) const = 0;
	protected:

		void flowLineLayout(const int32_t lineModelIndex, const float wrappingDrawWidth, std::vector<std::shared_ptr<ILayoutBlock>>& softLine);

		void getAsTextAndOffsets(GuGuUtf8Str* const OutDisplayText, TextOffsetLocations* const OutTextOffsetLocations) const;

		void createLineViewBlocks(int32_t lineModelIndex, const int32_t stopIndex, const float wrappedLineWidth, const std::optional<float>& justificationWidth,
			int32_t& OutRunIndex, int32_t& outPreviousBlockEnd, std::vector<std::shared_ptr<ILayoutBlock>>& outSoftLine);

		float m_scale;

		TextJustify::Type m_justification;

		Padding m_margin;

		float m_wrappingWidth;
		
		//在屏幕上的布局的最终大小
		TextLayoutSize m_textLayoutSize;

		std::vector<LineModel> m_lineModels;

		TextShapingMethod m_textShapingMethod;

		std::vector<LineView> m_lineViews;
	};
}