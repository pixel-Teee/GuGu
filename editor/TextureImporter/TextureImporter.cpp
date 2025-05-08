#include <pch.h>

#include "TextureImporter.h"
#include <Renderer/stb_image.h>
#include <Core/Texture/GTexture.h>
#include <Core/AssetManager/AssetManager.h>

#include <fstream> //todo:fix this

namespace GuGu {

	TextureImporter::TextureImporter()
	{

	}

	TextureImporter::~TextureImporter()
	{

	}

	nlohmann::json TextureImporter::loadTexture(const GuGuUtf8Str& texturePhysicalFilePath)
	{
		m_currentLoadTexture = std::make_shared<GTexture>();
		//load texture
		//std::vector<uint8_t> fileContent;
		std::ifstream in(texturePhysicalFilePath.getStr(), std::ios::binary); //todo:fix this
		
		std::vector<uint8_t> fileData;
		in.seekg(0, std::ios::end);
		fileData.reserve(in.tellg());
		in.seekg(0, std::ios::beg);
		char ch;
		while (in.get(ch))
		{
			fileData.push_back(ch);
		}

		//resolve texture data
		int width = 0, height = 0, originalChannels = 0, channels = 0;
		if (!stbi_info_from_memory(
			static_cast<const stbi_uc*>(fileData.data()),
			static_cast<int>(fileData.size()),
			&width, &height, &originalChannels))
		{
			//log::message(m_ErrorLogSeverity, "Couldn't process image header for texture '%s'", texture->path.c_str());
			
			return false;
		}
		bool is_hdr = stbi_is_hdr_from_memory(
			static_cast<const stbi_uc*>(fileData.data()),
			static_cast<int>(fileData.size()));

		if (originalChannels == 3)
		{
			channels = 4;
		}
		else {
			channels = originalChannels;
		}

		unsigned char* bitmap;
		int bytesPerPixel = channels * (is_hdr ? 4 : 1);

		if (is_hdr)
		{
			float* floatmap = stbi_loadf_from_memory(
				static_cast<const stbi_uc*>(fileData.data()),
				static_cast<int>(fileData.size()),
				&width, &height, &originalChannels, channels);

			bitmap = reinterpret_cast<unsigned char*>(floatmap);
		}
		else
		{
			bitmap = stbi_load_from_memory(
				static_cast<const stbi_uc*>(fileData.data()),
				static_cast<int>(fileData.size()),
				&width, &height, &originalChannels, channels);
		}

		if (!bitmap)
		{
			//log::message(m_ErrorLogSeverity, "Couldn't load generic texture '%s'", texture->path.c_str());
			return false;
		}
		m_currentLoadTexture->m_width = static_cast<uint32_t>(width);
		m_currentLoadTexture->m_height = static_cast<uint32_t>(height);
		m_currentLoadTexture->m_mipLevels = 1;
		m_currentLoadTexture->m_originalBitsPerPixel = static_cast<uint32_t>(originalChannels) * (is_hdr ? 32 : 8);
		m_currentLoadTexture->m_dimension = static_cast<uint32_t>(nvrhi::TextureDimension::Texture2D);
		m_currentLoadTexture->m_data.resize(width * height * bytesPerPixel);
		memcpy(m_currentLoadTexture->m_data.data(), bitmap, width * height * bytesPerPixel);
		stbi_image_free(bitmap);

		bool forceSRGB = false;//todo:暂时写死
		switch (channels)
		{
		case 1:
			m_currentLoadTexture->m_format = is_hdr ? static_cast<uint32_t>(nvrhi::Format::R32_FLOAT) : static_cast<uint32_t>(nvrhi::Format::R8_UNORM);
			break;
		case 2:
			m_currentLoadTexture->m_format = is_hdr ? static_cast<uint32_t>(nvrhi::Format::RG32_FLOAT) : static_cast<uint32_t>(nvrhi::Format::RG8_UNORM);
			break;
		case 4:
			m_currentLoadTexture->m_format = is_hdr ? static_cast<uint32_t>(nvrhi::Format::RGBA32_FLOAT) : (forceSRGB ? static_cast<uint32_t>(nvrhi::Format::SRGBA8_UNORM) : static_cast<uint32_t>(nvrhi::Format::RGBA8_UNORM));
			break;
		default:
			m_currentLoadTexture->m_data.clear(); // release the bitmap data

			//log::message(m_ErrorLogSeverity, "Unsupported number of components (%d) for texture '%s'", channels, texture->path.c_str());
			return false;
		}

		nlohmann::json staticMeshJson = AssetManager::getAssetManager().serializeJson(m_currentLoadTexture);
		return staticMeshJson;
	}

}