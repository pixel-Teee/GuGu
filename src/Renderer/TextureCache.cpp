#include <pch.h>

#include "TextureCache.h"

#include <Renderer/CommonRenderPasses.h>

#include <Core/GuGuUtf8Str.h>

#include <Core/Platform/Android/AndroidGuGuFile.h>//todo:remove this

#include <Renderer/stb_image.h>

namespace GuGu{

    TextureCache::TextureCache(nvrhi::IDevice *device)
    : m_Device(device){

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

        auto fileData = ReadTextureFile(path);

        if(!fileData.empty())
        {
            if(FillTextureData(fileData, texture, path, ""))
            {
                FinalizeTexture(texture, passes, commandList);
            }
        }

        return texture;
    }

    std::vector <uint8_t> TextureCache::ReadTextureFile(const GuGuUtf8Str &path) const {
        std::vector<uint8_t> file_content;

        GuGuUtf8Str filePath(path);
        AndroidGuGuFile file;
        file.OpenFile(filePath, GuGuFile::FileMode::OnlyRead);

        int32_t fileLength = file.getFileSize();
        file_content.resize(fileLength);

        int32_t haveReadedLength = 0;
        file.ReadFile(file_content.data(), fileLength, haveReadedLength);
        //assert(assetManager);
        //AAsset *file =
        //        AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
        //size_t file_length = AAsset_getLength(file);
//
        //file_content.resize(file_length);
//
        //AAsset_read(file, file_content.data(), file_length);
        //AAsset_close(file);
        file.CloseFile();
        return file_content;
    }

    bool TextureCache::FillTextureData(const std::vector<uint8_t> &fileData,
                                       const std::shared_ptr <TextureData> &texture,
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

    uint GetMipLevelsNum(uint width, uint height)
    {
        uint size = std::min(width, height);
        uint levelsNum = (uint)(logf((float)size) / logf(2.0f)) + 1;

        return levelsNum;
    }

    void
    TextureCache::FinalizeTexture(std::shared_ptr <TextureData> texture, CommonRenderPasses *passes,
                                  nvrhi::ICommandList *commandList) {
        //assert(texture->data);
        //assert(commandList);

        uint originalWidth = texture->width;
        uint originalHeight = texture->height;
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
        uint scaledWidth = originalWidth;
        uint scaledHeight = originalHeight;
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
}