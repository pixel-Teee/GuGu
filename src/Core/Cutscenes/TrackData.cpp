#include <pch.h>

#include "TrackData.h"

namespace GuGu {

	std::optional<Section> findSection(const TrackData& inTrackData, float inTime)
	{
		std::optional<Section> res;
		bool bFoundSection = false;
		bool foundIndex = -1;
		//lerp
		for (int32_t i = 0; i < inTrackData.m_sections.size(); ++i)
		{
			//find section
			if (inTime >= inTrackData.m_sections[i].m_startTime && inTime <= inTrackData.m_sections[i].m_endTime)
			{
				bFoundSection = true;
				foundIndex = i;
				break;
			}
		}
		if (bFoundSection)
		{
			res = inTrackData.m_sections[foundIndex];
		}
		return res;
	}

	std::optional<std::pair<GuGu::CutscenesKeyFrame, GuGu::CutscenesKeyFrame>> 
	findKeyFrame(const Section& inSection, float inTime)
	{
		std::optional<std::pair<GuGu::CutscenesKeyFrame, GuGu::CutscenesKeyFrame>> keyFrames;

		const Array<CutscenesKeyFrame>& keyFrameDatas = inSection.m_keyFrames;
		if (keyFrameDatas.size() == 1)
		{
			keyFrames = std::make_pair(keyFrameDatas[0], keyFrameDatas[0]);
		}
		else if(keyFrameDatas.size() >= 2)
		{
			CutscenesKeyFrame preKeyFrame;
			CutscenesKeyFrame nextKeyFrame;
			bool bFoundPreKeyFrame = false;
			for (int32_t i = 0; i < keyFrameDatas.size(); ++i)
			{
				if (inTime >= keyFrameDatas[i].m_timestamp)
				{
					preKeyFrame = keyFrameDatas[i];
					bFoundPreKeyFrame = true;
				}
			}
			bool bFoundNextKeyFrame = false;
			for (int32_t i = keyFrameDatas.size() - 1; i >= 0; --i)
			{
				if (inTime <= keyFrameDatas[i].m_timestamp)
				{
					nextKeyFrame = keyFrameDatas[i];
					bFoundNextKeyFrame = true;
				}
			}
			if (bFoundPreKeyFrame && !bFoundNextKeyFrame)
			{
				nextKeyFrame = preKeyFrame;
			}
			else if(!bFoundPreKeyFrame && bFoundNextKeyFrame)
			{
				preKeyFrame = nextKeyFrame;
			}

			keyFrames = std::make_pair(preKeyFrame, nextKeyFrame);
		}

		return keyFrames;
	}

}