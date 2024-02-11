#include <pch.h>

#include "vulkan-backend.h"
#include "vk_types.h"
#include "VulkanRenderer.h"

#include <Renderer/containers.h>

#include <Renderer/misc.h>
#include <nvrhi.h>


namespace GuGu{

    namespace nvrhi::vulkan{
        template <typename T>
        using attachment_vector = ::GuGu::nvrhi::static_vector<T, ::GuGu::nvrhi::c_MaxRenderTargets + 1>; // render targets + depth

        static TextureDimension getDimensionForFramebuffer(TextureDimension dimension, bool isArray)
        {
            // Can't render into cubes and 3D textures directly, convert them to 2D arrays
            if (dimension == TextureDimension::TextureCube || dimension == TextureDimension::TextureCubeArray || dimension == TextureDimension::Texture3D)
                dimension = TextureDimension::Texture2DArray;

            if (!isArray)
            {
                // Demote arrays to single textures if we just need one layer
                switch(dimension)  // NOLINT(clang-diagnostic-switch-enum)
                {
                    case TextureDimension::Texture1DArray:
                        dimension = TextureDimension::Texture1D;
                        break;
                    case TextureDimension::Texture2DArray:
                        dimension = TextureDimension::Texture2D;
                        break;
                    case TextureDimension::Texture2DMSArray:
                        dimension = TextureDimension::Texture2DMS;
                        break;
                    default:
                        break;
                }
            }

            return dimension;
        }

        Framebuffer::~Framebuffer() {
            if (framebuffer && managed)
            {
                vkDestroyFramebuffer(m_Context.device, framebuffer, nullptr);

                framebuffer = VK_NULL_HANDLE;
            }

            if (renderPass && managed)
            {
                vkDestroyRenderPass(m_Context.device, renderPass, nullptr);
                //m_Context.device.destroyRenderPass(renderPass);
                renderPass = VK_NULL_HANDLE;
            }
        }

        Object Framebuffer::getNativeObject(ObjectType objectType) {
            switch (objectType)
            {
                case ObjectTypes::VK_RenderPass:
                    return Object(renderPass);
                case ObjectTypes::VK_Framebuffer:
                    return Object(framebuffer);
                default:
                    return nullptr;
            }
        }

        void countSpecializationConstants(
                Shader* shader,
                size_t& numShaders,
                size_t& numShadersWithSpecializations,
                size_t& numSpecializationConstants)
        {
            if (!shader)
                return;

            numShaders += 1;

            if (shader->specializationConstants.empty())
                return;

            numShadersWithSpecializations += 1;
            numSpecializationConstants += shader->specializationConstants.size();
        }

        VkPipelineShaderStageCreateInfo
        makeShaderStageCreateInfo(Shader *shader, std::vector <VkSpecializationInfo> &specInfos,
                                  std::vector <VkSpecializationMapEntry> &specMapEntries,
                                  std::vector <uint32_t> &specData) {
            VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
            shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageCreateInfo.stage = shader->stageFlagBits;
            shaderStageCreateInfo.module = shader->shaderModule;
            shaderStageCreateInfo.pName = shader->desc.entryName.getStr();

            if(!shader->specializationConstants.empty())
            {
                //todo:implement this function
                // For specializations, this functions allocates:
                //  - One entry in specInfos per shader
                //  - One entry in specMapEntries and specData each per constant
                // The vectors are pre-allocated, so it's safe to use .data() before writing the data

                assert(specInfos.data());
                assert(specMapEntries.data());
                assert(specData.data());

                shaderStageCreateInfo.pSpecializationInfo = (specInfos.data() + specInfos.size());

                VkSpecializationInfo specializationInfo = {};
                specializationInfo.pMapEntries = specMapEntries.data() + specMapEntries.size();
                specializationInfo.mapEntryCount = static_cast<uint32_t>(shader->specializationConstants.size());
                specializationInfo.pData = specData.data() + specData.size();
                specializationInfo.dataSize = shader->specializationConstants.size() * sizeof(uint32_t);

                //auto specInfo = vk::SpecializationInfo()
                //        .setPMapEntries(specMapEntries.data() + specMapEntries.size())
                //        .setMapEntryCount(static_cast<uint32_t>(shader->specializationConstants.size()))
                //        .setPData(specData.data() + specData.size())
                //        .setDataSize(shader->specializationConstants.size() * sizeof(uint32_t));
//
                size_t dataOffset = 0;
                for (const auto& constant : shader->specializationConstants)
                {
                    VkSpecializationMapEntry specMapEntry = {};
                    specMapEntry.constantID = constant.constantID;
                    specMapEntry.offset = static_cast<uint32_t>(dataOffset);
                    specMapEntry.size = sizeof(uint32_t);
                    //auto specMapEntry = vk::SpecializationMapEntry()
                    //        .setConstantID(constant.constantID)
                    //        .setOffset(static_cast<uint32_t>(dataOffset))
                    //        .setSize(sizeof(uint32_t));

                    specMapEntries.push_back(specMapEntry);
                    specData.push_back(constant.value.u);
                    dataOffset += specMapEntry.size;
                }

                specInfos.push_back(specializationInfo);
            }

            return shaderStageCreateInfo;
        }

        VkPolygonMode convertFillMode(RasterFillMode mode)
        {
            switch(mode)
            {
                case RasterFillMode::Fill:
                    return VK_POLYGON_MODE_FILL;

                case RasterFillMode::Line:
                    return VK_POLYGON_MODE_LINE;

                default:
                    assert(0);
                    return VK_POLYGON_MODE_FILL;
            }
        }

        VkCullModeFlagBits convertCullMode(RasterCullMode mode)
        {
            switch(mode)
            {
                case RasterCullMode::Back:
                    return VK_CULL_MODE_BACK_BIT;

                case RasterCullMode::Front:
                    return VK_CULL_MODE_FRONT_BIT;

                case RasterCullMode::None:
                    return VK_CULL_MODE_NONE;

                default:
                    assert(0);
                    return VK_CULL_MODE_NONE;
            }
        }

        VkPrimitiveTopology convertPrimitiveTopology(PrimitiveType topology)
        {
            switch(topology)
            {
                case PrimitiveType::PointList:
                    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

                case PrimitiveType::LineList:
                    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

                case PrimitiveType::TriangleList:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

                case PrimitiveType::TriangleStrip:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

                case PrimitiveType::TriangleFan:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

                case PrimitiveType::TriangleListWithAdjacency:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

                case PrimitiveType::TriangleStripWithAdjacency:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

                case PrimitiveType::PatchList:
                    return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

                default:
                    assert(0);
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            }
        }

        VkCompareOp convertCompareOp(ComparisonFunc op)
        {
            switch(op)
            {
                case ComparisonFunc::Never:
                    return VK_COMPARE_OP_NEVER;

                case ComparisonFunc::Less:
                    return VK_COMPARE_OP_LESS;

                case ComparisonFunc::Equal:
                    return VK_COMPARE_OP_EQUAL;

                case ComparisonFunc::LessOrEqual:
                    return VK_COMPARE_OP_LESS_OR_EQUAL;

                case ComparisonFunc::Greater:
                    return VK_COMPARE_OP_GREATER;

                case ComparisonFunc::NotEqual:
                    return VK_COMPARE_OP_NOT_EQUAL;

                case ComparisonFunc::GreaterOrEqual:
                    return VK_COMPARE_OP_GREATER_OR_EQUAL;

                case ComparisonFunc::Always:
                    return VK_COMPARE_OP_ALWAYS;

                default:
                    utils::InvalidEnum();
                    return VK_COMPARE_OP_ALWAYS;
            }
        }


        VkStencilOp convertStencilOp(StencilOp op)
        {
            switch(op)
            {
                case StencilOp::Keep:
                    return VK_STENCIL_OP_KEEP;

                case StencilOp::Zero:
                    return VK_STENCIL_OP_ZERO;

                case StencilOp::Replace:
                    return VK_STENCIL_OP_REPLACE;

                case StencilOp::IncrementAndClamp:
                    return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

                case StencilOp::DecrementAndClamp:
                    return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

                case StencilOp::Invert:
                    return VK_STENCIL_OP_INVERT;

                case StencilOp::IncrementAndWrap:
                    return VK_STENCIL_OP_INCREMENT_AND_WRAP;

                case StencilOp::DecrementAndWrap:
                    return VK_STENCIL_OP_DECREMENT_AND_WRAP;

                default:
                    utils::InvalidEnum();
                    return VK_STENCIL_OP_KEEP;
            }
        }

        VkStencilOpState convertStencilState(const DepthStencilState& depthStencilState, const DepthStencilState::StencilOpDesc& desc)
        {
            //return vk::StencilOpState()
            //        .setFailOp(convertStencilOp(desc.failOp))
            //        .setPassOp(convertStencilOp(desc.passOp))
            //        .setDepthFailOp(convertStencilOp(desc.depthFailOp))
            //        .setCompareOp(convertCompareOp(desc.stencilFunc))
            //        .setCompareMask(depthStencilState.stencilReadMask)
            //        .setWriteMask(depthStencilState.stencilWriteMask)
            //        .setReference(depthStencilState.stencilRefValue);

            VkStencilOpState stencilOpState = {};
            stencilOpState.failOp = convertStencilOp(desc.failOp);
            stencilOpState.passOp = convertStencilOp(desc.passOp);
            stencilOpState.depthFailOp = convertStencilOp(desc.depthFailOp);
            stencilOpState.compareOp = convertCompareOp(desc.stencilFunc);
            stencilOpState.compareMask = depthStencilState.stencilReadMask;
            stencilOpState.writeMask = depthStencilState.stencilWriteMask;
            stencilOpState.reference = depthStencilState.stencilRefValue;

            return stencilOpState;
        }

        VkFragmentShadingRateCombinerOpKHR convertShadingRateCombiner(ShadingRateCombiner combiner)
        {
            switch (combiner)
            {
                case ShadingRateCombiner::Override:
                    return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
                case ShadingRateCombiner::Min:
                    return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;
                case ShadingRateCombiner::Max:
                    return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
                case ShadingRateCombiner::ApplyRelative:
                    return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;
                case ShadingRateCombiner::Passthrough:
                default:
                    return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
            }
        }

        VkExtent2D convertFragmentShadingRate(VariableShadingRate shadingRate)
        {
            switch (shadingRate)
            {
                case VariableShadingRate::e1x2:
                {
                    VkExtent2D extent2D = {};
                    extent2D.width = 1;
                    extent2D.height = 2;
                    return extent2D;
                }
                case VariableShadingRate::e2x1:
                {
                    VkExtent2D extent2D = {};
                    extent2D.width = 2;
                    extent2D.height = 1;
                    return extent2D;
                }
                case VariableShadingRate::e2x2:
                {
                    VkExtent2D extent2D = {};
                    extent2D.width = 2;
                    extent2D.height = 2;
                    return extent2D;
                }
                case VariableShadingRate::e2x4:
                {
                    VkExtent2D extent2D = {};
                    extent2D.width = 2;
                    extent2D.height = 4;
                    return extent2D;
                }
                case VariableShadingRate::e4x2:
                {
                    VkExtent2D extent2D = {};
                    extent2D.width = 4;
                    extent2D.height = 2;
                    return extent2D;
                }
                case VariableShadingRate::e4x4:
                {
                    VkExtent2D extent2D = {};
                    extent2D.width = 4;
                    extent2D.height = 4;
                    return extent2D;
                }
                case VariableShadingRate::e1x1:
                default:
                {
                    VkExtent2D extent2D = {};
                    extent2D.width = 1;
                    extent2D.height = 1;
                    return extent2D;
                }
            }
        }

        VkBlendFactor convertBlendValue(BlendFactor value)
        {
            switch(value)
            {
                case BlendFactor::Zero:
                    return VK_BLEND_FACTOR_ZERO;

                case BlendFactor::One:
                    return VK_BLEND_FACTOR_ONE;

                case BlendFactor::SrcColor:
                    return VK_BLEND_FACTOR_SRC_COLOR;

                case BlendFactor::OneMinusSrcColor:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

                case BlendFactor::SrcAlpha:
                    return VK_BLEND_FACTOR_SRC_ALPHA;

                case BlendFactor::OneMinusSrcAlpha:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

                case BlendFactor::DstAlpha:
                    return VK_BLEND_FACTOR_DST_ALPHA;

                case BlendFactor::OneMinusDstAlpha:
                    return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

                case BlendFactor::DstColor:
                    return VK_BLEND_FACTOR_DST_COLOR;

                case BlendFactor::OneMinusDstColor:
                    return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

                case BlendFactor::SrcAlphaSaturate:
                    return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

                case BlendFactor::ConstantColor:
                    return VK_BLEND_FACTOR_CONSTANT_COLOR;

                case BlendFactor::OneMinusConstantColor:
                    return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

                case BlendFactor::Src1Color:
                    return VK_BLEND_FACTOR_SRC1_COLOR;

                case BlendFactor::OneMinusSrc1Color:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

                case BlendFactor::Src1Alpha:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

                case BlendFactor::OneMinusSrc1Alpha:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

                default:
                    assert(0);
                    return VK_BLEND_FACTOR_ZERO;
            }
        }

        VkBlendOp convertBlendOp(BlendOp op)
        {
            switch(op)
            {
                case BlendOp::Add:
                    return VK_BLEND_OP_ADD;

                case BlendOp::Subrtact:
                    return VK_BLEND_OP_SUBTRACT;

                case BlendOp::ReverseSubtract:
                    return VK_BLEND_OP_REVERSE_SUBTRACT;

                case BlendOp::Min:
                    return VK_BLEND_OP_MIN;

                case BlendOp::Max:
                    return VK_BLEND_OP_MAX;

                default:
                    assert(0);
                    return VK_BLEND_OP_ADD;
            }
        }

        static VkViewport VKViewportWithDXCoords(const Viewport& v)
        {
            // requires VK_KHR_maintenance1 which allows negative-height to indicate an inverted coord space to match DX
            VkViewport viewport = {};
            viewport.x = v.minX;
            viewport.y = v.maxY;
            viewport.width = v.maxX - v.minX;
            viewport.height = -(v.maxY - v.minY);
            viewport.minDepth = v.minZ;
            viewport.maxDepth = v.maxZ;
            return viewport;
        }

        VkColorComponentFlags convertColorMask(ColorMask mask)
        {
            VkColorComponentFlagBits colorComponentFlagBits = {};
            colorComponentFlagBits = VkColorComponentFlagBits(uint8_t(mask));
            return colorComponentFlagBits;
        }

        VkPipelineColorBlendAttachmentState convertBlendState(const BlendState::RenderTarget& state)
        {
            //return vk::PipelineColorBlendAttachmentState()
            //        .setBlendEnable(state.blendEnable)
            //        .setSrcColorBlendFactor(convertBlendValue(state.srcBlend))
            //        .setDstColorBlendFactor(convertBlendValue(state.destBlend))
            //        .setColorBlendOp(convertBlendOp(state.blendOp))
            //        .setSrcAlphaBlendFactor(convertBlendValue(state.srcBlendAlpha))
            //        .setDstAlphaBlendFactor(convertBlendValue(state.destBlendAlpha))
            //        .setAlphaBlendOp(convertBlendOp(state.blendOpAlpha))
            //        .setColorWriteMask(convertColorMask(state.colorWriteMask));
            VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
            pipelineColorBlendAttachmentState.blendEnable = state.blendEnable;
            pipelineColorBlendAttachmentState.srcColorBlendFactor = convertBlendValue(state.srcBlend);
            pipelineColorBlendAttachmentState.dstColorBlendFactor = convertBlendValue(state.destBlend);
            pipelineColorBlendAttachmentState.colorBlendOp = convertBlendOp(state.blendOp);
            pipelineColorBlendAttachmentState.srcAlphaBlendFactor = convertBlendValue(state.srcBlendAlpha);
            pipelineColorBlendAttachmentState.dstAlphaBlendFactor = convertBlendValue(state.destBlendAlpha);
            pipelineColorBlendAttachmentState.alphaBlendOp = convertBlendOp(state.blendOpAlpha);
            pipelineColorBlendAttachmentState.colorWriteMask = convertColorMask(state.colorWriteMask);

            return pipelineColorBlendAttachmentState;
        }

        FramebufferHandle Device::createFramebuffer(const FramebufferDesc &desc) {
            Framebuffer* fb = new Framebuffer(m_Context);
            fb->desc = desc;
            fb->framebufferInfo = FramebufferInfoEx(desc);

            attachment_vector<VkAttachmentDescription2> attachmentDescs(desc.colorAttachments.size());
            attachment_vector<VkAttachmentReference2> colorAttachmentRefs(desc.colorAttachments.size());//note:use for render pass
            VkAttachmentReference2 depthAttachmentRef;

            static_vector<VkImageView, c_MaxRenderTargets + 1> attachmentViews;
            attachmentViews.resize(desc.colorAttachments.size());

            uint32_t numArraySlices = 0;
            for(uint32_t i = 0; i < desc.colorAttachments.size(); ++i)
            {
                const auto& rt = desc.colorAttachments[i];
                Texture* t = checked_cast<Texture*>(rt.texture);

                assert(fb->framebufferInfo.width == std::max(t->desc.width >> rt.subresources.baseMipLevel, 1u));
                assert(fb->framebufferInfo.height == std::max(t->desc.height >> rt.subresources.baseMipLevel, 1u));

                const VkFormat attachmentFormat = (rt.format == Format::UNKNOWN ? t->imageInfo.format : convertFormat(rt.format));

                attachmentDescs[i] = {};
                VkAttachmentDescription2 attachmentDescription2{};
                attachmentDescription2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
                attachmentDescription2.format = attachmentFormat;
                attachmentDescription2.samples = t->imageInfo.samples;
                attachmentDescription2.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                attachmentDescription2.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescription2.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentDescription2.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentDescs[i] = attachmentDescription2;

                colorAttachmentRefs[i] = {};
                VkAttachmentReference2 attachmentReference2{};
                attachmentReference2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                attachmentReference2.attachment = i;
                attachmentReference2.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentRefs[i] = attachmentReference2;

                TextureSubresourceSet subresources = rt.subresources.resolve(t->desc, true);

                TextureDimension  dimension;
                dimension = getDimensionForFramebuffer(t->desc.dimension, subresources.numArraySlices >> 1);//todo:fix this
                const auto& view = t->getSubresourceView(subresources, dimension, rt.format);
                attachmentViews[i] = view.view;

                fb->resources.push_back(rt.texture);
                if(numArraySlices)
                    assert(numArraySlices == subresources.numArraySlices);
                else
                    numArraySlices = subresources.numArraySlices;
            }

            //add depth/stencil attachment if present
            if(desc.depthAttachment.valid())
            {
                const auto& att = desc.depthAttachment;

                Texture* texture = checked_cast<Texture*>(att.texture);
                assert(fb->framebufferInfo.width == std::max(texture->desc.width >> att.subresources.baseMipLevel, 1u));
                assert(fb->framebufferInfo.height == std::max(texture->desc.height >> att.subresources.baseMipLevel, 1u));

                VkImageLayout depthLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                if(desc.depthAttachment.isReadOnly)
                {
                    depthLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                }

                VkAttachmentDescription2 attachmentDescription2 = {};
                attachmentDescription2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
                attachmentDescription2.format = texture->imageInfo.format;
                attachmentDescription2.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                attachmentDescription2.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescription2.initialLayout = depthLayout;
                attachmentDescription2.finalLayout = depthLayout;

                VkAttachmentReference2 attachmentReference2{};
                attachmentReference2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                attachmentReference2.attachment = uint32_t(attachmentDescs.size() - 1);
                attachmentReference2.layout = depthLayout;

                TextureSubresourceSet subresources = att.subresources.resolve(texture->desc, true);
                TextureDimension dimension = getDimensionForFramebuffer(texture->desc.dimension, subresources.numArraySlices > 1);

                const auto& view = texture->getSubresourceView(subresources, dimension, att.format);
                attachmentViews.push_back(view.view);

                fb->resources.push_back(att.texture);

                if (numArraySlices)
                    assert(numArraySlices == subresources.numArraySlices);
                else
                    numArraySlices = subresources.numArraySlices;
            }

            VkSubpassDescription2 subpassDescription2 = {};
            subpassDescription2.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
            subpassDescription2.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescription2.colorAttachmentCount = uint32_t(desc.colorAttachments.size());
            subpassDescription2.pColorAttachments = colorAttachmentRefs.data();
            subpassDescription2.pDepthStencilAttachment = desc.depthAttachment.valid() ? &depthAttachmentRef : nullptr;

            //add vrs attachment
            //declare the structures here to avoid using pointers to out-of-scope objects in renderPassInfo further
            VkAttachmentReference2 vrsAttachmentRef = {};
            VkFragmentShadingRateAttachmentInfoKHR shadingRateAttachmentInfoKhr{};

            if(desc.shadingRateAttachment.valid())
            {
                const auto& vrsAttachment = desc.shadingRateAttachment;
                Texture* vrsTexture = checked_cast<Texture*>(vrsAttachment.texture);
                assert(vrsTexture->imageInfo.format == VK_FORMAT_R8_UINT);
                assert(vrsTexture->imageInfo.samples == VK_SAMPLE_COUNT_1_BIT);

                VkAttachmentDescription2 vrsAttachmentDesc = {};
                vrsAttachmentDesc.format = VK_FORMAT_R8_UINT;
                vrsAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
                vrsAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                vrsAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                vrsAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
                vrsAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

                attachmentDescs.push_back(vrsAttachmentDesc);

                TextureSubresourceSet subresources = vrsAttachment.subresources.resolve(vrsTexture->desc, true);
                TextureDimension dimension = getDimensionForFramebuffer(vrsTexture->desc.dimension, subresources.numArraySlices > 1);

                const auto& view = vrsTexture->getSubresourceView(subresources, dimension, vrsAttachment.format);
                attachmentViews.push_back(view.view);

                fb->resources.push_back(vrsAttachment.texture);

                if (numArraySlices)
                    assert(numArraySlices == subresources.numArraySlices);
                else
                    numArraySlices = subresources.numArraySlices;

                VkPhysicalDeviceFragmentShadingRatePropertiesKHR vkPhysicalDeviceFragmentShadingRatePropertiesKhr{};
                vkPhysicalDeviceFragmentShadingRatePropertiesKhr.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

                VkPhysicalDeviceProperties2 physicalDeviceProperties2 = {};
                physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
                physicalDeviceProperties2.pNext = &vkPhysicalDeviceFragmentShadingRatePropertiesKhr;

                vkGetPhysicalDeviceProperties2(m_Context.physicalDevice, &physicalDeviceProperties2);

                //auto rateProps = vk::PhysicalDeviceFragmentShadingRatePropertiesKHR();
                //auto props = vk::PhysicalDeviceProperties2();
                //props.pNext = &rateProps;
                //m_Context.physicalDevice.getProperties2(&props);

                VkAttachmentReference2 attachmentReference2{};
                attachmentReference2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                attachmentReference2.attachment = uint32_t (attachmentDescs.size()) - 1;
                attachmentReference2.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

                //vrsAttachmentRef = vk::AttachmentReference2()
                //        .setAttachment(uint32_t(attachmentDescs.size()) - 1)
                //        .setLayout(vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR);

                VkFragmentShadingRateAttachmentInfoKHR fragmentShadingRateAttachmentInfoKhr{};
                fragmentShadingRateAttachmentInfoKhr.sType = VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;
                fragmentShadingRateAttachmentInfoKhr.pFragmentShadingRateAttachment = &vrsAttachmentRef;
                fragmentShadingRateAttachmentInfoKhr.shadingRateAttachmentTexelSize = vkPhysicalDeviceFragmentShadingRatePropertiesKhr.minFragmentShadingRateAttachmentTexelSize;

                //shadingRateAttachmentInfo = vk::FragmentShadingRateAttachmentInfoKHR()
                //        .setPFragmentShadingRateAttachment(&vrsAttachmentRef)
                //        .setShadingRateAttachmentTexelSize(rateProps.minFragmentShadingRateAttachmentTexelSize);

                subpassDescription2.pNext = &shadingRateAttachmentInfoKhr;
            }

            VkRenderPassCreateInfo2 renderPassCreateInfo2 = {};
            renderPassCreateInfo2.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
            renderPassCreateInfo2.attachmentCount = uint32_t(attachmentDescs.size());
            renderPassCreateInfo2.pAttachments = attachmentDescs.data();
            renderPassCreateInfo2.subpassCount = 1;
            renderPassCreateInfo2.pSubpasses = &subpassDescription2;//subpass

            //todo:fix this
            PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR = (PFN_vkCreateRenderPass2KHR) vkGetInstanceProcAddr(m_Context.instance, "vkCreateRenderPass2KHR");
            VkResult result = vkCreateRenderPass2KHR(m_Context.device, &renderPassCreateInfo2, VK_NULL_HANDLE, &fb->renderPass);

            VK_CHECK(result);

            VkFramebufferCreateInfo vkFramebufferCreateInfo = {};
            vkFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            vkFramebufferCreateInfo.renderPass = fb->renderPass;
            vkFramebufferCreateInfo.attachmentCount = uint32_t(attachmentViews.size());
            vkFramebufferCreateInfo.pAttachments = attachmentViews.data();
            vkFramebufferCreateInfo.width = fb->framebufferInfo.width;
            vkFramebufferCreateInfo.height = fb->framebufferInfo.height;
            vkFramebufferCreateInfo.layers = numArraySlices;


            result = vkCreateFramebuffer(m_Context.device, &vkFramebufferCreateInfo, m_Context.allocationCallbacks, &fb->framebuffer);

            VK_CHECK(result);

            return FramebufferHandle::Create(fb);
        }

        GraphicsPipeline::~GraphicsPipeline()
        {
            if (pipeline)
            {
                vkDestroyPipeline(m_Context.device, pipeline, m_Context.allocationCallbacks);
                //m_Context.device.destroyPipeline(pipeline, m_Context.allocationCallbacks);
                pipeline = VK_NULL_HANDLE;
            }

            if (pipelineLayout)
            {
                vkDestroyPipelineLayout(m_Context.device, pipelineLayout, m_Context.allocationCallbacks);
                //m_Context.device.destroyPipelineLayout(pipelineLayout, m_Context.allocationCallbacks);
                pipelineLayout = VK_NULL_HANDLE;
            }
        }

        Object GraphicsPipeline::getNativeObject(ObjectType objectType) {
            switch (objectType) {
                case ObjectTypes::VK_PipelineLayout:
                    return Object(pipelineLayout);
                case ObjectTypes::VK_Pipeline:
                    return Object(pipeline);
                default:
                    return nullptr;
            }
        }

        GraphicsPipelineHandle
        Device::createGraphicsPipeline(const GraphicsPipelineDesc &desc, IFramebuffer *_fb) {
            if (desc.renderState.singlePassStereo.enabled)
            {
                m_Context.error("Single-pass stereo is not supported by the Vulkan backend");
                return nullptr;
            }

            //vk::Result res;
            VkResult res;
//
            Framebuffer* fb = checked_cast<Framebuffer*>(_fb);
//
            InputLayout* inputLayout = checked_cast<InputLayout*>(desc.inputLayout.Get());
//
            GraphicsPipeline *pso = new GraphicsPipeline(m_Context);
            pso->desc = desc;
            pso->framebufferInfo = fb->framebufferInfo;
//
            for (const BindingLayoutHandle& _layout : desc.bindingLayouts)
            {
                BindingLayout* layout = checked_cast<BindingLayout*>(_layout.Get());
                pso->pipelineBindingLayouts.push_back(layout);
            }
//
            Shader* VS = checked_cast<Shader*>(desc.VS.Get());
            Shader* HS = checked_cast<Shader*>(desc.HS.Get());
            Shader* DS = checked_cast<Shader*>(desc.DS.Get());
            Shader* GS = checked_cast<Shader*>(desc.GS.Get());
            Shader* PS = checked_cast<Shader*>(desc.PS.Get());
//
            size_t numShaders = 0;
            size_t numShadersWithSpecializations = 0;
            size_t numSpecializationConstants = 0;
//
            //// Count the spec constants for all stages
            countSpecializationConstants(VS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
            countSpecializationConstants(HS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
            countSpecializationConstants(DS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
            countSpecializationConstants(GS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
            countSpecializationConstants(PS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
//
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
            std::vector<VkSpecializationInfo> specInfos;
            std::vector<VkSpecializationMapEntry> specMapEntries;
            std::vector<uint32_t> specData;
//
            // Allocate buffers for specialization constants and related structures
            // so that shaderStageCreateInfo(...) can directly use pointers inside the vectors
            // because the vectors won't reallocate their buffers
            shaderStages.reserve(numShaders);
            specInfos.reserve(numShadersWithSpecializations);
            specMapEntries.reserve(numSpecializationConstants);
            specData.reserve(numSpecializationConstants);
//
            //// Set up shader stages
            if (desc.VS)
            {
                shaderStages.push_back(makeShaderStageCreateInfo(VS,
                                                                 specInfos, specMapEntries, specData));
                pso->shaderMask = pso->shaderMask | ShaderType::Vertex;
            }
//
            if (desc.HS)
            {
                shaderStages.push_back(makeShaderStageCreateInfo(HS,
                                                                 specInfos, specMapEntries, specData));
                pso->shaderMask = pso->shaderMask | ShaderType::Hull;
            }
//
            if (desc.DS)
            {
                shaderStages.push_back(makeShaderStageCreateInfo(DS,
                                                                 specInfos, specMapEntries, specData));
                pso->shaderMask = pso->shaderMask | ShaderType::Domain;
            }
//
            if (desc.GS)
            {
                shaderStages.push_back(makeShaderStageCreateInfo(GS,
                                                                 specInfos, specMapEntries, specData));
                pso->shaderMask = pso->shaderMask | ShaderType::Geometry;
            }
//
            if (desc.PS)
            {
                shaderStages.push_back(makeShaderStageCreateInfo(PS,
                                                                 specInfos, specMapEntries, specData));
                pso->shaderMask = pso->shaderMask | ShaderType::Pixel;
            }
//
            //// set up vertex input state
            VkPipelineVertexInputStateCreateInfo vertexInput = {};
            vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            if (inputLayout)
            {
                vertexInput.vertexBindingDescriptionCount = uint32_t(inputLayout->bindingDesc.size());
                vertexInput.pVertexBindingDescriptions = inputLayout->bindingDesc.data();
                vertexInput.vertexAttributeDescriptionCount = uint32_t(inputLayout->attributeDesc.size());
                vertexInput.pVertexAttributeDescriptions = inputLayout->attributeDesc.data();
                //vertexInput.setVertexBindingDescriptionCount(uint32_t(inputLayout->bindingDesc.size()))
                //        .setPVertexBindingDescriptions(inputLayout->bindingDesc.data())
                //        .setVertexAttributeDescriptionCount(uint32_t(inputLayout->attributeDesc.size()))
                //        .setPVertexAttributeDescriptions(inputLayout->attributeDesc.data());
            }
            //auto vertexInput = vk::PipelineVertexInputStateCreateInfo();
            //if (inputLayout)
            //{
            //    vertexInput.setVertexBindingDescriptionCount(uint32_t(inputLayout->bindingDesc.size()))
            //            .setPVertexBindingDescriptions(inputLayout->bindingDesc.data())
            //            .setVertexAttributeDescriptionCount(uint32_t(inputLayout->attributeDesc.size()))
            //            .setPVertexAttributeDescriptions(inputLayout->attributeDesc.data());
            //}
//
            VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = convertPrimitiveTopology(desc.primType);
            //auto inputAssembly = vk::PipelineInputAssemblyStateCreateInfo()
            //        .setTopology(convertPrimitiveTopology(desc.primType));
//
            //// fixed function state
            const auto& rasterState = desc.renderState.rasterState;
            const auto& depthStencilState = desc.renderState.depthStencilState;
            const auto& blendState = desc.renderState.blendState;
//
            VkPipelineViewportStateCreateInfo viewportState = {};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;
            //auto viewportState = vk::PipelineViewportStateCreateInfo()
            //        .setViewportCount(1)
            //        .setScissorCount(1);
//
            VkPipelineRasterizationStateCreateInfo rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.polygonMode = convertFillMode(rasterState.fillMode);
            rasterizer.cullMode = convertCullMode(rasterState.cullMode);
            //auto rasterizer = vk::PipelineRasterizationStateCreateInfo()
            rasterizer.frontFace = rasterState.frontCounterClockwise ?
                                   VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
            rasterizer.depthBiasEnable = rasterState.depthBias ? true : false;
            rasterizer.depthBiasConstantFactor = float(rasterState.depthBias);
            rasterizer.depthBiasClamp = rasterState.depthBiasClamp;
            rasterizer.depthBiasSlopeFactor = rasterState.slopeScaledDepthBias;
            rasterizer.lineWidth = 1.0f;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.depthClampEnable = VK_FALSE;
            //        // .setDepthClampEnable(??)
            //        // .setRasterizerDiscardEnable(??)
            //        .setPolygonMode(convertFillMode(rasterState.fillMode))
            //        .setCullMode(convertCullMode(rasterState.cullMode))
            //        .setFrontFace(rasterState.frontCounterClockwise ?
            //                      vk::FrontFace::eCounterClockwise : vk::FrontFace::eClockwise)
            //        .setDepthBiasEnable(rasterState.depthBias ? true : false)
            //        .setDepthBiasConstantFactor(float(rasterState.depthBias))
            //        .setDepthBiasClamp(rasterState.depthBiasClamp)
            //        .setDepthBiasSlopeFactor(rasterState.slopeScaledDepthBias)
            //        .setLineWidth(1.0f);
//
            //// Conservative raster state
            VkPipelineRasterizationConservativeStateCreateInfoEXT conservativeStateCreate = {};
            conservativeStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT;
            conservativeStateCreate.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;
            if(rasterState.conservativeRasterEnable)
            {
                rasterizer.pNext = &conservativeStateCreate;
            }
            //auto conservativeRasterState = vk::PipelineRasterizationConservativeStateCreateInfoEXT()
            //        .setConservativeRasterizationMode(vk::ConservativeRasterizationModeEXT::eOverestimate);
            //if (rasterState.conservativeRasterEnable)
            //{
            //    rasterizer.setPNext(&conservativeRasterState);
            //}
//
            VkPipelineMultisampleStateCreateInfo multisample = {};
            multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            VkSampleCountFlagBits flagBits = (VkSampleCountFlagBits)fb->framebufferInfo.sampleCount;
            multisample.rasterizationSamples = flagBits;
            multisample.alphaToCoverageEnable = blendState.alphaToCoverageEnable;
            //auto multisample = vk::PipelineMultisampleStateCreateInfo()
            //        .setRasterizationSamples(vk::SampleCountFlagBits(fb->framebufferInfo.sampleCount))
            //        .setAlphaToCoverageEnable(blendState.alphaToCoverageEnable);
//
            VkPipelineDepthStencilStateCreateInfo depthStencil = {};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = depthStencilState.depthTestEnable;
            depthStencil.depthWriteEnable = depthStencilState.depthWriteEnable;
            depthStencil.depthCompareOp = convertCompareOp(depthStencilState.depthFunc);
            depthStencil.stencilTestEnable = depthStencilState.stencilEnable;
            depthStencil.front = convertStencilState(depthStencilState, depthStencilState.frontFaceStencil);
            depthStencil.back = convertStencilState(depthStencilState, depthStencilState.backFaceStencil);


            //auto depthStencil = vk::PipelineDepthStencilStateCreateInfo()
            //        .setDepthTestEnable(depthStencilState.depthTestEnable)
            //        .setDepthWriteEnable(depthStencilState.depthWriteEnable)
            //        .setDepthCompareOp(convertCompareOp(depthStencilState.depthFunc))
            //        .setStencilTestEnable(depthStencilState.stencilEnable)
            //        .setFront(convertStencilState(depthStencilState, depthStencilState.frontFaceStencil))
            //        .setBack(convertStencilState(depthStencilState, depthStencilState.backFaceStencil));
//
            //// VRS state
            std::array<VkFragmentShadingRateCombinerOpKHR, 2> combiners = { convertShadingRateCombiner(desc.shadingRateState.pipelinePrimitiveCombiner), convertShadingRateCombiner(desc.shadingRateState.imageCombiner) };

            VkPipelineFragmentShadingRateStateCreateInfoKHR shadingRateState = {};
            shadingRateState.sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR;
            shadingRateState.combinerOps[0] = combiners[0];
            shadingRateState.combinerOps[1] = combiners[1];
            shadingRateState.fragmentSize = convertFragmentShadingRate(desc.shadingRateState.shadingRate);

            //auto shadingRateState = vk::PipelineFragmentShadingRateStateCreateInfoKHR()
            //        .setCombinerOps(combiners)
            //        .setFragmentSize(convertFragmentShadingRate(desc.shadingRateState.shadingRate));
//
            BindingVector<VkDescriptorSetLayout> descriptorSetLayouts;
            uint32_t pushConstantSize = 0;
            VkShaderStageFlagBits shaderStageFlagBits = VkShaderStageFlagBits(0);//todo:fix this
            pso->pushConstantVisibility = shaderStageFlagBits;
            for (const BindingLayoutHandle& _layout : desc.bindingLayouts)
            {
                BindingLayout* layout = checked_cast<BindingLayout*>(_layout.Get());
                descriptorSetLayouts.push_back(layout->descriptorSetLayout);

                if (!layout->isBindless)
                {
                    for (const BindingLayoutItem& item : layout->desc.bindings)
                    {
                        if (item.type == ResourceType::PushConstants)
                        {
                            pushConstantSize = item.size;
                            pso->pushConstantVisibility = convertShaderTypeToShaderStageFlagBits(layout->desc.visibility);
                            // assume there's only one push constant item in all layouts -- the validation layer makes sure of that
                            break;
                        }
                    }
                }
            }
//
            VkPushConstantRange pushConstantRange = {};
            pushConstantRange.offset = 0;
            pushConstantRange.size = pushConstantSize;
            pushConstantRange.stageFlags = pso->pushConstantVisibility;

            VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
            pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
            pipelineLayoutInfo.pushConstantRangeCount = pushConstantSize ? 1 : 0;
            pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

            //auto pushConstantRange = vk::PushConstantRange()
            //        .setOffset(0)
            //        .setSize(pushConstantSize)
            //        .setStageFlags(pso->pushConstantVisibility);
//
            //auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
            //        .setSetLayoutCount(uint32_t(descriptorSetLayouts.size()))
            //        .setPSetLayouts(descriptorSetLayouts.data())
            //        .setPushConstantRangeCount(pushConstantSize ? 1 : 0)
            //        .setPPushConstantRanges(&pushConstantRange);
//
            res = vkCreatePipelineLayout(m_Context.device, &pipelineLayoutInfo, m_Context.allocationCallbacks, &pso->pipelineLayout);
            //res = m_Context.device.createPipelineLayout(&pipelineLayoutInfo,
            //                                            m_Context.allocationCallbacks,
            //                                            &pso->pipelineLayout);
            //CHECK_VK_FAIL(res)
            VK_CHECK(res);
//
            attachment_vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(fb->desc.colorAttachments.size());
//
            for(uint32_t i = 0; i < uint32_t(fb->desc.colorAttachments.size()); i++)
            {
                colorBlendAttachments[i] = convertBlendState(blendState.targets[i]);
            }
//
            VkPipelineColorBlendStateCreateInfo colorBlend = {};
            colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlend.attachmentCount = uint32_t(colorBlendAttachments.size());
            colorBlend.pAttachments = colorBlendAttachments.data();
            //auto colorBlend = vk::PipelineColorBlendStateCreateInfo()
            //        .setAttachmentCount(uint32_t(colorBlendAttachments.size()))
            //        .setPAttachments(colorBlendAttachments.data());
//
            pso->usesBlendConstants = blendState.usesConstantColor(uint32_t(fb->desc.colorAttachments.size()));
//
            VkDynamicState dynamicStates[4] = {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR,
                    //VK_DYNAMIC_STATE_BLEND_CONSTANTS,
                    VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR
            };
//
            VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
            dynamicStateInfo.dynamicStateCount = pso->usesBlendConstants ? 3 : 2;
            dynamicStateInfo.pDynamicStates = dynamicStates;
            dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            //auto dynamicStateInfo = vk::PipelineDynamicStateCreateInfo()
            //        .setDynamicStateCount(pso->usesBlendConstants ? 3 : 2)
            //        .setPDynamicStates(dynamicStates);
//
            VkGraphicsPipelineCreateInfo pipelineInfo = {};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = uint32_t(shaderStages.size());
            pipelineInfo.pStages = shaderStages.data();
            pipelineInfo.pVertexInputState = &vertexInput;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisample;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlend;
            pipelineInfo.pDynamicState = &dynamicStateInfo;
            pipelineInfo.layout = pso->pipelineLayout;
            pipelineInfo.renderPass = fb->renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex = -1;
            pipelineInfo.pTessellationState = VK_NULL_HANDLE;
            //pipelineInfo.pNext = &shadingRateState;
            //auto pipelineInfo = vk::GraphicsPipelineCreateInfo()
            //        .setStageCount(uint32_t(shaderStages.size()))
            //        .setPStages(shaderStages.data())
            //        .setPVertexInputState(&vertexInput)
            //        .setPInputAssemblyState(&inputAssembly)
            //        .setPViewportState(&viewportState)
            //        .setPRasterizationState(&rasterizer)
            //        .setPMultisampleState(&multisample)
            //        .setPDepthStencilState(&depthStencil)
            //        .setPColorBlendState(&colorBlend)
            //        .setPDynamicState(&dynamicStateInfo)
            //        .setLayout(pso->pipelineLayout)
            //        .setRenderPass(fb->renderPass)
            //        .setSubpass(0)
            //        .setBasePipelineHandle(nullptr)
            //        .setBasePipelineIndex(-1)
            //        .setPTessellationState(nullptr)
            //        .setPNext(&shadingRateState);
//
            VkPipelineTessellationStateCreateInfo tessellationState = {};
            tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
            //auto tessellationState = vk::PipelineTessellationStateCreateInfo();
//
            if (desc.primType == PrimitiveType::PatchList)
            {
                tessellationState.patchControlPoints = desc.patchControlPoints;
                //pipelineInfo.setPTessellationState(&tessellationState);
                pipelineInfo.pTessellationState = &tessellationState;
            }
//
            res = vkCreateGraphicsPipelines(m_Context.device, m_Context.pipelineCache,
                                                           1, &pipelineInfo,
                                                           m_Context.allocationCallbacks,
                                                           &pso->pipeline);
            //ASSERT_VK_OK(res); // for debugging
            //CHECK_VK_FAIL(res);
//
            VK_CHECK(res);
            return GraphicsPipelineHandle::Create(pso);
        }

        void CommandList::updateGraphicsVolatileBuffers()
        {
            if (m_AnyVolatileBufferWrites && m_CurrentGraphicsState.pipeline)
            {
                GraphicsPipeline* pso = checked_cast<GraphicsPipeline*>(m_CurrentGraphicsState.pipeline);

                bindBindingSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pso->pipelineLayout, m_CurrentGraphicsState.bindings);

                m_AnyVolatileBufferWrites = false;
            }
        }

        void CommandList::drawIndexed(const DrawArguments& args)
        {
            assert(m_CurrentCmdBuf);

            //todo:implement this function
            updateGraphicsVolatileBuffers();

            vkCmdDrawIndexed(m_CurrentCmdBuf->cmdBuf, args.vertexCount,
                             args.instanceCount,
                             args.startIndexLocation,
                             args.startVertexLocation,
                             args.startInstanceLocation);

        }

        void CommandList::setGraphicsState(const GraphicsState& state)
        {
            assert(m_CurrentCmdBuf);

            GraphicsPipeline* pso = checked_cast<GraphicsPipeline*>(state.pipeline);
            Framebuffer* fb = checked_cast<Framebuffer*>(state.framebuffer);

            if (m_EnableAutomaticBarriers)
            {
                trackResourcesAndBarriers(state);
            }

            //bool anyBarriers = this->anyBarriers();
            bool anyBarriers = this->anyBarriers();
            bool updatePipeline = false;

            if (m_CurrentGraphicsState.pipeline != state.pipeline)
            {
                vkCmdBindPipeline(m_CurrentCmdBuf->cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pso->pipeline);
                //m_CurrentCmdBuf->cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, pso->pipeline);

                m_CurrentCmdBuf->referencedResources.push_back(state.pipeline);
                updatePipeline = true;
            }

            if (m_CurrentGraphicsState.framebuffer != state.framebuffer || anyBarriers /* because barriers cannot be set inside a renderpass */)
            {
                endRenderPass();
            }

            auto desc = state.framebuffer->getDesc();
            if (desc.shadingRateAttachment.valid())
            {
                setTextureState(desc.shadingRateAttachment.texture, nvrhi::TextureSubresourceSet(0, 1, 0, 1), nvrhi::ResourceStates::ShadingRateSurface);
            }

            commitBarriers();
//
            if(!m_CurrentGraphicsState.framebuffer)
            {
                VkRenderPassBeginInfo renderPassBeginInfo = {};

                renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassBeginInfo.renderPass = fb->renderPass;
                renderPassBeginInfo.framebuffer = fb->framebuffer;
                VkRect2D rect2D = {};
                rect2D.offset.x = 0;
                rect2D.offset.y = 0;
                rect2D.extent.width = fb->framebufferInfo.width;
                rect2D.extent.height = fb->framebufferInfo.height;
                renderPassBeginInfo.renderArea = rect2D;
                //VkClearValue clearValue = {};
                //clearValue.color = { { 0.0f, 0.0f, 0.6, 1.0f } };
                //renderPassBeginInfo.pClearValues = &clearValue;
                renderPassBeginInfo.clearValueCount = 0;
                vkCmdBeginRenderPass(m_CurrentCmdBuf->cmdBuf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


                //m_CurrentCmdBuf->cmdBuf.beginRenderPass(vk::RenderPassBeginInfo()
                //                                                .setRenderPass(fb->renderPass)
                //                                                .setFramebuffer(fb->framebuffer)
                //                                                .setRenderArea(vk::Rect2D()
                //                                                                       .setOffset(vk::Offset2D(0, 0))
                //                                                                       .setExtent(vk::Extent2D(fb->framebufferInfo.width, fb->framebufferInfo.height)))
                //                                                .setClearValueCount(0),
                //                                        vk::SubpassContents::eInline);

                m_CurrentCmdBuf->referencedResources.push_back(state.framebuffer);
            }
//
            m_CurrentPipelineLayout = pso->pipelineLayout;
            m_CurrentPushConstantsVisibility = pso->pushConstantVisibility;
//
            //if (arraysAreDifferent(m_CurrentComputeState.bindings, state.bindings) || m_AnyVolatileBufferWrites)
            if(arraysAreDifferent(m_CurrentGraphicsState.bindings, state.bindings) || m_AnyVolatileBufferWrites) //todo:fix this
            {
                //GuGu_LOGD("volatile buffer write");
                bindBindingSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pso->pipelineLayout, state.bindings);
            }
//
            if (!state.viewport.viewports.empty() && arraysAreDifferent(state.viewport.viewports, m_CurrentGraphicsState.viewport.viewports))
            {
                nvrhi::static_vector<VkViewport, c_MaxViewports> viewports;
                for (const auto& vp : state.viewport.viewports)
                {
                    viewports.push_back(VKViewportWithDXCoords(vp));
                }
//
                vkCmdSetViewport(m_CurrentCmdBuf->cmdBuf, 0, uint32_t(viewports.size()), viewports.data());
                //m_CurrentCmdBuf->cmdBuf.setViewport(0, uint32_t(viewports.size()), viewports.data());
            }
//
            if (!state.viewport.scissorRects.empty() && arraysAreDifferent(state.viewport.scissorRects, m_CurrentGraphicsState.viewport.scissorRects))
            {
                nvrhi::static_vector<VkRect2D, c_MaxViewports> scissors;
                for (const auto& sc : state.viewport.scissorRects)
                {
                    VkRect2D rect2D = {};
                    rect2D.offset.x = sc.minX;
                    rect2D.offset.y = sc.minY;

                    rect2D.extent.width = std::abs(sc.maxX - sc.minX);
                    rect2D.extent.height = std::abs(sc.maxY - sc.minY);

                    scissors.push_back(rect2D);

                    //scissors.push_back(vk::Rect2D(vk::Offset2D(sc.minX, sc.minY),
                    //                              vk::Extent2D(std::abs(sc.maxX - sc.minX), std::abs(sc.maxY - sc.minY))));
                }
//
                vkCmdSetScissor(m_CurrentCmdBuf->cmdBuf, 0, uint32_t(scissors.size()), scissors.data());
                //m_CurrentCmdBuf->cmdBuf.setScissor(0, uint32_t(scissors.size()), scissors.data());
            }
//
            if (pso->usesBlendConstants && (updatePipeline || m_CurrentGraphicsState.blendConstantColor != state.blendConstantColor))
            {
                vkCmdSetBlendConstants(m_CurrentCmdBuf->cmdBuf, &state.blendConstantColor.r);
                //m_CurrentCmdBuf->cmdBuf.setBlendConstants(&state.blendConstantColor.r);
            }
//
            if (state.indexBuffer.buffer && m_CurrentGraphicsState.indexBuffer != state.indexBuffer)
            {
                vkCmdBindIndexBuffer(m_CurrentCmdBuf->cmdBuf, checked_cast<Buffer*>(state.indexBuffer.buffer)->buffer, state.indexBuffer.offset,
                                     state.indexBuffer.format == Format::R16_UINT ?
                                     VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
                //m_CurrentCmdBuf->cmdBuf.bindIndexBuffer(checked_cast<Buffer*>(state.indexBuffer.buffer)->buffer,
                //                                        state.indexBuffer.offset,
                //                                        state.indexBuffer.format == Format::R16_UINT ?
                //                                        vk::IndexType::eUint16 : vk::IndexType::eUint32);
//
                m_CurrentCmdBuf->referencedResources.push_back(state.indexBuffer.buffer);
            }
//
            if (!state.vertexBuffers.empty() && arraysAreDifferent(state.vertexBuffers, m_CurrentGraphicsState.vertexBuffers))
            {
                VkBuffer vertexBuffers[c_MaxVertexAttributes];
                VkDeviceSize vertexBufferOffsets[c_MaxVertexAttributes];
                uint32_t maxVbIndex = 0;
//
                for (const auto& binding : state.vertexBuffers)
                {
                    // This is tested by the validation layer, skip invalid slots here if VL is not used.
                    if (binding.slot >= c_MaxVertexAttributes)
                        continue;
//
                    vertexBuffers[binding.slot] = checked_cast<Buffer*>(binding.buffer)->buffer;
                    vertexBufferOffsets[binding.slot] = VkDeviceSize(binding.offset);
                    maxVbIndex = std::max(maxVbIndex, binding.slot);
//
                    m_CurrentCmdBuf->referencedResources.push_back(binding.buffer);
                }
//
                vkCmdBindVertexBuffers(m_CurrentCmdBuf->cmdBuf, 0, maxVbIndex + 1, vertexBuffers, vertexBufferOffsets);
                //m_CurrentCmdBuf->cmdBuf.bindVertexBuffers(0, maxVbIndex + 1, vertexBuffers, vertexBufferOffsets);
            }
//
            if (state.indirectParams)
            {
                m_CurrentCmdBuf->referencedResources.push_back(state.indirectParams);
            }
//
            //if (state.shadingRateState.enabled)
            //{
            //    vk::FragmentShadingRateCombinerOpKHR combiners[2] = { convertShadingRateCombiner(state.shadingRateState.pipelinePrimitiveCombiner), convertShadingRateCombiner(state.shadingRateState.imageCombiner) };
            //    vk::Extent2D shadingRate = convertFragmentShadingRate(state.shadingRateState.shadingRate);
            //    m_CurrentCmdBuf->cmdBuf.setFragmentShadingRateKHR(&shadingRate, combiners);
            //}
//
            m_CurrentGraphicsState = state;
            //m_CurrentComputeState = ComputeState();
            //m_CurrentMeshletState = MeshletState();
            //m_CurrentRayTracingState = rt::State();
            m_AnyVolatileBufferWrites = false;
        }

        void CommandList::endRenderPass()
        {
            //todo:add meshlet state frame buffer check
            if (m_CurrentGraphicsState.framebuffer)
            {
                //m_CurrentCmdBuf->cmdBuf.endRenderPass();
                vkCmdEndRenderPass(m_CurrentCmdBuf->cmdBuf);
                m_CurrentGraphicsState.framebuffer = VK_NULL_HANDLE;
                //m_CurrentMeshletState.framebuffer = nullptr;
            }
        }

        void CommandList::draw(const DrawArguments& args)
        {
            assert(m_CurrentCmdBuf);

            updateGraphicsVolatileBuffers();

            vkCmdDraw(m_CurrentCmdBuf->cmdBuf, args.vertexCount,
                      args.instanceCount,
                      args.startVertexLocation,
                      args.startInstanceLocation);

            //m_CurrentCmdBuf->cmdBuf.draw(args.vertexCount,
            //                             args.instanceCount,
            //                             args.startVertexLocation,
            //                             args.startInstanceLocation);
        }

        //void CommandList::updateGraphicsVolatileBuffers()
        //{
        //    if (m_AnyVolatileBufferWrites && m_CurrentGraphicsState.pipeline)
        //    {
        //        GraphicsPipeline* pso = checked_cast<GraphicsPipeline*>(m_CurrentGraphicsState.pipeline);
//
        //        bindBindingSets(vk::PipelineBindPoint::eGraphics, pso->pipelineLayout, m_CurrentGraphicsState.bindings);
//
        //        m_AnyVolatileBufferWrites = false;
        //    }
        //}
    }
}