#pragma once

#include <Renderer/nvrhi.h>

namespace GuGu{
    enum class TextureAlphaMode
    {
        UNKNOWN = 0,
        STRAIGHT = 1,
        PREMULTIPLIED = 2,
        OPAQUE_ = 3,
        CUSTOM = 4,
    };

    struct TextureSubresourceData
    {
        size_t rowPitch = 0;
        size_t depthPitch = 0;
        ptrdiff_t dataOffset = 0;
        size_t dataSize = 0;
    };

    struct LoadedTexture
    {
        nvrhi::TextureHandle texture;
        TextureAlphaMode alphaMode = TextureAlphaMode::UNKNOWN;
        uint32_t originalBitsPerPixel = 0;
        //DescriptorHandle bindlessDescriptor;
        GuGuUtf8Str path;
        GuGuUtf8Str mimeType;
    };//todo:move this class to other file

    struct TextureData : public LoadedTexture
    {
        //std::shared_ptr<vfs::IBlob> data;
        std::vector<uint8_t> data;//todo:remove this

        nvrhi::Format format = nvrhi::Format::UNKNOWN;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        uint32_t arraySize = 1;
        uint32_t mipLevels = 1;
        nvrhi::TextureDimension dimension = nvrhi::TextureDimension::Unknown;
        bool isRenderTarget = false;
        bool forceSRGB = false;

        // ArraySlice -> MipLevel -> TextureSubresourceData
        std::vector<std::vector<TextureSubresourceData>> dataLayout;
    };

    class CommonRenderPasses;
    class GuGuUtf8Str;
    class FileSystem;
    class GTexture;
    class TextureCache
    {
    protected:
        nvrhi::DeviceHandle m_Device;

        uint32_t m_MaxTextureSize = 0;

        bool m_GenerateMipmaps = true;

        std::shared_ptr<FileSystem> m_fileSystem;
    public:
        bool FillTextureData(const std::vector<uint8_t>& fileData, const std::shared_ptr<TextureData>& texture, const GuGuUtf8Str& extension, const GuGuUtf8Str& mimeType) const;
        void FinalizeTexture(std::shared_ptr<TextureData> texture, CommonRenderPasses* passes, nvrhi::ICommandList* commandList);
        void FinalizeTexture(std::shared_ptr<GTexture> gTexture, CommonRenderPasses* passes, nvrhi::ICommandList* commandList);
    public:
        TextureCache(nvrhi::IDevice* device, std::shared_ptr<FileSystem> fileSysem);
        virtual ~TextureCache();

        std::shared_ptr<LoadedTexture> LoadTextureFromFile(const GuGuUtf8Str& path, bool sRGB, CommonRenderPasses* passes, nvrhi::ICommandList* commandList);
    };
}