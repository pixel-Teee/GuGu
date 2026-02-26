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
						preKeyValue.x = LittleToHostFloat(preKeyValue.x);
						preKeyValue.y = LittleToHostFloat(preKeyValue.y);
						preKeyValue.z = LittleToHostFloat(preKeyValue.z);
						nextKeyValue.x = LittleToHostFloat(preKeyValue.x);
						nextKeyValue.y = LittleToHostFloat(preKeyValue.y);
						nextKeyValue.z = LittleToHostFloat(preKeyValue.z);
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
						preKeyValue.x = LittleToHostFloat(preKeyValue.x);
						preKeyValue.y = LittleToHostFloat(preKeyValue.y);
						preKeyValue.z = LittleToHostFloat(preKeyValue.z);
						nextKeyValue.x = LittleToHostFloat(nextKeyValue.x);
						nextKeyValue.y = LittleToHostFloat(nextKeyValue.y);
						nextKeyValue.z = LittleToHostFloat(nextKeyValue.z);
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
						preKeyValue.r = LittleToHostFloat(preKeyValue.r);
						preKeyValue.g = LittleToHostFloat(preKeyValue.g);
						preKeyValue.b = LittleToHostFloat(preKeyValue.b);
						preKeyValue.a = LittleToHostFloat(preKeyValue.a);
						nextKeyValue.r = LittleToHostFloat(nextKeyValue.r);
						nextKeyValue.g = LittleToHostFloat(nextKeyValue.g);
						nextKeyValue.a = LittleToHostFloat(nextKeyValue.a);
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