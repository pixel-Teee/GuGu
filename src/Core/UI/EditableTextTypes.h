#pragma once

#include "TextLayout.h"//TextLocation
#include "ILineHighlighter.h"

namespace GuGu {

	class CursorInfo;
	class TextBlockStyle;
	class Brush;
	namespace EditableTextTypes
	{
		enum class CursorAlignment : uint8_t
		{
			Left,

			Right
		};

		class CursorInfo
		{
		public:
			CursorInfo()
				: m_cursorPosition()
				, m_cursorAlignment(CursorAlignment::Left)
				, m_lastCursorInteractionTime(0)
			{}

			TextLocation getCursorPosition() const
			{
				return m_cursorPosition;
			}

			TextLocation getCursorInteractionLocation() const
			{
				//如果光标在文本最后面，那么它的alignment就是right，要偏移+1，获取实际的位置
				return TextLocation(m_cursorPosition, (m_cursorAlignment == CursorAlignment::Right) ? 1 : 0);
			}

			double getLastCursorInteractionTime() const
			{
				return m_lastCursorInteractionTime;
			}

			//光标在文字左边还是右边
			CursorAlignment getCursorAlignment() const
			{
				return m_cursorAlignment;
			}

			void setCursorLocationAndCalculateAlignment(const TextLayout& inTextLayout, const TextLocation& inCursorPosition);

			void updateLastCursorInteractionTime();
			
		private:
			TextLocation m_cursorPosition;

			CursorAlignment m_cursorAlignment;

			//用户上一次对光标做的任何事情的时间，用于闪烁
			double m_lastCursorInteractionTime;
		};

		//关于滑动到一个给定点的信息
		struct ScrollInfo
		{

			ScrollInfo()
				: m_position()
				, m_alignment(CursorAlignment::Left)
			{}

			ScrollInfo(const TextLocation inPosition, const CursorAlignment inAlignment)
				: m_position(inPosition)
				, m_alignment(inAlignment)
			{}

			TextLocation m_position;

			CursorAlignment m_alignment;
		};
		
		class CursorLineHighlighter : public ILineHighlighter
		{
		public:
			static std::shared_ptr<CursorLineHighlighter> Create(const CursorInfo* inCursorInfo);

			CursorLineHighlighter(const CursorInfo* inCursorInfo);

			virtual int32_t onPaint(const PaintArgs& args, const TextLayout::LineView& line, const float offsetX, const float inWidth, const TextBlockStyle& defaultStyle, const WidgetGeometry& allottedGeometry,
				const math::box2& cullingRect, ElementList& outDrawElements, int32_t layerId) const override;

			const CursorInfo* m_cursorInfo;

			std::shared_ptr<Brush> m_cursorBrush;
		};

		class TextSelectionHighlighter : public ILineHighlighter
		{
		public:
			static std::shared_ptr<TextSelectionHighlighter> Create();

			virtual int32_t onPaint(const PaintArgs& args, const TextLayout::LineView& line, const float offsetX, const float inWidth, const TextBlockStyle& defaultStyle, const WidgetGeometry& allottedGeometry,
				const math::box2& cullingRect, ElementList& outDrawElements, int32_t layerId) const override;

			TextSelectionHighlighter();

			void setHasKeyboardFocus(const bool bInHasKeyboardFocus)
			{
				m_bHasKeyboardFocus = bInHasKeyboardFocus;
			}
		protected:
			bool m_bHasKeyboardFocus;
		};
	}

}