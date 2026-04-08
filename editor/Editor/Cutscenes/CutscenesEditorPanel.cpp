#include <pch.h>

#include "CutscenesEditorPanel.h"
#include "CutscenesTimelineWidget.h"

#include <Core/Cutscenes/Cutscenes.h>
#include <Core/AssetManager/AssetData.h>

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Border.h>
#include <Core/UI/Button.h>
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/Slider.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/Splitter.h>
#include <Core/UI/Box.h>
#include <Core/UI/Reply.h>
#include <Core/UI/Overlay.h>

#include <Editor/StyleSet/EditorStyleSet.h>
#include <Editor/ContentBrowser/ContentBrowserSingleton.h>

namespace GuGu {

    CutscenesEditorPanel::CutscenesEditorPanel()
    {
    }

    CutscenesEditorPanel::~CutscenesEditorPanel()
    {
    }

    void CutscenesEditorPanel::init(const BuilderArguments& arguments)
    {
        m_childWidget = std::make_shared<SingleChildSlot>();
        m_childWidget->m_parentWidget = shared_from_this();

        auto styleSet = EditorStyleSet::getStyleSet();

        // ---- Toolbar (play / stop / add keyframe) ----------------------------
        std::shared_ptr<Button> stopButton;
        std::shared_ptr<Button> addKeyframeButton;

        std::shared_ptr<HorizontalBox> toolbar =
            WIDGET_NEW(HorizontalBox)
            + HorizontalBox::Slot()
            .FixedWidth()
            (
                WIDGET_ASSIGN_NEW(Button, m_playPauseButton)
                .buttonSyle(styleSet->getStyle<ButtonStyle>(u8"normalBlueButton"))
                .Content
                (
                    WIDGET_ASSIGN_NEW(TextBlockWidget, m_playPauseLabel)
                    .text(Attribute<GuGuUtf8Str>::Create(
                        [this]() -> GuGuUtf8Str {
                            return m_isPlaying ? u8"Pause" : u8"Play";
                        }))
                    .textColor(Attribute<math::float4>::Create([=]() {
                        return styleSet->getColor("SecondaryColorLevel9");
                    }))
                )
            )
            + HorizontalBox::Slot()
            .setPadding(Padding(4.0f, 0.0f, 0.0f, 0.0f))
            .FixedWidth()
            (
                WIDGET_ASSIGN_NEW(Button, stopButton)
                .buttonSyle(styleSet->getStyle<ButtonStyle>(u8"normalBlueButton"))
                .Content
                (
                    WIDGET_NEW(TextBlockWidget)
                    .text(u8"Stop")
                    .textColor(Attribute<math::float4>::Create([=]() {
                        return styleSet->getColor("SecondaryColorLevel9");
                    }))
                )
            )
            + HorizontalBox::Slot()
            .setPadding(Padding(4.0f, 0.0f, 0.0f, 0.0f))
            .FixedWidth()
            (
                WIDGET_ASSIGN_NEW(Button, addKeyframeButton)
                .buttonSyle(styleSet->getStyle<ButtonStyle>(u8"normalBlueButton"))
                .Content
                (
                    WIDGET_NEW(TextBlockWidget)
                    .text(u8"Add Keyframe")
                    .textColor(Attribute<math::float4>::Create([=]() {
                        return styleSet->getColor("SecondaryColorLevel9");
                    }))
                )
            );

        // Wire toolbar buttons
        m_playPauseButton->setOnClicked(OnClicked(std::bind(&CutscenesEditorPanel::onPlayPause, this)));
        stopButton->setOnClicked(OnClicked(std::bind(&CutscenesEditorPanel::onStop, this)));
        addKeyframeButton->setOnClicked(OnClicked(std::bind(&CutscenesEditorPanel::onAddKeyframe, this)));

        // ---- Asset picker (select a Cutscenes asset) -------------------------
        AssetPickerConfig pickerConfig;
        pickerConfig.m_initialAssetViewType = AssetViewType::List;
        pickerConfig.m_onAssetSelected = [this](const AssetData& inAsset) {
            auto assetPtr = std::make_shared<AssetData>(inAsset);
            setCutscenesAsset(assetPtr);
        };
        pickerConfig.m_filter.m_classNames.push_back("GuGu::Cutscenes");

        std::shared_ptr<Widget> assetPicker = createAssetPicker(pickerConfig);

        // ---- Timeline area ---------------------------------------------------
        WIDGET_ASSIGN_NEW(CutscenesTimelineWidget, m_timelineWidget)
            .Duration(1.0f)
            .CurrentTime(0.0f)
            .OnScrubPositionChanged(std::bind(&CutscenesEditorPanel::onScrubPositionChanged, this, std::placeholders::_1))
            .OnTrackSectionClicked(std::bind(&CutscenesEditorPanel::onTrackSectionClicked, this,
                std::placeholders::_1, std::placeholders::_2));

        // Wrap timeline in a border with a dark background so it has a fixed minimum height
        std::shared_ptr<Border> timelineBorder =
            WIDGET_NEW(Border)
            .BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
                return styleSet->getColor("ColorLevel1");
            }))
            .Content
            (
                m_timelineWidget
            );

        // ---- Track list (left side label names) built alongside the timeline -
        WIDGET_ASSIGN_NEW(VerticalBox, m_trackListBox);

        // ---- Main layout: top = toolbar, then splitter of asset picker & timeline
        std::shared_ptr<VerticalBox> mainLayout =
            WIDGET_NEW(VerticalBox)
            + VerticalBox::Slot()
            .FixedHeight()
            (
                // Toolbar strip
                WIDGET_NEW(Border)
                .BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
                    return styleSet->getColor("ColorLevel3");
                }))
                .padding(Padding(4.0f, 2.0f, 4.0f, 2.0f))
                .Content(toolbar)
            )
            + VerticalBox::Slot()
            .StretchHeight(1.0f)
            (
                WIDGET_NEW(Splitter)
                + Splitter::Slot()
                .value(0.25f)
                (
                    // Left: asset picker
                    WIDGET_NEW(Border)
                    .BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
                        return styleSet->getColor("ColorLevel3");
                    }))
                    .Content(assetPicker)
                )
                + Splitter::Slot()
                .value(0.75f)
                (
                    // Right: timeline
                    timelineBorder
                )
            );

        m_childWidget->m_childWidget = mainLayout;
        m_childWidget->m_childWidget->setParentWidget(shared_from_this());
        m_visibilityAttribute = Visibility::Visible;
    }

    void CutscenesEditorPanel::setCutscenesAsset(std::shared_ptr<AssetData> inAsset)
    {
        m_cutscenesAsset = inAsset;
        if (inAsset && inAsset->m_loadedResource)
        {
            m_cutscenes = std::static_pointer_cast<Cutscenes>(inAsset->m_loadedResource);
            m_timelineWidget->setCutscenes(m_cutscenes);
            m_currentTime = 0.0f;
            m_timelineWidget->setCurrentTime(0.0f);
            rebuildTrackList();
        }
    }

    Reply CutscenesEditorPanel::onPlayPause()
    {
        m_isPlaying = !m_isPlaying;
        if (m_cutscenes)
        {
            m_cutscenes->setIsRunning(m_isPlaying);
        }
        return Reply::Handled();
    }

    Reply CutscenesEditorPanel::onStop()
    {
        m_isPlaying = false;
        m_currentTime = 0.0f;
        m_timelineWidget->setCurrentTime(0.0f);
        if (m_cutscenes)
        {
            m_cutscenes->setIsRunning(false);
        }
        return Reply::Handled();
    }

    Reply CutscenesEditorPanel::onAddKeyframe()
    {
        // Adding keyframes requires knowing the selected track / section.
        // For now this is a placeholder that users can extend.
        return Reply::Handled();
    }

    void CutscenesEditorPanel::onScrubPositionChanged(float inTime)
    {
        m_currentTime = inTime;
        m_timelineWidget->setCurrentTime(inTime);
    }

    float CutscenesEditorPanel::getCurrentTime() const
    {
        return m_currentTime;
    }

    float CutscenesEditorPanel::getDuration() const
    {
        if (m_cutscenes)
            return m_cutscenes->getDuration();
        return 1.0f;
    }

    void CutscenesEditorPanel::rebuildTrackList()
    {
        if (!m_cutscenes) return;

        m_trackListBox->clearChildren();

        auto styleSet = EditorStyleSet::getStyleSet();
        const Array<TrackData>& tracks = m_cutscenes->getTrackData();

        for (int32_t i = 0; i < (int32_t)tracks.size(); ++i)
        {
            const TrackData& track = tracks[i];

            GuGuUtf8Str label = track.m_isEvent ? u8"[Event] " : u8"[Track] ";
            // Use type guid as a readable label
            label = label + track.m_typeGuid;

            m_trackListBox->addSlot()
            .FixedHeight()
            (
                WIDGET_NEW(Border)
                .BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
                    return (i % 2 == 0)
                        ? styleSet->getColor("ColorLevel2")
                        : styleSet->getColor("ColorLevel3");
                }))
                .Content
                (
                    WIDGET_NEW(TextBlockWidget)
                    .text(label)
                    .textColor(Attribute<math::float4>::Create([=]() {
                        return styleSet->getColor("SecondaryColorLevel9");
                    }))
                )
            );
        }
    }

    void CutscenesEditorPanel::onTrackSectionClicked(int32_t trackIndex, int32_t sectionIndex)
    {
        m_selectedTrack   = trackIndex;
        m_selectedSection = sectionIndex;
    }

}
