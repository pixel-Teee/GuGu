//track data
#include <Core/Reflection/Object.h> //meta object
#include "TrackData.h"

namespace GuGu {
	class Cutscenes : public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		Cutscenes();

		virtual ~Cutscenes();

		virtual void PostLoad() override;

		virtual void Update(float fElapsedTimeSeconds) override;
		virtual meta::Type GetType() const override;
		//拷贝对象
		virtual Object* Clone(void) const override;
		//序列化
		virtual void OnSerialize(nlohmann::json& output) const override;
		//反序列化
		virtual void OnDeserialize(const nlohmann::json& input) override;

		GuGuUtf8Str m_cutScenesName;

		//持续时间
		float m_duration;

		//每秒多少帧
		int32_t m_ticksPerSecond;

		//multiple track data
		//一种类型一种轨道
		Array<TrackData> m_trackDatas;
	};
}