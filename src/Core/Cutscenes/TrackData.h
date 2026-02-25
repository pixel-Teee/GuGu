#pragma once

namespace GuGu {
	enum class CutscenesInterpolationMode
	{
		Linear
	};
	struct CutscenesKeyFrame
	{
		meta::Variant m_value;
		float m_timestamp;
	};
	struct CutscenesEvents
	{
		GuGuUtf8Str m_functionName;

		Array<meta::Variant> m_arguments;
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
	};
	//one track
	struct TrackData
	{
		//由轨道的数据决定类型，事件则是为 meta::Type::Invalid
		meta::Type m_trackType;
		bool m_isEvent;
		Array<Section> m_sections;
	};
}