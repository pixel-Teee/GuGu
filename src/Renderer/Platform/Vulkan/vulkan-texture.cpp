#include <pch.h>

#include "vulkan-backend.h"
#include "vk_types.h"
#include "vulkan-constants.h"

#include <Core/GuGuUtf8Str.h>

#include <Renderer/misc.h>

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
            texture->imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            texture->imageInfo.imageType = type;
            texture->imageInfo.extent = extent;
            texture->imageInfo.mipLevels = desc.mipLevels;
            texture->imageInfo.arrayLayers = numLayers;
            texture->imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            texture->imageInfo.usage = usage;
            texture->imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            texture->imageInfo.samples = sampleCount;
            texture->imageInfo.flags = flags;
            texture->imageInfo.format = format;
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
            m_Context.nameVKObject((void*)view.view, VK_OBJECT_TYPE_IMAGE_VIEW, debugName.getStr());

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


        void CommandList::clearTextureFloat(ITexture *texture, TextureSubresourceSet subresources,
                                            const Color &clearColor) {
            //auto clearValue = vk::ClearColorValue()
            //        .setFloat32({ clearColor.r, clearColor.g, clearColor.b, clearColor.a });
//
            //clearTexture(texture, subresources, clearValue);

            VkClearColorValue clearValue = {};
            clearValue.float32[0] = clearColor.r;
            clearValue.float32[1] = clearColor.g;
            clearValue.float32[2] = clearColor.b;
            clearValue.float32[3] = clearColor.a;

            clearTexture(texture, subresources, clearValue);
        }

        void CommandList::clearTexture(ITexture *_texture, TextureSubresourceSet subresources,
                                       const VkClearColorValue &clearValue) {
            endRenderPass();

            Texture* texture = checked_cast<Texture*>(_texture);
            assert(texture);
            assert(m_CurrentCmdBuf);

            subresources = subresources.resolve(texture->desc, false);

            if (m_EnableAutomaticBarriers)
            {
                requireTextureState(texture, subresources, ResourceStates::CopyDest);
            }
            commitBarriers();

            VkImageSubresourceRange subresourceRange = {};
            subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresourceRange.baseArrayLayer = subresources.baseArraySlice;
            subresourceRange.layerCount = subresources.numArraySlices;
            subresourceRange.baseMipLevel = subresources.baseMipLevel;
            subresourceRange.levelCount = subresources.numMipLevels;

            vkCmdClearColorImage(m_CurrentCmdBuf->cmdBuf,
                                 texture->image,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                 &clearValue,
                                 1,
                                 &subresourceRange);

            //vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
            //        .setAspectMask(vk::ImageAspectFlagBits::eColor)
            //        .setBaseArrayLayer(subresources.baseArraySlice)
            //        .setLayerCount(subresources.numArraySlices)
            //        .setBaseMipLevel(subresources.baseMipLevel)
            //        .setLevelCount(subresources.numMipLevels);
//
            //m_CurrentCmdBuf->cmdBuf.clearColorImage(texture->image,
            //                                        vk::ImageLayout::eTransferDstOptimal,
            //                                        &clearValue,
            //                                        1, &subresourceRange);
        }

    static VkBorderColor pickSamplerBorderColor(const SamplerDesc& d)
    {
        if (d.borderColor.r == 0.f && d.borderColor.g == 0.f && d.borderColor.b == 0.f)
        {
            if (d.borderColor.a == 0.f)
            {
                return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            }

            if (d.borderColor.a == 1.f)
            {
                return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
            }
        }

        if (d.borderColor.r == 1.f && d.borderColor.g == 1.f && d.borderColor.b == 1.f)
        {
            if (d.borderColor.a == 1.f)
            {
                return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            }
        }

        utils::NotSupported();
        return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    }

        SamplerHandle Device::createSampler(const SamplerDesc& desc)
        {
            Sampler *sampler = new Sampler(m_Context);

            const bool anisotropyEnable = desc.maxAnisotropy > 1.0f;

            sampler->desc = desc;

            VkSamplerCreateInfo samplerCreateInfo = {};
            samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerCreateInfo.magFilter = desc.magFilter ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
            samplerCreateInfo.minFilter = desc.minFilter ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
            samplerCreateInfo.mipmapMode = desc.mipFilter ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
            samplerCreateInfo.addressModeU = convertSamplerAddressMode(desc.addressU);
            samplerCreateInfo.addressModeV = convertSamplerAddressMode(desc.addressV);
            samplerCreateInfo.addressModeW = convertSamplerAddressMode(desc.addressW);
            samplerCreateInfo.mipLodBias = desc.mipBias;
            samplerCreateInfo.anisotropyEnable = anisotropyEnable;
            samplerCreateInfo.maxAnisotropy = anisotropyEnable ? desc.maxAnisotropy : 1.0f;
            samplerCreateInfo.compareEnable = desc.reductionType == SamplerReductionType::Comparison;
            samplerCreateInfo.compareOp = VK_COMPARE_OP_LESS;
            samplerCreateInfo.minLod = 0.0f;
            samplerCreateInfo.maxLod = std::numeric_limits<float>::max();
            samplerCreateInfo.borderColor = pickSamplerBorderColor(desc);
            sampler->samplerInfo = samplerCreateInfo;
            //sampler->samplerInfo = vk::SamplerCreateInfo()
            //        .setMagFilter(desc.magFilter ? vk::Filter::eLinear : vk::Filter::eNearest)
            //        .setMinFilter(desc.minFilter ? vk::Filter::eLinear : vk::Filter::eNearest)
            //        .setMipmapMode(desc.mipFilter ? vk::SamplerMipmapMode::eLinear : vk::SamplerMipmapMode::eNearest)
            //        .setAddressModeU(convertSamplerAddressMode(desc.addressU))
            //        .setAddressModeV(convertSamplerAddressMode(desc.addressV))
            //        .setAddressModeW(convertSamplerAddressMode(desc.addressW))
            //        .setMipLodBias(desc.mipBias)
            //        .setAnisotropyEnable(anisotropyEnable)
            //        .setMaxAnisotropy(anisotropyEnable ? desc.maxAnisotropy : 1.f)
            //        .setCompareEnable(desc.reductionType == SamplerReductionType::Comparison)
            //        .setCompareOp(vk::CompareOp::eLess)
            //        .setMinLod(0.f)
            //        .setMaxLod(std::numeric_limits<float>::max())
            //        .setBorderColor(pickSamplerBorderColor(desc));
    //
            VkSamplerReductionModeCreateInfoEXT samplerReductionModeCreateInfoExt;
            if (desc.reductionType == SamplerReductionType::Minimum || desc.reductionType == SamplerReductionType::Maximum)
            {
                VkSamplerReductionModeEXT reductionMode =
                        desc.reductionType == SamplerReductionType::Maximum ? VK_SAMPLER_REDUCTION_MODE_MAX : VK_SAMPLER_REDUCTION_MODE_MIN;
                samplerReductionModeCreateInfoExt.reductionMode = reductionMode;
                sampler->samplerInfo.pNext = &samplerReductionModeCreateInfoExt;
                //samplerReductionCreateInfo.setReductionMode(reductionMode);
                //sampler->samplerInfo.setPNext(&samplerReductionCreateInfo);
            }
            //vk::SamplerReductionModeCreateInfoEXT samplerReductionCreateInfo;
            //if (desc.reductionType == SamplerReductionType::Minimum || desc.reductionType == SamplerReductionType::Maximum)
            //{
            //    vk::SamplerReductionModeEXT reductionMode =
            //            desc.reductionType == SamplerReductionType::Maximum ? vk::SamplerReductionModeEXT::eMax : vk::SamplerReductionModeEXT::eMin;
            //    samplerReductionCreateInfo.setReductionMode(reductionMode);
    //
            //    sampler->samplerInfo.setPNext(&samplerReductionCreateInfo);
            //}
    //
            VkResult res = vkCreateSampler(m_Context.device, &sampler->samplerInfo, m_Context.allocationCallbacks, &sampler->sampler);
            //const vk::Result res = m_Context.device.createSampler(&sampler->samplerInfo, m_Context.allocationCallbacks, &sampler->sampler);
            //CHECK_VK_FAIL(res)
    //
            return SamplerHandle::Create(sampler);
        }

        Object Sampler::getNativeObject(ObjectType objectType)
        {
            switch (objectType)
            {
                case ObjectTypes::VK_Sampler:
                    return Object(sampler);
                default:
                    return nullptr;
            }
        }

        Sampler::~Sampler()
        {
            vkDestroySampler(m_Context.device, sampler, m_Context.allocationCallbacks);
            //m_Context.device.destroySampler(sampler);
        }

        TextureHandle Device::createTexture(const TextureDesc& desc)
        {
            Texture *texture = new Texture(m_Context, m_Allocator);
            assert(texture);
            fillTextureInfo(texture, desc);

            VkResult res = vkCreateImage(m_Context.device, &texture->imageInfo, m_Context.allocationCallbacks, &texture->image);
            //VkResult res = m_Context.device.createImage(&texture->imageInfo, m_Context.allocationCallbacks, &texture->image);
            //ASSERT_VK_OK(res);
            //CHECK_VK_FAIL(res)

            VK_CHECK(res);

            m_Context.nameVKObject((void*)texture->image, VK_OBJECT_TYPE_IMAGE, desc.debugName.getStr());

            if (!desc.isVirtual)
            {
                res = m_Allocator.allocateTextureMemory(texture);
                //VK_CHECK(res);
                //ASSERT_VK_OK(res);
                //CHECK_VK_FAIL(res)

                if((desc.sharedResourceFlags & SharedResourceFlags::Shared) != 0)
                {
#ifdef _WIN32
                    //texture->sharedHandle = m_Context.device.getMemoryWin32HandleKHR({ texture->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 });
#else
                    //texture->sharedHandle = (void*)(size_t)m_Context.device.getMemoryFdKHR({ texture->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd });
#endif
                }

                m_Context.nameVKObject((void*)texture->memory, VK_OBJECT_TYPE_DEVICE_MEMORY, desc.debugName.getStr());
            }

            return TextureHandle::Create(texture);
        }

    static void computeMipLevelInformation(const TextureDesc& desc, uint32_t mipLevel, uint32_t* widthOut, uint32_t* heightOut, uint32_t* depthOut)
    {
        uint32_t width = std::max(desc.width >> mipLevel, uint32_t(1));
        uint32_t height = std::max(desc.height >> mipLevel, uint32_t(1));
        uint32_t depth = std::max(desc.depth >> mipLevel, uint32_t(1));

        if (widthOut)
            *widthOut = width;
        if (heightOut)
            *heightOut = height;
        if (depthOut)
            *depthOut = depth;
    }

    void CommandList::writeTexture(ITexture* _dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch)
    {
        endRenderPass();

        Texture* dest = checked_cast<Texture*>(_dest);

        TextureDesc desc = dest->getDesc();

        uint32_t mipWidth, mipHeight, mipDepth;
        computeMipLevelInformation(desc, mipLevel, &mipWidth, &mipHeight, &mipDepth);

        const FormatInfo& formatInfo = getFormatInfo(desc.format);
        uint32_t deviceNumCols = (mipWidth + formatInfo.blockSize - 1) / formatInfo.blockSize;
        uint32_t deviceNumRows = (mipHeight + formatInfo.blockSize - 1) / formatInfo.blockSize;
        uint32_t deviceRowPitch = deviceNumCols * formatInfo.bytesPerBlock;
        uint32_t deviceMemSize = deviceRowPitch * deviceNumRows * mipDepth;

        Buffer* uploadBuffer;
        uint64_t uploadOffset;
        void* uploadCpuVA;
        m_UploadManager->suballocateBuffer(
                deviceMemSize,
                &uploadBuffer,
                &uploadOffset,
                &uploadCpuVA,
                MakeVersion(m_CurrentCmdBuf->recordingID, m_CommandListParameters.queueType, false));

        size_t minRowPitch = std::min(size_t(deviceRowPitch), rowPitch);
        uint8_t* mappedPtr = (uint8_t*)uploadCpuVA;
        for (uint32_t slice = 0; slice < mipDepth; slice++)
        {
            const uint8_t* sourcePtr = (const uint8_t*)data + depthPitch * slice;
            for (uint32_t row = 0; row < deviceNumRows; row++)
            {
                memcpy(mappedPtr, sourcePtr, minRowPitch);
                mappedPtr += deviceRowPitch;
                sourcePtr += rowPitch;
            }
        }

        VkBufferImageCopy imageCopy = {};
        imageCopy.bufferOffset = uploadOffset;
        imageCopy.bufferRowLength = deviceNumCols * formatInfo.blockSize;
        imageCopy.bufferImageHeight = deviceNumRows * formatInfo.blockSize;

        VkImageSubresourceLayers subresourceLayers = {};
        subresourceLayers.layerCount = 1;
        subresourceLayers.baseArrayLayer = arraySlice;
        subresourceLayers.mipLevel = mipLevel;
        subresourceLayers.aspectMask = guessImageAspectFlags(dest->imageInfo.format);

        imageCopy.imageSubresource = subresourceLayers;
        VkExtent3D extent3D = {};
        extent3D.width = mipWidth;
        extent3D.height = mipHeight;
        extent3D.depth = mipDepth;

        imageCopy.imageExtent = extent3D;

        //auto imageCopy = vk::BufferImageCopy()
        //        .setBufferOffset(uploadOffset)
        //        .setBufferRowLength(deviceNumCols * formatInfo.blockSize)
        //        .setBufferImageHeight(deviceNumRows * formatInfo.blockSize)
        //        .setImageSubresource(vk::ImageSubresourceLayers()
        //                                     .setAspectMask(guessImageAspectFlags(dest->imageInfo.format))
        //                                     .setMipLevel(mipLevel)
        //                                     .setBaseArrayLayer(arraySlice)
        //                                     .setLayerCount(1))
        //        .setImageExtent(vk::Extent3D().setWidth(mipWidth).setHeight(mipHeight).setDepth(mipDepth));
//
        //assert(m_CurrentCmdBuf);
//
        if (m_EnableAutomaticBarriers)
        {
            requireTextureState(dest, TextureSubresourceSet(mipLevel, 1, arraySlice, 1), ResourceStates::CopyDest);
        }
        commitBarriers();
//
        m_CurrentCmdBuf->referencedResources.push_back(dest);
//
        vkCmdCopyBufferToImage(m_CurrentCmdBuf->cmdBuf, uploadBuffer->buffer, dest->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

        //m_CurrentCmdBuf->cmdBuf.copyBufferToImage(uploadBuffer->buffer,
        //                                          dest->image, vk::ImageLayout::eTransferDstOptimal,
          //                                        1, &imageCopy);
    }
    }
}