#include <pch.h>

#include "nvrhi.h"

namespace GuGu{
    namespace nvrhi{
        TextureSubresourceSet TextureSubresourceSet::resolve(const TextureDesc& desc, bool singleMipLevel) const
        {
            TextureSubresourceSet ret;
            ret.baseMipLevel = baseMipLevel;

            if (singleMipLevel)
            {
                ret.numMipLevels = 1;
            }
            else
            {
                int lastMipLevelPlusOne = std::min(baseMipLevel + numMipLevels, desc.mipLevels);
                ret.numMipLevels = MipLevel(std::max(0u, lastMipLevelPlusOne - baseMipLevel));
            }

            switch (desc.dimension)  // NOLINT(clang-diagnostic-switch-enum)
            {
                case TextureDimension::Texture1DArray:
                case TextureDimension::Texture2DArray:
                case TextureDimension::TextureCube:
                case TextureDimension::TextureCubeArray:
                case TextureDimension::Texture2DMSArray: {
                    ret.baseArraySlice = baseArraySlice;
                    int lastArraySlicePlusOne = std::min(baseArraySlice + numArraySlices, desc.arraySize);
                    ret.numArraySlices = ArraySlice(std::max(0u, lastArraySlicePlusOne - baseArraySlice));
                    break;
                }
                default:
                    ret.baseArraySlice = 0;
                    ret.numArraySlices = 1;
                    break;
            }

            return ret;
        }

        BufferRange BufferRange::resolve(const BufferDesc& desc) const
        {
            BufferRange result;
            result.byteOffset = std::min(byteOffset, desc.byteSize);
            if (byteSize == 0)
                result.byteSize = desc.byteSize - result.byteOffset;
            else
                result.byteSize = std::min(byteSize, desc.byteSize - result.byteOffset);
            return result;
        }

        FramebufferInfo::FramebufferInfo(const FramebufferDesc &desc) {
            for (size_t i = 0; i < desc.colorAttachments.size(); i++)
            {
                const FramebufferAttachment& attachment = desc.colorAttachments[i];
                colorFormats.push_back(attachment.format == Format::UNKNOWN && attachment.texture ? attachment.texture->getDesc().format : attachment.format);
            }

            if (desc.depthAttachment.valid())
            {
                const TextureDesc& textureDesc = desc.depthAttachment.texture->getDesc();
                depthFormat = textureDesc.format;
                sampleCount = textureDesc.sampleCount;
                sampleQuality = textureDesc.sampleQuality;
            }
            else if (!desc.colorAttachments.empty() && desc.colorAttachments[0].valid())
            {
                const TextureDesc& textureDesc = desc.colorAttachments[0].texture->getDesc();
                sampleCount = textureDesc.sampleCount;
                sampleQuality = textureDesc.sampleQuality;
            }
        }

        FramebufferInfoEx::FramebufferInfoEx(const FramebufferDesc &desc) : FramebufferInfo(desc) {
            if (desc.depthAttachment.valid())
            {
                const TextureDesc& textureDesc = desc.depthAttachment.texture->getDesc();
                width = std::max(textureDesc.width >> desc.depthAttachment.subresources.baseMipLevel, 1u);
                height = std::max(textureDesc.height >> desc.depthAttachment.subresources.baseMipLevel, 1u);
            }
            else if (!desc.colorAttachments.empty() && desc.colorAttachments[0].valid())
            {
                const TextureDesc& textureDesc = desc.colorAttachments[0].texture->getDesc();
                width = std::max(textureDesc.width >> desc.colorAttachments[0].subresources.baseMipLevel, 1u);
                height = std::max(textureDesc.height >> desc.colorAttachments[0].subresources.baseMipLevel, 1u);
            }
        }

        bool BlendState::RenderTarget::usesConstantColor() const
        {
            return srcBlend == BlendFactor::ConstantColor || srcBlend == BlendFactor::OneMinusConstantColor ||
                   destBlend == BlendFactor::ConstantColor || destBlend == BlendFactor::OneMinusConstantColor ||
                   srcBlendAlpha == BlendFactor::ConstantColor || srcBlendAlpha == BlendFactor::OneMinusConstantColor ||
                   destBlendAlpha == BlendFactor::ConstantColor || destBlendAlpha == BlendFactor::OneMinusConstantColor;
        }


        bool BlendState::usesConstantColor(uint32_t numTargets) const {
            for (uint32_t rt = 0; rt < numTargets; rt++)
            {
                if (targets[rt].usesConstantColor())
                    return true;
            }

            return false;
        }

        bool TextureSubresourceSet::isEntireTexture(const TextureDesc& desc) const
        {
            if (baseMipLevel > 0u || baseMipLevel + numMipLevels < desc.mipLevels)
                return false;

            switch (desc.dimension)  // NOLINT(clang-diagnostic-switch-enum)
            {
                case TextureDimension::Texture1DArray:
                case TextureDimension::Texture2DArray:
                case TextureDimension::TextureCube:
                case TextureDimension::TextureCubeArray:
                case TextureDimension::Texture2DMSArray:
                    if (baseArraySlice > 0u || baseArraySlice + numArraySlices < desc.arraySize)
                        return false;
                default:
                    return true;
            }
        }

        void ICommandList::setResourceStatesForFramebuffer(IFramebuffer* framebuffer)
        {
            const FramebufferDesc& desc = framebuffer->getDesc();

            for (const auto& attachment : desc.colorAttachments)
            {
                setTextureState(attachment.texture, attachment.subresources,
                                ResourceStates::RenderTarget);
            }

            if (desc.depthAttachment.valid())
            {
                setTextureState(desc.depthAttachment.texture, desc.depthAttachment.subresources,
                                desc.depthAttachment.isReadOnly ? ResourceStates::DepthRead : ResourceStates::DepthWrite);
            }
        }
    }
}