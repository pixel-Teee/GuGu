#pragma once

namespace GuGu {
	enum class CutscenesInterpolationMode
	{
		Linear
	};
	struct CutscenesKeyFrame
	{
		Array<uint8_t> m_value;
		//type
		GuGuUtf8Str m_typeGuid;

		//field name
		GuGuUtf8Str m_fieldName;
		float m_timestamp;
	};
	//cut scenes event argument
	struct CutscenesEventsArgument
	{
		Array<uint8_t> m_argument;
		
		//type
		GuGuUtf8Str m_typeGuid;
	};
	struct CutscenesEvents
	{
		GuGuUtf8Str m_functionName;

		Array<CutscenesEventsArgument> m_arguments;

		float m_timestamp;
	};
	struct Section
	{
		CutscenesInterpolationMode m_mode;
		float m_startTime;
		float m_endTime;

		//keyframe or events
		Array<CutscenesKeyFrame> m_keyFrames;

		Array<CutscenesEvents> m_events;

		//binding level
		GuGuUtf8Str m_levelPath;

		//binding level
		GuGuUtf8Str m_levelName;

		GuGuUtf8Str m_bindingObjectName;

		//will call component func
		GuGuUtf8Str m_bindingObjectComponentTypeGuid;
	};
	//one track
	struct TrackData
	{
		//由轨道的数据决定类型，事件则是为 meta::Type::Invalid
		GuGuUtf8Str m_typeGuid;
		bool m_isEvent;
		Array<Section> m_sections;
	};

	//helper function
	std::optional<Section> findSection(const TrackData& inTrackData, float inTime);

	std::optional<std::pair<CutscenesKeyFrame, CutscenesKeyFrame>> findKeyFrame(const Section& inSection, float inTime);
}