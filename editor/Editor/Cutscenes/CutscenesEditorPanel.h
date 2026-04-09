#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/AssetManager/AssetData.h>
#include <Core/Cutscenes/TrackData.h>

#include "CutscenesCommon.h"

namespace GuGu {
    class HorizontalBox;
    class VerticalBox;
    class Border;
    class Button;
    class TextBlockWidget;
    class Slider;
    class CutscenesTimelineWidget;
    class Cutscenes;

    // Panel that displays and edits a Cutscenes asset's keyframe tracks
    class CutscenesEditorPanel : public CompoundWidget
    {
    public:
        CutscenesEditorPanel();
        virtual ~CutscenesEditorPanel();

        struct BuilderArguments : public Arguments<CutscenesEditorPanel>
        {
            BuilderArguments() {}
            ~BuilderArguments() = default;
        };

        void init(const BuilderArguments& arguments);

        // Open a cutscenes asset into the editor
        void setCutscenesAsset(std::shared_ptr<AssetData> inAsset);

        // Playback controls
        Reply onPlayPause();
        Reply onStop();
        Reply onAddKeyframe();

        // Called when the user moves the timeline scrubber
        void onScrubPositionChanged(float inTime);

        float getCurrentTime() const;
        float getDuration() const;

        // Rebuild the track list after data changes
        void rebuildTrackList();

        // Called when a track's section is clicked to select it
        void onTrackSectionClicked(int32_t trackIndex, int32_t sectionIndex);

        virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) override;

    private:
        std::shared_ptr<AssetData>   m_cutscenesAsset;
        std::shared_ptr<Cutscenes>   m_cutscenes;

        bool  m_isPlaying   = false;

        // UI
        std::shared_ptr<CutscenesTimelineWidget> m_timelineWidget;
        std::shared_ptr<VerticalBox>             m_trackListBox;
        std::shared_ptr<Border>                  m_trackListBorder;
        std::shared_ptr<Button>                  m_playPauseButton;
        std::shared_ptr<TextBlockWidget>         m_playPauseLabel;

        // Selection state
        int32_t m_selectedTrack   = -1;
        int32_t m_selectedSection = -1;
    };
}
