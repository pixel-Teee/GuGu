#include <pch.h>

#include "EditableTextTypes.h"

#include "ElementList.h"

#include "StyleSet.h"

#include <Core/Timer.h>
#include <Application/Application.h>

namespace GuGu {
	#define TWO_PI			(6.28318530717f)
	static float makePulsatingValue(const double inCurrentTime, const float inPulsesPerSecond, const float inPhase = 0.0f)
	{
		return 0.5f + 0.5f * std::sin(((0.25f + inPhase) * TWO_PI) + ((float)inCurrentTime * TWO_PI) * inPulsesPerSecond);
	}
	namespace EditableTextTypes
	{
		void CursorInfo::setCursorLocationAndCalculateAlignment(const TextLayout& inTextLayout, const TextLocation& inCursorPosition)
		{
			TextLocation newCursorPosition = inCursorPosition;
			CursorAlignment newAlignment = CursorAlignment::Left;

			const int32_t cursorLineIndex = inCursorPosition.getLineIndex();
			const int32_t cursorOffset = inCursorPosition.getOffset();

			if (cursorOffset > 0)
			{
				const std::vector<TextLayout::LineModel>& lines = inTextLayout.getLineModels();
				const TextLayout::LineModel& line = lines[cursorLineIndex];

				if (line.text->len() == cursorOffset)
				{
					newCursorPosition = TextLocation(newCursorPosition, -1);
					newAlignment = CursorAlignment::Right;
				}
			}

			m_cursorPosition = newCursorPosition;
			m_cursorAlignment = newAlignment;

			updateLastCursorInteractionTime();//这里更新交互时间
		}

		void CursorInfo::updateLastCursorInteractionTime()
		{
			m_lastCursorInteractionTime = Application::getApplication()->getTimer()->GetTotalTime();//current time
		}

		std::shared_ptr<CursorLineHighlighter> CursorLineHighlighter::Create(const CursorInfo* inCursorInfo)
		{
			return std::make_shared<CursorLineHighlighter>(inCursorInfo);
		}
		CursorLineHighlighter::CursorLineHighlighter(const CursorInfo* inCursorInfo)
			: m_cursorInfo(inCursorInfo)
		{
			m_cursorBrush = StyleSet::getStyle()->getBrush(u8"selectionBackground");
		}
		int32_t CursorLineHighlighter::onPaint(const PaintArgs& args, const TextLayout::LineView& line, const float offsetX, const float inWidth, const TextBlockStyle& defaultStyle, const WidgetGeometry& allottedGeometry, const math::box2& cullingRect, ElementList& outDrawElements, int32_t layerId) const
		{
			math::float2 location(line.offset.x + offsetX, line.offset.y);
			math::float2 size(inWidth, line.textHeight);

			math::float2 cursorWidth = FontCache::getFontCache()->getMaxCharacterHeight(*defaultStyle.m_textInfo, 1.0f) * 0.08f;

			const double currentTime = Application::getApplication()->getTimer()->GetTotalTime();

			//这里文字是预缩放的
			const float inverseScale = 1.0f / allottedGeometry.mAbsoluteScale;

			const double timeSinceLastInteraction = currentTime - m_cursorInfo->getLastCursorInteractionTime();
			const bool bForceCursorVisible = timeSinceLastInteraction < 0.1f;
			float cursorOpacity = (bForceCursorVisible) ? 1.0f : std::roundf(makePulsatingValue(timeSinceLastInteraction, 1.0f));

			math::float2 transformedSize = math::scaling(math::float2(inverseScale)).transformVector(math::float2(cursorWidth.x, size.y));
			math::float2 transformedLocation = math::scaling(math::float2(inverseScale)).transformPoint(location);

			WidgetGeometry paintGeometry = allottedGeometry.getChildGeometry(transformedSize, transformedLocation, allottedGeometry.getAccumulateTransform());

			ElementList::addBoxElement(outDrawElements, paintGeometry, math::float4(1.0f, 1.0f, 1.0f, cursorOpacity * cursorOpacity), m_cursorBrush, layerId);

			return layerId;
		}
	}

}
