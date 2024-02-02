#include <pch.h>

#include "vulkan-backend.h"
#include "vk_types.h"
#include <Renderer/misc.h>
#include <sstream>

namespace GuGu{
    namespace nvrhi::vulkan {
        static Texture::TextureSubresourceViewType getTextureViewType(Format bindingFormat, Format textureFormat)
        {
            Format format = (bindingFormat == Format::UNKNOWN) ? textureFormat : bindingFormat;

            const FormatInfo& formatInfo = getFormatInfo(format);

            if (formatInfo.hasDepth)
                return Texture::TextureSubresourceViewType::DepthOnly;
            else if (formatInfo.hasStencil)
                return Texture::TextureSubresourceViewType::StencilOnly;
            else
                return Texture::TextureSubresourceViewType::AllAspects;
        }

        BindingLayout::BindingLayout(const VulkanContext &context, const BindingLayoutDesc &_desc)
                : desc(_desc), isBindless(false), m_Context(context) {
            VkShaderStageFlagBits shaderStageFlags = convertShaderTypeToShaderStageFlagBits(
                    desc.visibility);

            // iterate over all binding types and add to map
            for (const BindingLayoutItem &binding: desc.bindings) {
                VkDescriptorType descriptorType;
                uint32_t descriptorCount = 1;
                uint32_t registerOffset;

                switch (binding.type) {
                    case ResourceType::Texture_SRV:
                        registerOffset = _desc.bindingOffsets.shaderResource;
                        descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                        break;

                    case ResourceType::Texture_UAV:
                        registerOffset = _desc.bindingOffsets.unorderedAccess;
                        descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                        break;

                    case ResourceType::TypedBuffer_SRV:
                        registerOffset = _desc.bindingOffsets.shaderResource;
                        descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
                        break;

                    case ResourceType::StructuredBuffer_SRV:
                    case ResourceType::RawBuffer_SRV:
                        registerOffset = _desc.bindingOffsets.shaderResource;
                        descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        break;

                    case ResourceType::TypedBuffer_UAV:
                        registerOffset = _desc.bindingOffsets.unorderedAccess;
                        descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
                        break;

                    case ResourceType::StructuredBuffer_UAV:
                    case ResourceType::RawBuffer_UAV:
                        registerOffset = _desc.bindingOffsets.unorderedAccess;
                        descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        break;

                    case ResourceType::ConstantBuffer:
                        registerOffset = _desc.bindingOffsets.constantBuffer;
                        descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case ResourceType::VolatileConstantBuffer:
                        registerOffset = _desc.bindingOffsets.constantBuffer;
                        descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                        break;

                    case ResourceType::Sampler:
                        registerOffset = _desc.bindingOffsets.sampler;
                        descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                        break;

                    case ResourceType::PushConstants:
                        // don't need any descriptors for the push constants, but the vulkanLayoutBindings array
                        // must match the binding layout items for further processing within nvrhi --
                        // so set descriptorCount to 0 instead of skipping it
                        registerOffset = _desc.bindingOffsets.constantBuffer;
                        descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        descriptorCount = 0;
                        break;

                    case ResourceType::RayTracingAccelStruct:
                        registerOffset = _desc.bindingOffsets.shaderResource;
                        descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
                        break;

                    case ResourceType::None:
                    case ResourceType::Count:
                    default:
                        utils::InvalidEnum();
                        continue;
                }

                const auto bindingLocation = registerOffset + binding.slot;



               //vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = vk::DescriptorSetLayoutBinding()
               //        .setBinding(bindingLocation)
               //        .setDescriptorCount(descriptorCount)
               //        .setDescriptorType(descriptorType)
               //        .setStageFlags(shaderStageFlags);

               VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
               descriptorSetLayoutBinding.binding = bindingLocation;
               descriptorSetLayoutBinding.descriptorCount = descriptorCount;
               descriptorSetLayoutBinding.descriptorType = descriptorType;
               descriptorSetLayoutBinding.stageFlags = shaderStageFlags;

               vulkanLayoutBindings.push_back(descriptorSetLayoutBinding);

               //vulkanLayoutBindings.push_back(descriptorSetLayoutBinding);
            }
        }

        BindingLayout::~BindingLayout() {
            if (descriptorSetLayout)
            {
                //m_Context.device.destroyDescriptorSetLayout(descriptorSetLayout, m_Context.allocationCallbacks);
                //descriptorSetLayout = vk::DescriptorSetLayout();
                vkDestroyDescriptorSetLayout(m_Context.device, descriptorSetLayout, m_Context.allocationCallbacks);
                descriptorSetLayout = VK_NULL_HANDLE;
            }
        }

        Object BindingLayout::getNativeObject(ObjectType objectType)
        {
            switch (objectType)
            {
                case ObjectTypes::VK_DescriptorSetLayout:
                    return Object(descriptorSetLayout);
                default:
                    return nullptr;
            }
        }

        VkResult BindingLayout::bake() {
            // create the descriptor set layout object

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
            descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutInfo.bindingCount = vulkanLayoutBindings.size();
            descriptorSetLayoutInfo.pBindings = vulkanLayoutBindings.data();

            //auto descriptorSetLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
            //        .setBindingCount(uint32_t(vulkanLayoutBindings.size()))
            //        .setPBindings(vulkanLayoutBindings.data());

            std::vector<VkDescriptorBindingFlags> bindFlag(vulkanLayoutBindings.size(), VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

            VkDescriptorSetLayoutBindingFlagsCreateInfo extendedInfo = {};
            extendedInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
            extendedInfo.bindingCount = (uint32_t)(vulkanLayoutBindings.size());
            extendedInfo.pBindingFlags = bindFlag.data();

            //auto extendedInfo = vk::DescriptorSetLayoutBindingFlagsCreateInfo()
            //        .setBindingCount(uint32_t(vulkanLayoutBindings.size()))
            //        .setPBindingFlags(bindFlag.data());

            if (isBindless)
            {
                descriptorSetLayoutInfo.pNext = &extendedInfo;
            }

            //const vk::Result res = m_Context.device.createDescriptorSetLayout(&descriptorSetLayoutInfo,
            //                                                                  m_Context.allocationCallbacks,
            //                                                                  &descriptorSetLayout);

            VkResult res = vkCreateDescriptorSetLayout(m_Context.device, &descriptorSetLayoutInfo, m_Context.allocationCallbacks, &descriptorSetLayout);

            //CHECK_VK_RETURN(res)

            VK_CHECK(res);

            // count the number of descriptors required per type
            std::unordered_map<VkDescriptorType, uint32_t> poolSizeMap;
            for (auto layoutBinding : vulkanLayoutBindings)
            {
                if (poolSizeMap.find(layoutBinding.descriptorType) == poolSizeMap.end())
                {
                    poolSizeMap[layoutBinding.descriptorType] = 0;
                }

                poolSizeMap[layoutBinding.descriptorType] += layoutBinding.descriptorCount;
            }

            // compute descriptor pool size info
            for (auto poolSizeIter : poolSizeMap)
            {
                if (poolSizeIter.second > 0)
                {
                    //descriptorPoolSizeInfo.push_back(vk::DescriptorPoolSize()
                    //                                         .setType(poolSizeIter.first)
                    //                                         .setDescriptorCount(poolSizeIter.second));
                    VkDescriptorPoolSize descriptorPoolSize = {};
                    descriptorPoolSize.type = poolSizeIter.first;
                    descriptorPoolSize.descriptorCount = poolSizeIter.second;
                    descriptorPoolSizeInfo.push_back(descriptorPoolSize);
                }
            }

            return VK_SUCCESS;
        }


        vulkan::BindingSet::~BindingSet() {
            if(descriptorPool)
            {
                vkDestroyDescriptorPool(m_Context.device, descriptorPool, m_Context.allocationCallbacks);
                descriptorPool = VK_NULL_HANDLE;
                descriptorSet = VK_NULL_HANDLE;
            }
        }

        Object BindingSet::getNativeObject(ObjectType objectType) {
            switch (objectType)
            {
                case ObjectTypes::VK_DescriptorPool:
                    return Object(descriptorPool);
                case ObjectTypes::VK_DescriptorSet:
                    return Object(descriptorSet);
                default:
                    return nullptr;
            }
        }

        BindingLayoutHandle Device::createBindingLayout(const BindingLayoutDesc &desc) {
            BindingLayout* ret = new BindingLayout(m_Context, desc);

            ret->bake();

            return BindingLayoutHandle::Create(ret);
        }

        BindingSetHandle
        Device::createBindingSet(const BindingSetDesc &desc, IBindingLayout *_layout) {
            BindingLayout* layout = checked_cast<BindingLayout*>(_layout);

            BindingSet* ret = new BindingSet(m_Context);
            ret->desc = desc;
            ret->layout = layout;

            const auto& descriptorSetLayout = layout->descriptorSetLayout;
            const auto& poolSizes = layout->descriptorPoolSizeInfo;

            // create descriptor pool to allocate a descriptor from
            VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
            descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptorPoolCreateInfo.poolSizeCount = uint32_t(poolSizes.size());
            descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
            descriptorPoolCreateInfo.maxSets = 1;

            //auto poolInfo = vk::DescriptorPoolCreateInfo()
            //        .setPoolSizeCount(uint32_t(poolSizes.size()))
            //        .setPPoolSizes(poolSizes.data())
            //        .setMaxSets(1);

            VkResult res = vkCreateDescriptorPool(m_Context.device, &descriptorPoolCreateInfo, m_Context.allocationCallbacks, &ret->descriptorPool);
            //vk::Result res = m_Context.device.createDescriptorPool(&poolInfo,
            //                                                       m_Context.allocationCallbacks,
                                                                   //&ret->descriptorPool);
            VK_CHECK(res);

            // create the descriptor set
            VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
            descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocateInfo.descriptorPool = ret->descriptorPool;
            descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
            descriptorSetAllocateInfo.descriptorSetCount = 1;

            //auto descriptorSetAllocInfo = vk::DescriptorSetAllocateInfo()
            //        .setDescriptorPool(ret->descriptorPool)
            //        .setDescriptorSetCount(1)
            //        .setPSetLayouts(&descriptorSetLayout);
//
            res = vkAllocateDescriptorSets(m_Context.device, &descriptorSetAllocateInfo, &ret->descriptorSet);
            VK_CHECK(res);

            // collect all of the descriptor write data
            static_vector<VkDescriptorImageInfo, 3> descriptorImageInfo;
            static_vector<VkDescriptorBufferInfo, 3> descriptorBufferInfo;
            static_vector<VkWriteDescriptorSet, 3> descriptorWriteInfo;
            static_vector<VkWriteDescriptorSetAccelerationStructureKHR, c_MaxBindingsPerLayout> accelStructWriteInfo;

            auto generateWriteDescriptorData =
                    // generates a vk::WriteDescriptorSet struct in descriptorWriteInfo
                    [&](uint32_t bindingLocation,
                        VkDescriptorType descriptorType,
                        VkDescriptorImageInfo *imageInfo,
                        VkDescriptorBufferInfo *bufferInfo,
                        VkBufferView *bufferView,
                        const void* pNext = nullptr)
                    {
                        VkWriteDescriptorSet writeDescriptorSet = {};
                        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptorSet.dstSet = ret->descriptorSet;
                        writeDescriptorSet.dstBinding = bindingLocation;
                        writeDescriptorSet.dstArrayElement = 0;
                        writeDescriptorSet.descriptorCount = 1;
                        writeDescriptorSet.descriptorType = descriptorType;
                        writeDescriptorSet.pImageInfo = imageInfo;
                        writeDescriptorSet.pBufferInfo = bufferInfo;
                        writeDescriptorSet.pTexelBufferView = bufferView;
                        writeDescriptorSet.pNext = pNext;

                        descriptorWriteInfo.push_back(
                                writeDescriptorSet
                        );
                    };

            for (size_t bindingIndex = 0; bindingIndex < 3; bindingIndex++)
            {
                const BindingSetItem& binding = desc.bindings[bindingIndex];
                const VkDescriptorSetLayoutBinding& layoutBinding = layout->vulkanLayoutBindings[bindingIndex];

                if (binding.resourceHandle == nullptr)
                {
                    continue;
                }

                ret->resources.push_back(binding.resourceHandle); // keep a strong reference to the resource

                switch (binding.type)
                {
                    case ResourceType::Texture_SRV:
                    {
                        const auto texture = checked_cast<Texture *>(binding.resourceHandle);

                        const auto subresource = binding.subresources.resolve(texture->desc, false);
                        const auto textureViewType = getTextureViewType(binding.format, texture->desc.format);
                        auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);

                        auto& imageInfo = descriptorImageInfo.emplace_back();
                        VkDescriptorImageInfo descriptorImageInfo = {};
                        descriptorImageInfo.imageView = view.view;
                        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo = descriptorImageInfo;
                        //imageInfo = vk::DescriptorImageInfo()
                        //        .setImageView(view.view)
                        //        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

                        generateWriteDescriptorData(layoutBinding.binding,
                                                    layoutBinding.descriptorType,
                                                    &imageInfo, nullptr, nullptr);

                        if (!texture->permanentState)
                            ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                        else
                            verifyPermanentResourceState(texture->permanentState,
                                                         ResourceStates::ShaderResource,
                                                         true, texture->desc.debugName, m_Context.messageCallback);
                    }

                        break;

                    case ResourceType::Texture_UAV:
                    {
                        const auto texture = checked_cast<Texture *>(binding.resourceHandle);

                        const auto subresource = binding.subresources.resolve(texture->desc, true);
                        const auto textureViewType = getTextureViewType(binding.format, texture->desc.format);
                        auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);

                        auto& imageInfo = descriptorImageInfo.emplace_back();
                        VkDescriptorImageInfo descriptorImageInfo = {};
                        descriptorImageInfo.imageView = view.view;
                        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

                        imageInfo = descriptorImageInfo;

                        generateWriteDescriptorData(layoutBinding.binding,
                                                    layoutBinding.descriptorType,
                                                    &imageInfo, nullptr, nullptr);

                        if (!texture->permanentState)
                            ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                        else
                            verifyPermanentResourceState(texture->permanentState,
                                                         ResourceStates::UnorderedAccess,
                                                         true, texture->desc.debugName, m_Context.messageCallback);
                    }

                        break;

                    case ResourceType::TypedBuffer_SRV:
                    case ResourceType::TypedBuffer_UAV:
                    {
                        const auto buffer = checked_cast<Buffer *>(binding.resourceHandle);

                        assert(buffer->desc.canHaveTypedViews);
                        const bool isUAV = (binding.type == ResourceType::TypedBuffer_UAV);
                        if (isUAV)
                            assert(buffer->desc.canHaveUAVs);

                        Format format = binding.format;

                        if (format == Format::UNKNOWN)
                        {
                            format = buffer->desc.format;
                        }

                        auto vkformat = nvrhi::vulkan::convertFormat(format);
                        const auto range = binding.range.resolve(buffer->desc);

                        size_t viewInfoHash = 0;
                        nvrhi::hash_combine(viewInfoHash, range.byteOffset);
                        nvrhi::hash_combine(viewInfoHash, range.byteSize);
                        nvrhi::hash_combine(viewInfoHash, (uint64_t)vkformat);

                        const auto& bufferViewFound = buffer->viewCache.find(viewInfoHash);
                        auto& bufferViewRef = (bufferViewFound != buffer->viewCache.end()) ? bufferViewFound->second : buffer->viewCache[viewInfoHash];
                        if (bufferViewFound == buffer->viewCache.end())
                        {
                            assert(format != Format::UNKNOWN);

                            VkBufferViewCreateInfo bufferViewInfo = {};
                            bufferViewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
                            bufferViewInfo.buffer = buffer->buffer;
                            bufferViewInfo.offset = range.byteOffset;
                            bufferViewInfo.range = range.byteSize;
                            bufferViewInfo.format = vkformat;

                            //auto bufferViewInfo = vk::BufferViewCreateInfo()
                            //        .setBuffer(buffer->buffer)
                            //        .setOffset(range.byteOffset)
                            //        .setRange(range.byteSize)
                            //        .setFormat(vk::Format(vkformat));

                           res = vkCreateBufferView(m_Context.device, &bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef);
                            //res = m_Context.device.createBufferView(&bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef);
                            VK_CHECK(res);
                        }

                        generateWriteDescriptorData(layoutBinding.binding,
                                                    layoutBinding.descriptorType,
                                                    nullptr, nullptr, &bufferViewRef);

                        if (!buffer->permanentState)
                            ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                        else
                            verifyPermanentResourceState(buffer->permanentState,
                                                         isUAV ? ResourceStates::UnorderedAccess : ResourceStates::ShaderResource,
                                                         false, buffer->desc.debugName, m_Context.messageCallback);
                    }
                        break;

                    case ResourceType::StructuredBuffer_SRV:
                    case ResourceType::StructuredBuffer_UAV:
                    case ResourceType::RawBuffer_SRV:
                    case ResourceType::RawBuffer_UAV:
                    case ResourceType::ConstantBuffer:
                    case ResourceType::VolatileConstantBuffer:
                    {
                        const auto buffer = checked_cast<Buffer *>(binding.resourceHandle);

                        if (binding.type == ResourceType::StructuredBuffer_UAV || binding.type == ResourceType::RawBuffer_UAV)
                            assert(buffer->desc.canHaveUAVs);
                        if (binding.type == ResourceType::StructuredBuffer_UAV || binding.type == ResourceType::StructuredBuffer_SRV)
                            assert(buffer->desc.structStride != 0);
                        if (binding.type == ResourceType::RawBuffer_SRV|| binding.type == ResourceType::RawBuffer_UAV)
                            assert(buffer->desc.canHaveRawViews);

                        const auto range = binding.range.resolve(buffer->desc);

                        auto& bufferInfo = descriptorBufferInfo.emplace_back();
                        VkDescriptorBufferInfo descriptorBufferInfo = {};
                        descriptorBufferInfo.buffer = buffer->buffer;
                        descriptorBufferInfo.offset = range.byteOffset;
                        descriptorBufferInfo.range = range.byteSize;
                        bufferInfo = descriptorBufferInfo;
                        //bufferInfo = vk::DescriptorBufferInfo()
                        //        .setBuffer(buffer->buffer)
                        //        .setOffset(range.byteOffset)
                        //        .setRange(range.byteSize);

                        assert(buffer->buffer);
                        generateWriteDescriptorData(layoutBinding.binding,
                                                    layoutBinding.descriptorType,
                                                    nullptr, &bufferInfo, nullptr);

                        if (binding.type == ResourceType::VolatileConstantBuffer)
                        {
                            assert(buffer->desc.isVolatile);
                            ret->volatileConstantBuffers.push_back(buffer);
                        }
                        else
                        {
                            if (!buffer->permanentState)
                                ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                            else
                            {
                                ResourceStates requiredState;
                                if (binding.type == ResourceType::StructuredBuffer_UAV || binding.type == ResourceType::RawBuffer_UAV)
                                    requiredState = ResourceStates::UnorderedAccess;
                                else if (binding.type == ResourceType::ConstantBuffer)
                                    requiredState = ResourceStates::ConstantBuffer;
                                else
                                    requiredState = ResourceStates::ShaderResource;

                                verifyPermanentResourceState(buffer->permanentState, requiredState,
                                                             false, buffer->desc.debugName, m_Context.messageCallback);
                            }
                        }
                    }

                        break;

                    case ResourceType::Sampler:
                    {
                        //todo:implement this
                       const auto sampler = checked_cast<Sampler *>(binding.resourceHandle);
//
                       auto& imageInfo = descriptorImageInfo.emplace_back();
                       VkDescriptorImageInfo descriptorImageInfo = {};
                       descriptorImageInfo.sampler = sampler->sampler;
                       //imageInfo.sampler = sampler->sampler;
                       imageInfo = descriptorImageInfo;
                       //imageInfo = vk::DescriptorImageInfo()
                       //        .setSampler(sampler->sampler);
//
                        generateWriteDescriptorData(layoutBinding.binding,
                                                    layoutBinding.descriptorType,
                                                    &imageInfo, nullptr, nullptr);
                    }

                        break;

                    case ResourceType::RayTracingAccelStruct:
                    {
                        //const auto as = checked_cast<AccelStruct*>(binding.resourceHandle);
//
                        //auto& accelStructWrite = accelStructWriteInfo.emplace_back();
                        //accelStructWrite.accelerationStructureCount = 1;
                        //accelStructWrite.pAccelerationStructures = &as->accelStruct;
//
                        //generateWriteDescriptorData(layoutBinding.binding,
                        //                            layoutBinding.descriptorType,
                        //                            nullptr, nullptr, nullptr, &accelStructWrite);
//
                        //ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                    }

                        break;

                    case ResourceType::PushConstants:
                        break;

                    case ResourceType::None:
                    case ResourceType::Count:
                    default:
                        utils::InvalidEnum();
                        break;
                }
            }

            vkUpdateDescriptorSets(m_Context.device, uint32_t(descriptorWriteInfo.size()), descriptorWriteInfo.data(), 0, nullptr);
            //m_Context.device.updateDescriptorSets(uint32_t(descriptorWriteInfo.size()), descriptorWriteInfo.data(), 0, nullptr);

            return BindingSetHandle::Create(ret);
        }

        void CommandList::bindBindingSets(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, const BindingSetVector& bindings)
        {
            BindingVector<VkDescriptorSet> descriptorSets;
            static_vector<uint32_t, c_MaxVolatileConstantBuffers> dynamicOffsets;

            for (const auto& bindingSetHandle : bindings)
            {
                const BindingSetDesc* desc = bindingSetHandle->getDesc();
                if (desc)
                {
                    BindingSet* bindingSet = checked_cast<BindingSet*>(bindingSetHandle);
                    descriptorSets.push_back(bindingSet->descriptorSet);

                    for (Buffer* constnatBuffer : bindingSet->volatileConstantBuffers)
                    {
                        auto found = m_VolatileBufferStates.find(constnatBuffer);
                        if (found == m_VolatileBufferStates.end())
                        {
                            std::stringstream ss;
                            ss << "Binding volatile constant buffer " << utils::DebugNameToString(constnatBuffer->desc.debugName)
                               << " before writing into it is invalid.";
                            m_Context.error(ss.str());

                            dynamicOffsets.push_back(0); // use zero offset just to use something
                        }
                        else
                        {
                            uint32_t version = found->second.latestVersion;
                            uint64_t offset = version * constnatBuffer->desc.byteSize;
                            assert(offset < std::numeric_limits<uint32_t>::max());
                            dynamicOffsets.push_back(uint32_t(offset));
                        }
                    }

                    if (desc->trackLiveness)
                        m_CurrentCmdBuf->referencedResources.push_back(bindingSetHandle);
                }
                else
                {
                    //DescriptorTable* table = checked_cast<DescriptorTable*>(bindingSetHandle);
                    //descriptorSets.push_back(table->descriptorSet);
                }
            }

            if (!descriptorSets.empty())
            {
                vkCmdBindDescriptorSets(m_CurrentCmdBuf->cmdBuf, bindPoint, pipelineLayout, 0, uint32_t(descriptorSets.size()),
                                        descriptorSets.data(), uint32_t(dynamicOffsets.size()), dynamicOffsets.data());
                //m_CurrentCmdBuf->cmdBuf.bindDescriptorSets(bindPoint, pipelineLayout,
                //        /* firstSet = */ 0, uint32_t(descriptorSets.size()), descriptorSets.data(),
                //                                           uint32_t(dynamicOffsets.size()), dynamicOffsets.data());
            }
        }
    }
}