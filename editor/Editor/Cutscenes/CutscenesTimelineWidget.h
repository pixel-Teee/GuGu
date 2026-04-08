#pragma once

#include <Core/UI/Widget.h>
#include <Core/UI/UIMacros.h>
#include <Core/Cutscenes/TrackData.h>
#include <functional>

namespace GuGu {
    class Cutscenes;

    // A custom widget that renders the timeline ruler, track lanes and keyframe diamonds.
    // It also handles scrubbing via mouse drag.
    class CutscenesTimelineWidget : public Widget
    {
    public:
        CutscenesTimelineWidget();
        virtual ~CutscenesTimelineWidget();

        struct BuilderArguments : public Arguments<CutscenesTimelineWidget>
        {
            BuilderArguments()
                : mDuration(1.0f)
                , mCurrentTime(0.0f)
            {}
            ~BuilderArguments() = default;

            ARGUMENT_VALUE(float, Duration)
            ARGUMENT_VALUE(float, CurrentTime)
            UI_EVENT(std::function<void(float)>, OnScrubPositionChanged)
            // Called when a keyframe diamond on track trackIndex, section sectionIndex is clicked
            UI_EVENT(std::function<void(int32_t, int32_t)>, OnTrackSectionClicked)
        };

        void init(const BuilderArguments& arguments);

        // Set the cutscenes data to display
        void setCutscenes(std::shared_ptr<Cutscenes> inCutscenes);

        void setCurrentTime(float inTime);

        float getCurrentTime() const { return m_currentTime; }

        // Widget overrides
        virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect,
            ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

        virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

        virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;
        virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;
        virtual Reply OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;
        virtual bool  isInteractable() const override { return true; }

    private:
        // Convert a time value to an x pixel offset within the widget
        float timeToPixel(float time, float widgetWidth) const;
        // Convert x pixel offset to time
        float pixelToTime(float x, float widgetWidth) const;

        std::shared_ptr<Cutscenes> m_cutscenes;

        float m_duration    = 1.0f;
        float m_currentTime = 0.0f;

        bool  m_isDragging  = false;

        std::function<void(float)>           m_onScrubPositionChanged;
        std::function<void(int32_t, int32_t)> m_onTrackSectionClicked;

        // Layout constants
        static constexpr float k_rulerHeight  = 24.0f;   // pixels
        static constexpr float k_trackHeight  = 28.0f;   // pixels per track lane
        static constexpr float k_labelWidth   = 140.0f;  // left label column
        static constexpr float k_keyDiamondR  = 5.0f;    // keyframe diamond half-size
    };
}
