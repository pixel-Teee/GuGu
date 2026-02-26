#include <pch.h>

#include "TransformEvaluator.h"
#include "Cutscenes.h"
#include <Core/Math/MyMath.h>
#include <Core/FileSystem/ByteOrder.h>

namespace GuGu {

	VectorEvaluator::VectorEvaluator()
	{

	}

	VectorEvaluator::~VectorEvaluator()
	{

	}

	meta::Variant VectorEvaluator::Evaluate(const TrackData& inData, float time)
	{
		//lerp
		if(inData.m_isEvent)
			return meta::Variant();

		GuGuUtf8Str typeGuid = inData.m_typeGuid;

		meta::Type currentType = meta::Type::getType(typeGuid);

		//meta::Variant res;
		if (currentType == typeof(math::float3))
		{
			std::optional<Section> currentSection = findSection(inData, time);
			if (currentSection.has_value())
			{
				const Section& section = currentSection.value();
				if (section.m_mode == CutscenesInterpolationMode::Linear)
				{
					//lerp
					//find two keyframes
					std::optional<std::pair<CutscenesKeyFrame, CutscenesKeyFrame>> twoKeys = findKeyFrame(section, time);
					if (twoKeys.has_value())
					{
						CutscenesKeyFrame preKey = twoKeys.value().first;
						CutscenesKeyFrame nextKey = twoKeys.value().second;
						//lerp
						math::float3 preKeyValue;
						math::float3 nextKeyValue;
						std::memcpy(&preKeyValue, preKey.m_value.data(), sizeof(math::float3));
						std::memcpy(&nextKeyValue, nextKey.m_value.data(), sizeof(math::float3));
						preKeyValue.x = LittleToHostFloat(preKeyValue.x);
						preKeyValue.y = LittleToHostFloat(preKeyValue.y);
						preKeyValue.z = LittleToHostFloat(preKeyValue.z);
						nextKeyValue.x = LittleToHostFloat(preKeyValue.x);
						nextKeyValue.y = LittleToHostFloat(preKeyValue.y);
						nextKeyValue.z = LittleToHostFloat(preKeyValue.z);
						math::float3 resLerpValue = (time - preKey.m_timestamp) / (nextKey.m_timestamp - preKey.m_timestamp);
						return resLerpValue;
					}
				}
			}
		}
		else if (currentType == typeof(math::double3))
		{
			std::optional<Section> currentSection = findSection(inData, time);
			if (currentSection.has_value())
			{
				const Section& section = currentSection.value();
				if (section.m_mode == CutscenesInterpolationMode::Linear)
				{
					//lerp
					//find two keyframes
					std::optional<std::pair<CutscenesKeyFrame, CutscenesKeyFrame>> twoKeys = findKeyFrame(section, time);
					if (twoKeys.has_value())
					{
						CutscenesKeyFrame preKey = twoKeys.value().first;
						CutscenesKeyFrame nextKey = twoKeys.value().second;
						//lerp
						math::double3 preKeyValue;
						math::double3 nextKeyValue;
						std::memcpy(&preKeyValue, preKey.m_value.data(), sizeof(math::double3));
						std::memcpy(&nextKeyValue, nextKey.m_value.data(), sizeof(math::double3));
						preKeyValue.x = LittleToHostFloat(preKeyValue.x);
						preKeyValue.y = LittleToHostFloat(preKeyValue.y);
						preKeyValue.z = LittleToHostFloat(preKeyValue.z);
						nextKeyValue.x = LittleToHostFloat(preKeyValue.x);
						nextKeyValue.y = LittleToHostFloat(preKeyValue.y);
						nextKeyValue.z = LittleToHostFloat(preKeyValue.z);
						math::double3 resLerpValue = (time - preKey.m_timestamp) / (nextKey.m_timestamp - preKey.m_timestamp);
						return resLerpValue;
					}
				}
			}
		}

		return meta::Variant();
	}

}