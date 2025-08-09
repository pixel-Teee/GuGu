#pragma once

#include <Core/Reflection/Object.h>
#include <Core/Reflection/Type.h>
#include <Renderer/nvrhi.h>

namespace GuGu {

	//font file
	class GFont : public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		GFont();

		virtual ~GFont();

		virtual void PostLoad() override;

		virtual void Update(float fElapsedTimeSeconds) override;
		virtual meta::Type GetType() const override;
		//拷贝对象
		virtual Object* Clone(void) const override;
		//序列化
		virtual void OnSerialize(nlohmann::json& output) const override;
		//反序列化
		virtual void OnDeserialize(const nlohmann::json& input) override;

		//font atlas
		Array<uint8_t> m_fontAtlasData;

		uint32_t m_fontAtlasWidth;

		uint32_t m_fontAtlasHeight;

		//gpu texture
		nvrhi::TextureHandle m_texture;

		//font file data
		Array<uint8_t> m_data;
	};
}