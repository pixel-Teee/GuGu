#include <pch.h>

#include "vulkan-backend.h"
#include "vk_types.h"
#include "vulkan-constants.h"

#include <Core/GuGuUtf8Str.h>

namespace GuGu{
    namespace nvrhi::vulkan{
        static VkImageType textureDimensionToImageType(TextureDimension dimension)
        {
            switch (dimension)
            {
                case TextureDimension::Texture1D:
                case TextureDimension::Texture1DArray:
                    return VK_IMAGE_TYPE_1D;

                case TextureDimension::Texture2D:
                case TextureDimension::Texture2DArray:
                case TextureDimension::TextureCube:
                case TextureDimension::TextureCubeArray:
                case TextureDimension::Texture2DMS:
                case TextureDimension::Texture2DMSArray:
                    return VK_IMAGE_TYPE_2D;

                case TextureDimension::Texture3D:
                    return VK_IMAGE_TYPE_3D;

                case TextureDimension::Unknown:
                default:
                    utils::InvalidEnum();
                    return VK_IMAGE_TYPE_2D;
            }
        }

        static VkExtent3D pickImageExtent(const TextureDesc& d)
        {
            //return VkExtent3D(d.width, d.height, d.depth);
            VkExtent3D extent3D = {};
            extent3D.width = d.width;
            extent3D.height = d.height;
            extent3D.depth = d.depth;
            return extent3D;
        }

        static uint32_t pickImageLayers(const TextureDesc& d)
        {
            return d.arraySize;
        }

        static VkImageUsageFlags pickImageUsage(const TextureDesc& d)
        {
            const FormatInfo& formatInfo = getFormatInfo(d.format);

            VkImageUsageFlags ret = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT;

            if (d.isShaderResource)
                ret |= VK_IMAGE_USAGE_SAMPLED_BIT;

            if (d.isRenderTarget)
            {
                if (formatInfo.hasDepth || formatInfo.hasStencil)
                {
                    ret |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                } else {
                    ret |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                }
            }

            if (d.isUAV)
                ret |= VK_IMAGE_USAGE_STORAGE_BIT;

            if (d.isShadingRateSurface)
                ret |= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

            return ret;
        }

        // infer aspect flags for a given image format
        VkImageAspectFlags guessImageAspectFlags(VkFormat format)
        {
            switch(format)  // NOLINT(clang-diagnostic-switch-enum)
            {
                case VK_FORMAT_D16_UNORM:
                case VK_FORMAT_X8_D24_UNORM_PACK32:
                //case vk::Format::eD16Unorm:
                //case vk::Format::eX8D24UnormPack32:
                case VK_FORMAT_D32_SFLOAT:
                //case vk::Format::eD32Sfloat:
                    return VK_IMAGE_ASPECT_DEPTH_BIT;
                    //return vk::ImageAspectFlagBits::eDepth;

                case VK_FORMAT_S8_UINT:
                    return VK_IMAGE_ASPECT_STENCIL_BIT;

                case VK_FORMAT_D16_UNORM_S8_UINT:
                case VK_FORMAT_D24_UNORM_S8_UINT:
                case VK_FORMAT_D32_SFLOAT_S8_UINT:
                //case vk::Format::eD16UnormS8Uint:
                //case vk::Format::eD24UnormS8Uint:
                //case vk::Format::eD32SfloatS8Uint:
                    return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

                default:
                    return VK_IMAGE_ASPECT_COLOR_BIT;
            }
        }

        VkImageAspectFlags guessSubresourceImageAspectFlags(VkFormat format, Texture::TextureSubresourceViewType viewType)
        {
            VkImageAspectFlags flags = guessImageAspectFlags(format);
            if ((flags & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
                == (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
            {
                if (viewType == Texture::TextureSubresourceViewType::DepthOnly)
                {
                    flags = flags & (~VK_IMAGE_ASPECT_STENCIL_BIT);
                }
                else if(viewType == Texture::TextureSubresourceViewType::StencilOnly)
                {
                    flags = flags & (~VK_IMAGE_ASPECT_DEPTH_BIT);
                }
            }
            return flags;
        }

        static VkImageViewType textureDimensionToImageViewType(TextureDimension dimension)
        {
            switch (dimension)
            {
                case TextureDimension::Texture1D:
                    //return vk::ImageViewType::e1D;
                    return VK_IMAGE_VIEW_TYPE_1D;
                case TextureDimension::Texture1DArray:
                    //return vk::ImageViewType::e1DArray;
                    return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                case TextureDimension::Texture2D:
                case TextureDimension::Texture2DMS:
                    return VK_IMAGE_VIEW_TYPE_2D;

                case TextureDimension::Texture2DArray:
                case TextureDimension::Texture2DMSArray:
                    return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

                case TextureDimension::TextureCube:
                    return VK_IMAGE_VIEW_TYPE_CUBE;

                case TextureDimension::TextureCubeArray:
                    return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

                case TextureDimension::Texture3D:
                    return VK_IMAGE_VIEW_TYPE_3D;

                case TextureDimension::Unknown:
                default:
                    utils::InvalidEnum();
                    return VK_IMAGE_VIEW_TYPE_2D;
            }
        }

        static VkSampleCountFlagBits pickImageSampleCount(const TextureDesc& d)
        {
            switch(d.sampleCount)
            {
                case 1:
                    return VK_SAMPLE_COUNT_1_BIT;

                case 2:
                    return VK_SAMPLE_COUNT_2_BIT;

                case 4:
                    return VK_SAMPLE_COUNT_4_BIT;

                case 8:
                    return VK_SAMPLE_COUNT_8_BIT;

                case 16:
                    return VK_SAMPLE_COUNT_16_BIT;

                case 32:
                    return VK_SAMPLE_COUNT_32_BIT;

                case 64:
                    return VK_SAMPLE_COUNT_64_BIT;

                default:
                    utils::InvalidEnum();
                    return VK_SAMPLE_COUNT_1_BIT;
            }
        }

        VkImageCreateFlags pickImageFlags(const TextureDesc& d) //todo:may be error
        {
            VkImageCreateFlags flags = {};

            if (d.dimension == TextureDimension::TextureCube ||
                d.dimension == TextureDimension::TextureCubeArray)
                flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

            if (d.isTypeless)
                flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

            return flags;
        }

        // fills out all info fields in Texture based on a TextureDesc
        static void fillTextureInfo(Texture *texture, const TextureDesc& desc)
        {
            texture->desc = desc;

            VkImageType type = textureDimensionToImageType(desc.dimension);
            VkExtent3D extent = pickImageExtent(desc);
            uint32_t numLayers = pickImageLayers(desc);
            VkFormat format = convertFormat(desc.format);
            VkImageUsageFlags usage = pickImageUsage(desc);
            VkSampleCountFlagBits sampleCount = pickImageSampleCount(desc);
            VkImageCreateFlags flags = pickImageFlags(desc);

            texture->imageInfo = {};
            texture->imageInfo.imageType = type;
            texture->imageInfo.extent = extent;
            texture->imageInfo.mipLevels = desc.mipLevels;
            texture->imageInfo.arrayLayers = numLayers;
            texture->imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            texture->imageInfo.usage = usage;
            texture->imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            texture->imageInfo.samples = sampleCount;
            texture->imageInfo.flags = flags;
            //texture->imageInfo = vk::ImageCreateInfo()
            //        .setImageType(type)
            //        .setExtent(extent)
            //        .setMipLevels(desc.mipLevels)
            //        .setArrayLayers(numLayers)
            //        .setFormat(format)
            //        .setInitialLayout(vk::ImageLayout::eUndefined)
            //        .setUsage(usage)
            //        .setSharingMode(vk::SharingMode::eExclusive)
            //        .setSamples(sampleCount)
            //        .setFlags(flags);

#if _WIN32
            //Vk
            //const auto handleType = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32;
            VkExternalMemoryHandleTypeFlags  handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else
            VkExternalMemoryHandleTypeFlags  handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
            //const auto handleType = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd;
#endif
            texture->externalMemoryImageInfo = {};
            texture->externalMemoryImageInfo.handleTypes = handleType;
            //texture->externalMemoryImageInfo = vk::ExternalMemoryImageCreateInfo()
            //        .setHandleTypes(handleType);
//
            if (desc.sharedResourceFlags == SharedResourceFlags::Shared)
                texture->imageInfo.pNext = &texture->externalMemoryImageInfo;
                //texture->imageInfo.setPNext(&texture->externalMemoryImageInfo);
        }



        TextureHandle Device::createHandleForNativeTexture(ObjectType objectType, Object _texture, const TextureDesc& desc) {
            if (_texture.integer == 0)
                return nullptr;

            if (objectType != ObjectTypes::VK_Image)
                return nullptr;

            VkImage image{};
            image = (VkImage)_texture.integer;

            //todo:implement texture
            Texture *texture = new Texture(m_Context, m_Allocator);
            fillTextureInfo(texture, desc);

            texture->image = image;
            texture->managed = false;

            return TextureHandle::Create(texture);
            //return nullptr;
        }


        TextureSubresourceView& Texture::getSubresourceView(const TextureSubresourceSet& subresource, TextureDimension dimension,
                                                            Format format, TextureSubresourceViewType viewtype)
        {
            // This function is called from createBindingSet etc. and therefore free-threaded.
            // It modifies the subresourceViews map associated with the texture.
            std::lock_guard lockGuard(m_Mutex);

            if (dimension == TextureDimension::Unknown)
                dimension = desc.dimension;

            if (format == Format::UNKNOWN)
                format = desc.format;

            auto cachekey = std::make_tuple(subresource, viewtype, dimension, format);
            auto iter = subresourceViews.find(cachekey);
            if (iter != subresourceViews.end())
            {
                return iter->second;
            }

            auto iter_pair = subresourceViews.emplace(cachekey, *this);
            auto& view = std::get<0>(iter_pair)->second;

            view.subresource = subresource;

            auto vkformat = nvrhi::vulkan::convertFormat(format);

            VkImageAspectFlags aspectflags = guessSubresourceImageAspectFlags(vkformat, viewtype);

            VkImageSubresourceRange subresourceRange = {};
            subresourceRange.aspectMask = aspectflags;
            subresourceRange.baseMipLevel = subresource.baseMipLevel;
            subresourceRange.levelCount = subresource.numMipLevels;
            subresourceRange.baseArrayLayer = subresource.baseArraySlice;
            subresourceRange.layerCount = subresource.numArraySlices;
            view.subresourceRange = subresourceRange;
            //view.subresourceRange = vk::ImageSubresourceRange()
            //        .setAspectMask(aspectflags)
            //        .setBaseMipLevel(subresource.baseMipLevel)
            //        .setLevelCount(subresource.numMipLevels)
            //        .setBaseArrayLayer(subresource.baseArraySlice)
            //        .setLayerCount(subresource.numArraySlices);

            VkImageViewType imageViewType = textureDimensionToImageViewType(dimension);
            //vk::ImageViewType imageViewType = textureDimensionToImageViewType(dimension);

            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = imageViewType;
            viewInfo.format = vkformat;
            viewInfo.subresourceRange = view.subresourceRange;

            //auto viewInfo = vk::ImageViewCreateInfo()
            //        .setImage(image)
            //        .setViewType(imageViewType)
            //        .setFormat(vk::Format(vkformat))
            //        .setSubresourceRange(view.subresourceRange);

            if (viewtype == TextureSubresourceViewType::StencilOnly)
            {
                // D3D / HLSL puts stencil values in the second component to keep the illusion of combined depth/stencil.
                // Set a component swizzle so we appear to do the same.
                viewInfo.components.g = VK_COMPONENT_SWIZZLE_R;
                //viewInfo.components.setG(vk::ComponentSwizzle::eR);
            }

            VK_CHECK(vkCreateImageView(m_Context.device, &viewInfo, m_Context.allocationCallbacks, &view.view));
            //const VkResult res = m_Context.device.createImageView(&viewInfo, m_Context.allocationCallbacks, &view.view);
            //ASSERT_VK_OK(res);

            GuGuUtf8Str debugName("ImageView for: ");
            debugName.append(GuGuUtf8Str(utils::DebugNameToString(desc.debugName)));
            //const GuGuUtf8Str debugName = GuGuUtf8Str("ImageView for: ").append(GuGuUtf8Str(utils::DebugNameToString(desc.debugName)));
            m_Context.nameVKObject((void*)view.view, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT, debugName.getStr());

            return view;
        }

        uint32_t Texture::getNumSubresources() const {
            return desc.mipLevels * desc.arraySize;
        }

        uint32_t Texture::getSubresourceIndex(uint32_t mipLevel, uint32_t arrayLayer) const {
            return mipLevel * desc.arraySize + arrayLayer;
        }

        Texture::~Texture() {
            for (auto& viewIter : subresourceViews)
            {
                auto& view = viewIter.second.view;
                vkDestroyImageView(m_Context.device, view, m_Context.allocationCallbacks);
                //m_Context.device.destroyImageView(view, m_Context.allocationCallbacks);
                view = VK_NULL_HANDLE;
            }
            subresourceViews.clear();

            if (managed)
            {
                if (image)
                {
                   vkDestroyImage(m_Context.device, image, m_Context.allocationCallbacks);
                   //m_Context.device.destroyImage(image, m_Context.allocationCallbacks);
                   image = VK_NULL_HANDLE;
                }

                if (memory)
                {
                    m_Allocator.freeTextureMemory(this);
                    memory = VK_NULL_HANDLE;
                }
            }
        }

        Object Texture::getNativeObject(ObjectType objectType) {
            switch (objectType)
            {
                case ObjectTypes::VK_Image:
                    return Object(image);
                case ObjectTypes::VK_DeviceMemory:
                    return Object(memory);
                case ObjectTypes::SharedHandle:
                    return Object(sharedHandle);
                default:
                    return nullptr;
            }
        }

        Object Texture::getNativeView(ObjectType objectType, Format format,
                                      TextureSubresourceSet subresources,
                                      TextureDimension dimension, bool isReadOnlyDSV) {
            switch (objectType)
            {
                case ObjectTypes::VK_ImageView:
                {
                    if (format == Format::UNKNOWN)
                        format = desc.format;

                    const FormatInfo& formatInfo = getFormatInfo(format);

                    TextureSubresourceViewType viewType = TextureSubresourceViewType::AllAspects;
                    if (formatInfo.hasDepth && !formatInfo.hasStencil)
                        viewType = TextureSubresourceViewType::DepthOnly;
                    else if(!formatInfo.hasDepth && formatInfo.hasStencil)
                        viewType = TextureSubresourceViewType::StencilOnly;

                    return Object(getSubresourceView(subresources, dimension, format, viewType).view);
                }
                default:
                    return nullptr;
            }
        }
    }
}