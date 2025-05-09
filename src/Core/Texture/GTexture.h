#pragma once

#include <Core/Reflection/Object.h>
#include <Core/Reflection/Type.h>
#include <Renderer/nvrhi.h>
#include <Renderer/TextureCache.h>

namespace GuGu {

	//static mesh
	class GTexture : public meta::Object
	{
	public:
		GTexture();

		virtual ~GTexture();

		virtual void PostLoad() override;

		virtual void Update(float fElapsedTimeSeconds) override;
		virtual meta::Type GetType() const override;
		//拷贝对象
		virtual Object* Clone(void) const override;
		//序列化
		virtual void OnSerialize(nlohmann::json& output) const override;
		//反序列化
		virtual void OnDeserialize(const nlohmann::json& input) override;

		//gpu texture
		nvrhi::TextureHandle m_texture;

		//create gpu texture info（需要去序列化）
		uint32_t m_format;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_arraySize;
		uint32_t m_dimension;
		uint32_t m_mipLevels;
		uint32_t m_bytesPerPixel = 0;
		std::vector<std::vector<TextureSubresourceData>> m_dataLayout;
		//GuGuUtf8Str m_debugName;
		Array<uint8_t> m_data;
	};
}