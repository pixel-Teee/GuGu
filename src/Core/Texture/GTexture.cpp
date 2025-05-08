#include <pch.h>

#include "GTexture.h"

namespace GuGu {
	GTexture::GTexture()
	{
		m_format = static_cast<uint32_t>(nvrhi::Format::UNKNOWN);
		m_width = 1;
		m_height = 1;
		m_depth = 1;
		m_arraySize = 1;
		m_mipLevels = 1;
		m_dimension = static_cast<uint32_t>(nvrhi::TextureDimension::Unknown);
		m_originalBitsPerPixel = 0;
	}
	GTexture::~GTexture()
	{
	}

	void GTexture::PostLoad()
	{
		this->m_dataLayout.resize(1);
		this->m_dataLayout[0].resize(1);
		this->m_dataLayout[0][0].dataOffset = 0;
		this->m_dataLayout[0][0].rowPitch = static_cast<size_t>(m_width * m_originalBitsPerPixel);
		this->m_dataLayout[0][0].dataSize = static_cast<size_t>(m_width * m_height * m_originalBitsPerPixel);
	}

	void GTexture::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type GTexture::GetType() const
	{
		return typeof(GTexture);
	}

	meta::Object* GTexture::Clone(void) const
	{
		GTexture* texture = new GTexture();
		texture->m_format = m_format;
		texture->m_width = m_width;
		texture->m_height = m_height;
		texture->m_depth = m_depth;
		texture->m_arraySize = m_arraySize;
		texture->m_dimension = m_dimension;
		texture->m_mipLevels = m_mipLevels;
		texture->m_data = m_data;
		texture->m_originalBitsPerPixel = m_originalBitsPerPixel;
		texture->m_dataLayout = m_dataLayout;
		return texture;
	}

	void GTexture::OnSerialize(nlohmann::json& output) const
	{
		output["type"] = typeidof(GTexture);
		output["format"] = m_format;
		output["width"] = m_width;
		output["height"] = m_height;
		output["depth"] = m_depth;
		output["arraySize"] = m_arraySize;
		output["dimension"] = m_dimension;
		output["mipLevels"] = m_mipLevels;
		output["originalBitsPerPixel"] = m_originalBitsPerPixel;
		output["data"] = nlohmann::json::array();
		for (size_t i = 0; i < m_data.size(); ++i)
		{
			output["data"].emplace_back(m_data[i]);
		}
	}

	void GTexture::OnDeserialize(const nlohmann::json& input)
	{
		m_format = input["format"];
		m_width = input["width"];
		m_height = input["height"];
		m_depth = input["depth"];
		m_arraySize = input["arraySize"];
		m_dimension = input["dimension"];
		m_mipLevels = input["mipLevels"];
		m_originalBitsPerPixel = input["originalBitsPerPixel"];
		nlohmann::json textureData = input["data"];
		m_data.resize(textureData.size());
		for (int32_t i = 0; i < textureData.size(); ++i)
		{
			m_data.push_back(textureData[i]);
		}
	}

}