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
		float m_timestamp;
	};
	struct CutscenesEvents
	{
		GuGuUtf8Str m_functionName;

		Array<Array<uint8_t>> m_arguments;

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
		GuGuUtf8Str m_bindingObjectComponentType;
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