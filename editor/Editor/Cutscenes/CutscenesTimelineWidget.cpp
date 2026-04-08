#include <pch.h>

#include "CutscenesTimelineWidget.h"

#include <Core/UI/ElementList.h>
#include <Core/UI/WidgetGeometry.h>
#include <Core/UI/Events.h>
#include <Core/UI/Reply.h>
#include <Core/Cutscenes/Cutscenes.h>

#include <Editor/StyleSet/EditorStyleSet.h>

#include <Application/InputTypes.h>

namespace GuGu {

    CutscenesTimelineWidget::CutscenesTimelineWidget()
    {
    }

    CutscenesTimelineWidget::~CutscenesTimelineWidget()
    {
    }

    void CutscenesTimelineWidget::init(const BuilderArguments& arguments)
    {
        m_duration    = arguments.mDuration;
        m_currentTime = arguments.mCurrentTime;
        m_onScrubPositionChanged = arguments.mOnScrubPositionChanged;
        m_onTrackSectionClicked  = arguments.mOnTrackSectionClicked;
        m_visibilityAttribute    = Visibility::Visible;
        m_widgetClipping         = WidgetClipping::ClipToBounds;
    }

    void CutscenesTimelineWidget::setCutscenes(std::shared_ptr<Cutscenes> inCutscenes)
    {
        m_cutscenes = inCutscenes;
        if (m_cutscenes)
        {
            m_duration = m_cutscenes->getDuration();
        }
    }

    void CutscenesTimelineWidget::setCurrentTime(float inTime)
    {
        m_currentTime = inTime;
    }

    float CutscenesTimelineWidget::timeToPixel(float time, float widgetWidth) const
    {
        const float trackWidth = widgetWidth - k_labelWidth;
        if (m_duration <= 0.0f) return k_labelWidth;
        return k_labelWidth + (time / m_duration) * trackWidth;
    }

    float CutscenesTimelineWidget::pixelToTime(float x, float widgetWidth) const
    {
        const float trackWidth = widgetWidth - k_labelWidth;
        if (trackWidth <= 0.0f) return 0.0f;
        float t = (x - k_labelWidth) / trackWidth * m_duration;
        if (t < 0.0f) t = 0.0f;
        if (t > m_duration) t = m_duration;
        return t;
    }

    uint32_t CutscenesTimelineWidget::onGenerateElement(PaintArgs& paintArgs,
        const math::box2& cullingRect, ElementList& elementList,
        const WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
        const math::float2 size = allocatedGeometry.getLocalSize();
        const float W = size.x;

        auto styleSet = EditorStyleSet::getStyleSet();

        // ---- Background -------------------------------------------------------
        math::float4 bgColor = styleSet->getColor("ColorLevel2");
        ElementList::addBoxElement(elementList, allocatedGeometry, bgColor,
            styleSet->getBrush("NoBorder"), layer);

        // ---- Ruler (top band) -------------------------------------------------
        {
            WidgetGeometry rulerGeom = allocatedGeometry.getChildGeometry(
                math::float2(W, k_rulerHeight), math::float2(0.0f, 0.0f));
            math::float4 rulerBg = styleSet->getColor("ColorLevel3");
            ElementList::addBoxElement(elementList, rulerGeom, rulerBg,
                styleSet->getBrush("NoBorder"), layer + 1);

            // Tick marks – one per second
            if (m_duration > 0.0f)
            {
                const int32_t numTicks = (int32_t)std::ceil(m_duration) + 1;
                for (int32_t t = 0; t <= numTicks; ++t)
                {
                    const float px = timeToPixel((float)t, W);
                    if (px < k_labelWidth || px > W) continue;

                    const float tickH = (t % 5 == 0) ? k_rulerHeight * 0.6f : k_rulerHeight * 0.35f;
                    const float tickY = k_rulerHeight - tickH;

                    std::vector<math::float2> tickLine = {
                        math::float2(px, tickY),
                        math::float2(px, k_rulerHeight)
                    };
                    WidgetGeometry lineGeom = allocatedGeometry.getChildGeometry(
                        allocatedGeometry.getLocalSize(), math::float2(0.0f, 0.0f));
                    math::float4 tickColor = styleSet->getColor("SecondaryColorLevel6");
                    ElementList::addLineElement(elementList, lineGeom, tickColor, tickLine, 1.0f, layer + 2);
                }
            }
        }

        // ---- Track lanes ------------------------------------------------------
        if (m_cutscenes)
        {
            const Array<TrackData>& tracks = m_cutscenes->getTrackData();
            for (int32_t ti = 0; ti < (int32_t)tracks.size(); ++ti)
            {
                const float laneY = k_rulerHeight + (float)ti * k_trackHeight;
                const TrackData& track = tracks[ti];

                // Lane background alternation
                math::float4 laneColor = (ti % 2 == 0)
                    ? styleSet->getColor("ColorLevel2")
                    : styleSet->getColor("ColorLevel3");

                WidgetGeometry laneGeom = allocatedGeometry.getChildGeometry(
                    math::float2(W, k_trackHeight), math::float2(0.0f, laneY));
                ElementList::addBoxElement(elementList, laneGeom, laneColor,
                    styleSet->getBrush("NoBorder"), layer + 1);

                // Label column background
                WidgetGeometry labelGeom = allocatedGeometry.getChildGeometry(
                    math::float2(k_labelWidth, k_trackHeight), math::float2(0.0f, laneY));
                math::float4 labelBg = styleSet->getColor("ColorLevel4");
                ElementList::addBoxElement(elementList, labelGeom, labelBg,
                    styleSet->getBrush("NoBorder"), layer + 2);

                // Section bars
                for (int32_t si = 0; si < (int32_t)track.m_sections.size(); ++si)
                {
                    const Section& sec = track.m_sections[si];
                    const float x0 = timeToPixel(sec.m_startTime, W);
                    const float x1 = timeToPixel(sec.m_endTime,   W);
                    const float barH = k_trackHeight - 6.0f;
                    const float barY = laneY + 3.0f;

                    math::float4 barColor = styleSet->getColor("AccentColorLevel1");
                    WidgetGeometry barGeom = allocatedGeometry.getChildGeometry(
                        math::float2(std::max(x1 - x0, 2.0f), barH),
                        math::float2(x0, barY));
                    ElementList::addBoxElement(elementList, barGeom, barColor,
                        styleSet->getBrush("NoBorder"), layer + 3);

                    // Keyframe diamonds
                    for (const CutscenesKeyFrame& kf : sec.m_keyFrames)
                    {
                        const float kx = timeToPixel(kf.m_timestamp, W);
                        const float ky = laneY + k_trackHeight * 0.5f;

                        std::vector<math::float2> diamond = {
                            math::float2(kx,                    ky - k_keyDiamondR),
                            math::float2(kx + k_keyDiamondR,   ky),
                            math::float2(kx,                    ky + k_keyDiamondR),
                            math::float2(kx - k_keyDiamondR,   ky),
                            math::float2(kx,                    ky - k_keyDiamondR),
                        };
                        WidgetGeometry kfGeom = allocatedGeometry.getChildGeometry(
                            allocatedGeometry.getLocalSize(), math::float2(0.0f, 0.0f));
                        math::float4 kfColor = styleSet->getColor("SecondaryColorLevel9");
                        ElementList::addLineElement(elementList, kfGeom, kfColor, diamond, 1.5f, layer + 4);
                    }
                }
            }
        }

        // ---- Playhead ---------------------------------------------------------
        {
            const float headX = timeToPixel(m_currentTime, W);
            std::vector<math::float2> headLine = {
                math::float2(headX, 0.0f),
                math::float2(headX, size.y)
            };
            WidgetGeometry headGeom = allocatedGeometry.getChildGeometry(
                allocatedGeometry.getLocalSize(), math::float2(0.0f, 0.0f));
            math::float4 headColor = math::float4(1.0f, 0.3f, 0.1f, 1.0f);
            ElementList::addLineElement(elementList, headGeom, headColor, headLine, 2.0f, layer + 5);
        }

        return layer + 5;
    }

    math::float2 CutscenesTimelineWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
    {
        int32_t trackCount = 0;
        if (m_cutscenes)
        {
            trackCount = (int32_t)m_cutscenes->getTrackData().size();
        }
        const float h = k_rulerHeight + (float)std::max(trackCount, 1) * k_trackHeight;
        return math::float2(0.0f, h); // width stretches; height is computed
    }

    Reply CutscenesTimelineWidget::OnMouseButtonDown(const WidgetGeometry& myGeometry,
        const PointerEvent& inMouseEvent)
    {
        if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
        {
            m_isDragging = true;
            const float W = myGeometry.getLocalSize().x;
            const math::float2 localPos = myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition);
            const float newTime = pixelToTime(localPos.x, W);
            m_currentTime = newTime;
            if (m_onScrubPositionChanged)
                m_onScrubPositionChanged(newTime);

            // Check track section click
            if (m_cutscenes && localPos.y > k_rulerHeight)
            {
                int32_t ti = (int32_t)((localPos.y - k_rulerHeight) / k_trackHeight);
                const Array<TrackData>& tracks = m_cutscenes->getTrackData();
                if (ti >= 0 && ti < (int32_t)tracks.size())
                {
                    const TrackData& track = tracks[ti];
                    for (int32_t si = 0; si < (int32_t)track.m_sections.size(); ++si)
                    {
                        const Section& sec = track.m_sections[si];
                        const float x0 = timeToPixel(sec.m_startTime, W);
                        const float x1 = timeToPixel(sec.m_endTime,   W);
                        if (localPos.x >= x0 && localPos.x <= x1)
                        {
                            if (m_onTrackSectionClicked)
                                m_onTrackSectionClicked(ti, si);
                            break;
                        }
                    }
                }
            }

            return Reply::Handled().captureMouse(shared_from_this());
        }
        return Reply::Unhandled();
    }

    Reply CutscenesTimelineWidget::OnMouseButtonUp(const WidgetGeometry& myGeometry,
        const PointerEvent& inMouseEvent)
    {
        if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton && m_isDragging)
        {
            m_isDragging = false;
            return Reply::Handled().releaseMouseCapture();
        }
        return Reply::Unhandled();
    }

    Reply CutscenesTimelineWidget::OnMouseMove(const WidgetGeometry& myGeometry,
        const PointerEvent& inMouseEvent)
    {
        if (m_isDragging)
        {
            const float W = myGeometry.getLocalSize().x;
            const math::float2 localPos = myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition);
            const float newTime = pixelToTime(localPos.x, W);
            m_currentTime = newTime;
            if (m_onScrubPositionChanged)
                m_onScrubPositionChanged(newTime);
            return Reply::Handled();
        }
        return Reply::Unhandled();
    }

}
