#include <pch.h>

#include "TextureCache.h"

#include <Renderer/CommonRenderPasses.h>

#include <Core/GuGuUtf8Str.h>

#include <Renderer/stb_image.h>

#include <Core/FileSystem/FileSystem.h>

#include <Core/Texture/GTexture.h>

namespace GuGu{

    TextureCache::TextureCache(nvrhi::IDevice *device, std::shared_ptr<FileSystem> fileSysem)
    : m_Device(device)
    , m_fileSystem(fileSysem){

    }

	TextureCache::TextureCache()
	{
        //nothing
	}

	TextureCache::~TextureCache() {
        //todo:add reset
    }


    std::shared_ptr<LoadedTexture> TextureCache::LoadTextureFromFile(const GuGuUtf8Str& path, bool sRGB,
                                      CommonRenderPasses *passes,
                                      nvrhi::ICommandList *commandList) {
        std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

        //todo:find texture cache

        texture->forceSRGB = sRGB;
        texture->path = path;

        auto fileData = ReadFile(path, std::static_pointer_cast<RootFileSystem>(m_fileSystem));

        if(!fileData.empty())
        {
            if(FillTextureData(fileData, texture, path, ""))
            {
                FinalizeTexture(texture, passes, commandList);
            }
        }

        return texture;
    }

    bool TextureCache::FillTextureData(const std::vector<uint8_t> &fileData,
                                       const std::shared_ptr<TextureData> &texture,
                                       const GuGuUtf8Str &extension,
                                       const GuGuUtf8Str &mimeType) const {
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

        texture->originalBitsPerPixel = static_cast<uint32_t>(originalChannels) * (is_hdr ? 32 : 8);
        texture->width = static_cast<uint32_t>(width);
        texture->height = static_cast<uint32_t>(height);
        texture->isRenderTarget = true;
        texture->mipLevels = 1;
        texture->dimension = nvrhi::TextureDimension::Texture2D;

        texture->dataLayout.resize(1);
        texture->dataLayout[0].resize(1);
        texture->dataLayout[0][0].dataOffset = 0;
        texture->dataLayout[0][0].rowPitch = static_cast<size_t>(width * bytesPerPixel);
        texture->dataLayout[0][0].dataSize = static_cast<size_t>(width * height * bytesPerPixel);

        texture->data.resize(width * height * bytesPerPixel);
        memcpy(texture->data.data(), bitmap, width * height * bytesPerPixel);
        //bitmap = nullptr; // ownership transferred to the blob
        stbi_image_free(bitmap);
        switch (channels)
        {
            case 1:
                texture->format = is_hdr ? nvrhi::Format::R32_FLOAT : nvrhi::Format::R8_UNORM;
                break;
            case 2:
                texture->format = is_hdr ? nvrhi::Format::RG32_FLOAT : nvrhi::Format::RG8_UNORM;
                break;
            case 4:
                texture->format = is_hdr ? nvrhi::Format::RGBA32_FLOAT : (texture->forceSRGB ? nvrhi::Format::SRGBA8_UNORM : nvrhi::Format::RGBA8_UNORM);
                break;
            default:
                texture->data.clear(); // release the bitmap data

                //log::message(m_ErrorLogSeverity, "Unsupported number of components (%d) for texture '%s'", channels, texture->path.c_str());
                return false;
        }

        return true;
    }

    uint32_t GetMipLevelsNum(uint32_t width, uint32_t height)
    {
        uint32_t size = std::min(width, height);
        uint32_t levelsNum = (uint32_t)(logf((float)size) / logf(2.0f)) + 1;

        return levelsNum;
    }

    void
    TextureCache::FinalizeTexture(std::shared_ptr <TextureData> texture, CommonRenderPasses *passes,
                                  nvrhi::ICommandList *commandList) {
        //assert(texture->data);
        //assert(commandList);

        uint32_t originalWidth = texture->width;
        uint32_t originalHeight = texture->height;
//
        bool isBlockCompressed =
                (texture->format == nvrhi::Format::BC1_UNORM) ||
                (texture->format == nvrhi::Format::BC1_UNORM_SRGB) ||
                (texture->format == nvrhi::Format::BC2_UNORM) ||
                (texture->format == nvrhi::Format::BC2_UNORM_SRGB) ||
                (texture->format == nvrhi::Format::BC3_UNORM) ||
                (texture->format == nvrhi::Format::BC3_UNORM_SRGB) ||
                (texture->format == nvrhi::Format::BC4_SNORM) ||
                (texture->format == nvrhi::Format::BC4_UNORM) ||
                (texture->format == nvrhi::Format::BC5_SNORM) ||
                (texture->format == nvrhi::Format::BC5_UNORM) ||
                (texture->format == nvrhi::Format::BC6H_SFLOAT) ||
                (texture->format == nvrhi::Format::BC6H_UFLOAT) ||
                (texture->format == nvrhi::Format::BC7_UNORM) ||
                (texture->format == nvrhi::Format::BC7_UNORM_SRGB);
//
        if (isBlockCompressed)
        {
            originalWidth = (originalWidth + 3) & ~3;
            originalHeight = (originalHeight + 3) & ~3;
        }
//
        uint32_t scaledWidth = originalWidth;
        uint32_t scaledHeight = originalHeight;
//
        if (m_MaxTextureSize > 0 && int(std::max(originalWidth, originalHeight)) > m_MaxTextureSize && texture->isRenderTarget && texture->dimension == nvrhi::TextureDimension::Texture2D)
        {
            if (originalWidth >= originalHeight)
            {
                scaledHeight = originalHeight * m_MaxTextureSize / originalWidth;
                scaledWidth = m_MaxTextureSize;
            }
            else
            {
                scaledWidth = originalWidth * m_MaxTextureSize / originalHeight;
                scaledHeight = m_MaxTextureSize;
            }
        }
//
        const char* dataPointer = reinterpret_cast<const char*>(static_cast<const uint8_t*>(texture->data.data())); //todo:fix this
//
        nvrhi::TextureDesc textureDesc;
        textureDesc.format = texture->format;
        textureDesc.width = scaledWidth;
        textureDesc.height = scaledHeight;
        textureDesc.depth = texture->depth;
        textureDesc.arraySize = texture->arraySize;
        textureDesc.dimension = texture->dimension;
        textureDesc.mipLevels = m_GenerateMipmaps && texture->isRenderTarget && passes
                                ? GetMipLevelsNum(textureDesc.width, textureDesc.height)
                                : texture->mipLevels;
        textureDesc.debugName = texture->path;
        textureDesc.isRenderTarget = texture->isRenderTarget;
        texture->texture = m_Device->createTexture(textureDesc);
//
        commandList->beginTrackingTextureState(texture->texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
//
        //if (m_DescriptorTable)
        //    texture->bindlessDescriptor = m_DescriptorTable->CreateDescriptorHandle(nvrhi::BindingSetItem::Texture_SRV(0, texture->texture));
//
        //if (scaledWidth != originalWidth || scaledHeight != originalHeight)
        //{
        //    nvrhi::TextureDesc tempTextureDesc;
        //    tempTextureDesc.format = texture->format;
        //    tempTextureDesc.width = originalWidth;
        //    tempTextureDesc.height = originalHeight;
        //    tempTextureDesc.depth = textureDesc.depth;
        //    tempTextureDesc.arraySize = textureDesc.arraySize;
        //    tempTextureDesc.mipLevels = 1;
        //    tempTextureDesc.dimension = textureDesc.dimension;
//
        //    nvrhi::TextureHandle tempTexture = m_Device->createTexture(tempTextureDesc);
        //    assert(tempTexture);
        //    commandList->beginTrackingTextureState(tempTexture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
//
        //    for (uint32_t arraySlice = 0; arraySlice < texture->arraySize; arraySlice++)
        //    {
        //        const TextureSubresourceData& layout = texture->dataLayout[arraySlice][0];
//
        //        commandList->writeTexture(tempTexture, arraySlice, 0, dataPointer + layout.dataOffset, layout.rowPitch, layout.depthPitch);
        //    }
//
        //    nvrhi::FramebufferHandle framebuffer = m_Device->createFramebuffer(nvrhi::FramebufferDesc()
        //                                                                               .addColorAttachment(texture->texture));
//
        //    passes->BlitTexture(commandList, framebuffer, tempTexture);
        //}
        //else
        {
            for (uint32_t arraySlice = 0; arraySlice < texture->arraySize; arraySlice++)
            {
                for (uint32_t mipLevel = 0; mipLevel < texture->mipLevels; mipLevel++)
                {
                    const TextureSubresourceData& layout = texture->dataLayout[arraySlice][mipLevel];
//
                    commandList->writeTexture(texture->texture, arraySlice, mipLevel, dataPointer + layout.dataOffset, layout.rowPitch, layout.depthPitch);
                }
            }
        }
//
        texture->data.clear();
//
        //uint width = scaledWidth;
        //uint height = scaledHeight;
        //for (uint mipLevel = texture->mipLevels; mipLevel < textureDesc.mipLevels; mipLevel++)
        //{
        //    width /= 2;
        //    height /= 2;
//
        //    nvrhi::FramebufferHandle framebuffer = m_Device->createFramebuffer(nvrhi::FramebufferDesc()
        //                                                                               .addColorAttachment(nvrhi::FramebufferAttachment()
        //                                                                                                           .setTexture(texture->texture)
        //                                                                                                           .setArraySlice(0)
        //                                                                                                           .setMipLevel(mipLevel)));
//
        //    BlitParameters blitParams;
        //    blitParams.sourceTexture = texture->texture;
        //    blitParams.sourceMip = mipLevel - 1;
        //    blitParams.targetFramebuffer = framebuffer;
        //    passes->BlitTexture(commandList, blitParams);
        //}
//
        commandList->setPermanentTextureState(texture->texture, nvrhi::ResourceStates::ShaderResource);
        commandList->commitBarriers();
//
        //++m_TexturesFinalized;
    }

	void TextureCache::FinalizeTexture(std::shared_ptr<GTexture> gTexture, CommonRenderPasses* passes, nvrhi::ICommandList* commandList)
	{
		//assert(texture->data);
 //assert(commandList);

		uint32_t originalWidth = gTexture->m_width;
		uint32_t originalHeight = gTexture->m_height;
		//
		bool isBlockCompressed =
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC1_UNORM) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC1_UNORM_SRGB) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC2_UNORM) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC2_UNORM_SRGB) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC3_UNORM) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC3_UNORM_SRGB) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC4_SNORM) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC4_UNORM) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC5_SNORM) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC5_UNORM) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC6H_SFLOAT) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC6H_UFLOAT) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC7_UNORM) ||
			(static_cast<nvrhi::Format>(gTexture->m_format) == nvrhi::Format::BC7_UNORM_SRGB);
		//
		if (isBlockCompressed)
		{
			originalWidth = (originalWidth + 3) & ~3;
			originalHeight = (originalHeight + 3) & ~3;
		}
		//
		uint32_t scaledWidth = originalWidth;
		uint32_t scaledHeight = originalHeight;
		//
        bool isRenderTarget = false;
		if (m_MaxTextureSize > 0 && int(std::max(originalWidth, originalHeight)) > m_MaxTextureSize && isRenderTarget && static_cast<nvrhi::TextureDimension>(gTexture->m_dimension) == nvrhi::TextureDimension::Texture2D)
		{
			if (originalWidth >= originalHeight)
			{
				scaledHeight = originalHeight * m_MaxTextureSize / originalWidth;
				scaledWidth = m_MaxTextureSize;
			}
			else
			{
				scaledWidth = originalWidth * m_MaxTextureSize / originalHeight;
				scaledHeight = m_MaxTextureSize;
			}
		}
		//
		const char* dataPointer = reinterpret_cast<const char*>(static_cast<const uint8_t*>(gTexture->m_data.data())); //todo:fix this
		//
		nvrhi::TextureDesc textureDesc;
		textureDesc.format = static_cast<nvrhi::Format>(gTexture->m_format);
		textureDesc.width = scaledWidth;
		textureDesc.height = scaledHeight;
		textureDesc.depth = gTexture->m_depth;
		textureDesc.arraySize = gTexture->m_arraySize;
		textureDesc.dimension = static_cast<nvrhi::TextureDimension>(gTexture->m_dimension);
		textureDesc.mipLevels = m_GenerateMipmaps && isRenderTarget && passes
			? GetMipLevelsNum(textureDesc.width, textureDesc.height)
			: gTexture->m_mipLevels;
		textureDesc.debugName = "";
		textureDesc.isRenderTarget = isRenderTarget;
        gTexture->m_texture = m_Device->createTexture(textureDesc);
		//
		commandList->beginTrackingTextureState(gTexture->m_texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
		//
				//if (m_DescriptorTable)
				//    texture->bindlessDescriptor = m_DescriptorTable->CreateDescriptorHandle(nvrhi::BindingSetItem::Texture_SRV(0, texture->texture));
		//
				//if (scaledWidth != originalWidth || scaledHeight != originalHeight)
				//{
				//    nvrhi::TextureDesc tempTextureDesc;
				//    tempTextureDesc.format = texture->format;
				//    tempTextureDesc.width = originalWidth;
				//    tempTextureDesc.height = originalHeight;
				//    tempTextureDesc.depth = textureDesc.depth;
				//    tempTextureDesc.arraySize = textureDesc.arraySize;
				//    tempTextureDesc.mipLevels = 1;
				//    tempTextureDesc.dimension = textureDesc.dimension;
		//
				//    nvrhi::TextureHandle tempTexture = m_Device->createTexture(tempTextureDesc);
				//    assert(tempTexture);
				//    commandList->beginTrackingTextureState(tempTexture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
		//
				//    for (uint32_t arraySlice = 0; arraySlice < texture->arraySize; arraySlice++)
				//    {
				//        const TextureSubresourceData& layout = texture->dataLayout[arraySlice][0];
		//
				//        commandList->writeTexture(tempTexture, arraySlice, 0, dataPointer + layout.dataOffset, layout.rowPitch, layout.depthPitch);
				//    }
		//
				//    nvrhi::FramebufferHandle framebuffer = m_Device->createFramebuffer(nvrhi::FramebufferDesc()
				//                                                                               .addColorAttachment(texture->texture));
		//
				//    passes->BlitTexture(commandList, framebuffer, tempTexture);
				//}
				//else
		{
			for (uint32_t arraySlice = 0; arraySlice < gTexture->m_arraySize; arraySlice++)
			{
				for (uint32_t mipLevel = 0; mipLevel < gTexture->m_mipLevels; mipLevel++)
				{
					const TextureSubresourceData& layout = gTexture->m_dataLayout[arraySlice][mipLevel];
					//
					commandList->writeTexture(gTexture->m_texture, arraySlice, mipLevel, dataPointer + layout.dataOffset, layout.rowPitch, layout.depthPitch);
				}
			}
		}
		//
        // gTexture->m_data.clear();
		//
				//uint width = scaledWidth;
				//uint height = scaledHeight;
				//for (uint mipLevel = texture->mipLevels; mipLevel < textureDesc.mipLevels; mipLevel++)
				//{
				//    width /= 2;
				//    height /= 2;
		//
				//    nvrhi::FramebufferHandle framebuffer = m_Device->createFramebuffer(nvrhi::FramebufferDesc()
				//                                                                               .addColorAttachment(nvrhi::FramebufferAttachment()
				//                                                                                                           .setTexture(texture->texture)
				//                                                                                                           .setArraySlice(0)
				//                                                                                                           .setMipLevel(mipLevel)));
		//
				//    BlitParameters blitParams;
				//    blitParams.sourceTexture = texture->texture;
				//    blitParams.sourceMip = mipLevel - 1;
				//    blitParams.targetFramebuffer = framebuffer;
				//    passes->BlitTexture(commandList, blitParams);
				//}
		//
		commandList->setPermanentTextureState(gTexture->m_texture, nvrhi::ResourceStates::ShaderResource);
		commandList->commitBarriers();
		//
		//++m_TexturesFinalized;
	}

    nvrhi::TextureHandle TextureCache::FinalizeCubeMapTexture(std::vector<std::shared_ptr<GTexture>> skyBoxTextures, CommonRenderPasses* passes, nvrhi::ICommandList* commandList)
	{
        return nvrhi::TextureHandle();
	}

}