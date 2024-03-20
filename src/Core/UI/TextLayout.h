#pragma once

#include "BasicElement.h"
#include "TextRange.h"
#include "FontCache.h"//text shaping method

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

			int16_t getBaseLine(float inScale) const;

			int16_t getMaxHeight(float inScale) const;

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
		};

		struct LineView
		{
			std::vector<std::shared_ptr<ILayoutBlock>> blocks;
			math::float2 offset;
			math::float2 size;
			float textHeight;
			float justificationWidth;
			TextRange range;
			int32_t modelIndex;
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

	protected:

		void flowLineLayout(const int32_t lineModelIndex, const float wrappingDrawWidth, std::vector<std::shared_ptr<ILayoutBlock>>& softLine);

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