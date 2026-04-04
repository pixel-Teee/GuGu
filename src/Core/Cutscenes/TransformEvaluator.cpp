#include <pch.h>

#include "TransformEvaluator.h"
#include "Cutscenes.h"
#include <Core/Math/MyMath.h>
#include <Core/FileSystem/ByteOrder.h>
#include <Renderer/Color.h>

namespace GuGu {

	VectorEvaluator::VectorEvaluator()
	{
		m_currentFieldName = "";
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
						meta::Variant preKeyVar = preKeyValue;
						meta::Variant nextKeyVar = nextKeyValue;
						LittleToHost(typeof(math::float3), preKeyVar);
						LittleToHost(typeof(math::float3), nextKeyVar);
						preKeyValue = preKeyVar.GetValue<math::float3>();
						nextKeyValue = nextKeyVar.GetValue<math::float3>();
						float percentage = (time - preKey.m_timestamp) / (nextKey.m_timestamp - preKey.m_timestamp);
						math::float3 length = nextKeyValue - preKeyValue;
						math::float3 resLerpValue = math::float3(percentage) * length + preKeyValue;
						m_currentFieldName = preKey.m_fieldName;
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
						meta::Variant preKeyVar = preKeyValue;
						meta::Variant nextKeyVar = nextKeyValue;
						LittleToHost(typeof(math::double3), preKeyVar);
						LittleToHost(typeof(math::double3), nextKeyVar);
						preKeyValue = preKeyVar.GetValue<math::double3>();
						nextKeyValue = nextKeyVar.GetValue<math::double3>();
						float percentage = (time - preKey.m_timestamp) / (nextKey.m_timestamp - preKey.m_timestamp);
						math::double3 length = nextKeyValue - preKeyValue;
						math::double3 resLerpValue = math::double3(percentage) * length + preKeyValue;
						m_currentFieldName = preKey.m_fieldName;
						return resLerpValue;
					}
				}
			}
		}
		else if (currentType == typeof(Color))
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
						Color preKeyValue;
						Color nextKeyValue;
						std::memcpy(&preKeyValue, preKey.m_value.data(), sizeof(Color));
						std::memcpy(&nextKeyValue, nextKey.m_value.data(), sizeof(Color));
						meta::Variant preKeyVar = preKeyValue;
						meta::Variant nextKeyVar = nextKeyValue;
						LittleToHost(typeof(Color), preKeyVar);
						LittleToHost(typeof(Color), nextKeyVar);
						preKeyValue = preKeyVar.GetValue<Color>();
						nextKeyValue = nextKeyVar.GetValue<Color>();
						float percentage = (time - preKey.m_timestamp) / (nextKey.m_timestamp - preKey.m_timestamp);
						Color length = nextKeyValue - preKeyValue;
						Color resLerpValue = percentage * length + preKeyValue;
						m_currentFieldName = preKey.m_fieldName;
						return resLerpValue;
					}
				}
			}
		}

		return meta::Variant();
	}

	GuGuUtf8Str VectorEvaluator::getCurrentField()
	{
		return m_currentFieldName;
	}

}