#include <pch.h>

#include "vulkan-backend.h"

#include <Renderer/misc.h>

namespace GuGu{
    namespace nvrhi::vulkan{
        void CommandList::commitBarriers()
        {
            if (m_StateTracker.getBufferBarriers().empty() && m_StateTracker.getTextureBarriers().empty())
                return;

            endRenderPass();

            if (m_Context.extensions.KHR_synchronization2)
            {
                commitBarriersInternal_synchronization2();
            }
            else
            {
                commitBarriersInternal();
            }
        }

        void CommandList::commitBarriersInternal()
        {
            std::vector<VkImageMemoryBarrier> imageBarriers;
            std::vector<VkBufferMemoryBarrier> bufferBarriers;
            VkPipelineStageFlags beforeStageFlags = VkPipelineStageFlags(0);
            VkPipelineStageFlags afterStageFlags = VkPipelineStageFlags(0);

            for (const TextureBarrier& barrier : m_StateTracker.getTextureBarriers())
            {
                ResourceStateMapping before = convertResourceState(barrier.stateBefore);
                ResourceStateMapping after = convertResourceState(barrier.stateAfter);

                if ((before.stageFlags != beforeStageFlags || after.stageFlags != afterStageFlags) && !imageBarriers.empty())
                {
                    vkCmdPipelineBarrier(m_CurrentCmdBuf->cmdBuf, beforeStageFlags, afterStageFlags, 0, 0, nullptr, 0, nullptr, imageBarriers.size(), imageBarriers.data());

                    //m_CurrentCmdBuf->cmdBuf.pipelineBarrier(beforeStageFlags, afterStageFlags,
                    //                                        vk::DependencyFlags(), {}, {}, imageBarriers);

                    imageBarriers.clear();
                }

                beforeStageFlags = before.stageFlags;
                afterStageFlags = after.stageFlags;

                assert(after.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED);

                Texture* texture = static_cast<Texture*>(barrier.texture);

                const FormatInfo& formatInfo = getFormatInfo(texture->desc.format);

                VkImageAspectFlags aspectMask = (VkImageAspectFlagBits)0;
                if (formatInfo.hasDepth) aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
                if (formatInfo.hasStencil) aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
                if (!aspectMask) aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                VkImageSubresourceRange subresourceRange = {};
                subresourceRange.baseArrayLayer = barrier.entireTexture ? 0 : barrier.arraySlice;
                subresourceRange.layerCount = barrier.entireTexture ? texture->desc.arraySize : 1;
                subresourceRange.baseMipLevel = barrier.entireTexture ? 0 : barrier.mipLevel;
                subresourceRange.levelCount = barrier.entireTexture ? texture->desc.mipLevels : 1;
                subresourceRange.aspectMask = aspectMask;

                //vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
                //        .setBaseArrayLayer(barrier.entireTexture ? 0 : barrier.arraySlice)
                //        .setLayerCount(barrier.entireTexture ? texture->desc.arraySize : 1)
                //        .setBaseMipLevel(barrier.entireTexture ? 0 : barrier.mipLevel)
                //        .setLevelCount(barrier.entireTexture ? texture->desc.mipLevels : 1)
                //        .setAspectMask(aspectMask);

                VkImageMemoryBarrier imageMemoryBarrier = {};
                imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageMemoryBarrier.srcAccessMask = before.accessMask;
                imageMemoryBarrier.dstAccessMask = after.accessMask;
                imageMemoryBarrier.oldLayout = before.imageLayout;
                imageMemoryBarrier.newLayout = after.imageLayout;
                imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrier.image = texture->image;
                imageMemoryBarrier.subresourceRange = subresourceRange;
//
                //imageBarriers.push_back(vk::ImageMemoryBarrier()
                //                                .setSrcAccessMask(before.accessMask)
                //                                .setDstAccessMask(after.accessMask)
                //                                .setOldLayout(before.imageLayout)
                //                                .setNewLayout(after.imageLayout)
                //                                .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                //                                .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                //                                .setImage(texture->image)
                //                                .setSubresourceRange(subresourceRange));
            }

           if (!imageBarriers.empty())
           {
               //m_CurrentCmdBuf->cmdBuf.pipelineBarrier(beforeStageFlags, afterStageFlags,
               //                                        vk::DependencyFlags(), {}, {}, imageBarriers);
               vkCmdPipelineBarrier(m_CurrentCmdBuf->cmdBuf, beforeStageFlags, afterStageFlags, 0, 0, nullptr, 0, nullptr, imageBarriers.size(), imageBarriers.data());
           }

           beforeStageFlags = VkPipelineStageFlagBits(0);
           afterStageFlags = VkPipelineStageFlagBits(0);
           imageBarriers.clear();

           for (const BufferBarrier& barrier : m_StateTracker.getBufferBarriers())
           {
               ResourceStateMapping before = convertResourceState(barrier.stateBefore);
               ResourceStateMapping after = convertResourceState(barrier.stateAfter);

               if ((before.stageFlags != beforeStageFlags || after.stageFlags != afterStageFlags) && !bufferBarriers.empty())
               {
                   vkCmdPipelineBarrier(m_CurrentCmdBuf->cmdBuf, beforeStageFlags, afterStageFlags, 0, 0, nullptr, bufferBarriers.size(), bufferBarriers.data(), 0, nullptr);
                   //m_CurrentCmdBuf->cmdBuf.pipelineBarrier(beforeStageFlags, afterStageFlags,
                   //                                        vk::DependencyFlags(), {}, bufferBarriers, {});

                   bufferBarriers.clear();
               }

               beforeStageFlags = before.stageFlags;
               afterStageFlags = after.stageFlags;

               Buffer* buffer = static_cast<Buffer*>(barrier.buffer);

               VkBufferMemoryBarrier bufferMemoryBarrier = {};
               bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
               bufferMemoryBarrier.srcAccessMask = before.accessMask;
               bufferMemoryBarrier.dstAccessMask = after.accessMask;
               bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
               bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
               bufferMemoryBarrier.buffer = buffer->buffer;
               bufferMemoryBarrier.offset = 0;
               bufferMemoryBarrier.size = buffer->desc.byteSize;

               //bufferBarriers.push_back(vk::BufferMemoryBarrier()
               //                                 .setSrcAccessMask(before.accessMask)
               //                                 .setDstAccessMask(after.accessMask)
               //                                 .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
               //                                 .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
               //                                 .setBuffer(buffer->buffer)
               //                                 .setOffset(0)
               //                                 .setSize(buffer->desc.byteSize));
           }

           if (!bufferBarriers.empty())
           {
               vkCmdPipelineBarrier(m_CurrentCmdBuf->cmdBuf, beforeStageFlags, afterStageFlags, 0, 0, nullptr, bufferBarriers.size(), bufferBarriers.data(), 0, nullptr);
               //m_CurrentCmdBuf->cmdBuf.pipelineBarrier(beforeStageFlags, afterStageFlags,
               //                                        vk::DependencyFlags(), {}, bufferBarriers, {});
           }
           bufferBarriers.clear();

           m_StateTracker.clearBarriers();
        }

        void CommandList::commitBarriersInternal_synchronization2()
        {
            //std::vector<vk::ImageMemoryBarrier2> imageBarriers;
            //std::vector<vk::BufferMemoryBarrier2> bufferBarriers;
//
            //for (const TextureBarrier& barrier : m_StateTracker.getTextureBarriers())
            //{
            //    ResourceStateMapping2 before = convertResourceState2(barrier.stateBefore);
            //    ResourceStateMapping2 after = convertResourceState2(barrier.stateAfter);
//
            //    assert(after.imageLayout != vk::ImageLayout::eUndefined);
//
            //    Texture* texture = static_cast<Texture*>(barrier.texture);
//
            //    const FormatInfo& formatInfo = getFormatInfo(texture->desc.format);
//
            //    vk::ImageAspectFlags aspectMask = (vk::ImageAspectFlagBits)0;
            //    if (formatInfo.hasDepth) aspectMask |= vk::ImageAspectFlagBits::eDepth;
            //    if (formatInfo.hasStencil) aspectMask |= vk::ImageAspectFlagBits::eStencil;
            //    if (!aspectMask) aspectMask = vk::ImageAspectFlagBits::eColor;
//
            //    vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
            //            .setBaseArrayLayer(barrier.entireTexture ? 0 : barrier.arraySlice)
            //            .setLayerCount(barrier.entireTexture ? texture->desc.arraySize : 1)
            //            .setBaseMipLevel(barrier.entireTexture ? 0 : barrier.mipLevel)
            //            .setLevelCount(barrier.entireTexture ? texture->desc.mipLevels : 1)
            //            .setAspectMask(aspectMask);
//
            //    imageBarriers.push_back(vk::ImageMemoryBarrier2()
            //                                    .setSrcAccessMask(before.accessMask)
            //                                    .setDstAccessMask(after.accessMask)
            //                                    .setSrcStageMask(before.stageFlags)
            //                                    .setDstStageMask(after.stageFlags)
            //                                    .setOldLayout(before.imageLayout)
            //                                    .setNewLayout(after.imageLayout)
            //                                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            //                                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            //                                    .setImage(texture->image)
            //                                    .setSubresourceRange(subresourceRange));
            //}
//
            //if (!imageBarriers.empty())
            //{
            //    vk::DependencyInfo dep_info;
            //    dep_info.setImageMemoryBarriers(imageBarriers);
//
            //    m_CurrentCmdBuf->cmdBuf.pipelineBarrier2(dep_info);
            //}
//
            //imageBarriers.clear();
//
            //for (const BufferBarrier& barrier : m_StateTracker.getBufferBarriers())
            //{
            //    ResourceStateMapping2 before = convertResourceState2(barrier.stateBefore);
            //    ResourceStateMapping2 after = convertResourceState2(barrier.stateAfter);
//
            //    Buffer* buffer = static_cast<Buffer*>(barrier.buffer);
//
            //    bufferBarriers.push_back(vk::BufferMemoryBarrier2()
            //                                     .setSrcAccessMask(before.accessMask)
            //                                     .setDstAccessMask(after.accessMask)
            //                                     .setSrcStageMask(before.stageFlags)
            //                                     .setDstStageMask(after.stageFlags)
            //                                     .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            //                                     .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            //                                     .setBuffer(buffer->buffer)
            //                                     .setOffset(0)
            //                                     .setSize(buffer->desc.byteSize));
            //}
//
            //if (!bufferBarriers.empty())
            //{
            //    vk::DependencyInfo dep_info;
            //    dep_info.setBufferMemoryBarriers(bufferBarriers);
//
            //    m_CurrentCmdBuf->cmdBuf.pipelineBarrier2(dep_info);
            //}
            //bufferBarriers.clear();
//
            //m_StateTracker.clearBarriers();
        }

        void CommandList::beginTrackingTextureState(ITexture* _texture, TextureSubresourceSet subresources, ResourceStates stateBits)
        {
            Texture* texture = checked_cast<Texture*>(_texture);

            m_StateTracker.beginTrackingTextureState(texture, subresources, stateBits);
        }

        void CommandList::beginTrackingBufferState(IBuffer* _buffer, ResourceStates stateBits)
        {
            Buffer* buffer = checked_cast<Buffer*>(_buffer);

            m_StateTracker.beginTrackingBufferState(buffer, stateBits);
        }

        void CommandList::requireTextureState(ITexture* _texture, TextureSubresourceSet subresources, ResourceStates state)
        {
            Texture* texture = checked_cast<Texture*>(_texture);

            m_StateTracker.requireTextureState(texture, subresources, state);
        }

        void CommandList::requireBufferState(IBuffer* _buffer, ResourceStates state)
        {
            Buffer* buffer = checked_cast<Buffer*>(_buffer);

            m_StateTracker.requireBufferState(buffer, state);
        }

        void CommandList::setPermanentBufferState(IBuffer* _buffer, ResourceStates stateBits)
        {
            Buffer* buffer = checked_cast<Buffer*>(_buffer);

            m_StateTracker.endTrackingBufferState(buffer, stateBits, true);
        }

        void CommandList::flushVolatileBufferWrites() {
        // The volatile CBs are permanently mapped with the eHostVisible flag, but not eHostCoherent,
        // so before using the data on the GPU, we need to make sure it's available there.
        // Go over all the volatile CBs that were used in this CL and flush their written versions.

            std::vector<VkMappedMemoryRange> ranges;

            for (auto& iter : m_VolatileBufferStates)
            {
                Buffer* buffer = iter.first;
                VolatileBufferState& state = iter.second;

                if (state.maxVersion < state.minVersion || !state.initialized)
                    continue;

                // Flush all the versions between min and max - that might be too conservative,
                // but that should be fine - better than using potentially hundreds of ranges.
                int numVersions = state.maxVersion - state.minVersion + 1;

                //auto range = vk::MappedMemoryRange()
                //        .setMemory(buffer->memory)
                //        .setOffset(state.minVersion * buffer->desc.byteSize)
                //        .setSize(numVersions * buffer->desc.byteSize);
//
                VkMappedMemoryRange mappedMemoryRange = {};
                mappedMemoryRange.size = numVersions * buffer->desc.byteSize;
                mappedMemoryRange.offset = state.minVersion * buffer->desc.byteSize;
                mappedMemoryRange.memory = buffer->memory;
                ranges.push_back(mappedMemoryRange);
            }

            if (!ranges.empty())
            {
                vkFlushMappedMemoryRanges(m_Context.device, ranges.size(), ranges.data());
                //m_Context.device.flushMappedMemoryRanges(ranges);
            }
        }

        void CommandList::setResourceStatesForBindingSet(IBindingSet* _bindingSet)
        {
            if (_bindingSet->getDesc() == nullptr)
                return; // is bindless

            BindingSet* bindingSet = checked_cast<BindingSet*>(_bindingSet);

            for (auto bindingIndex : bindingSet->bindingsThatNeedTransitions)
            {
                const BindingSetItem& binding = bindingSet->desc.bindings[bindingIndex];

                switch(binding.type)  // NOLINT(clang-diagnostic-switch-enum)
                {
                    case ResourceType::Texture_SRV:
                        requireTextureState(checked_cast<ITexture*>(binding.resourceHandle), binding.subresources, ResourceStates::ShaderResource);
                        break;

                    case ResourceType::Texture_UAV:
                        requireTextureState(checked_cast<ITexture*>(binding.resourceHandle), binding.subresources, ResourceStates::UnorderedAccess);
                        break;

                    case ResourceType::TypedBuffer_SRV:
                    case ResourceType::StructuredBuffer_SRV:
                    case ResourceType::RawBuffer_SRV:
                        requireBufferState(checked_cast<IBuffer*>(binding.resourceHandle), ResourceStates::ShaderResource);
                        break;

                    case ResourceType::TypedBuffer_UAV:
                    case ResourceType::StructuredBuffer_UAV:
                    case ResourceType::RawBuffer_UAV:
                        requireBufferState(checked_cast<IBuffer*>(binding.resourceHandle), ResourceStates::UnorderedAccess);
                        break;

                    case ResourceType::ConstantBuffer:
                        requireBufferState(checked_cast<IBuffer*>(binding.resourceHandle), ResourceStates::ConstantBuffer);
                        break;

                    //case ResourceType::RayTracingAccelStruct:
                    //    requireBufferState(checked_cast<AccelStruct*>(binding.resourceHandle)->dataBuffer, ResourceStates::AccelStructRead);

                    default:
                        // do nothing
                        break;
                }
            }
        }

        void CommandList::trackResourcesAndBarriers(const GraphicsState &state) {
            assert(m_EnableAutomaticBarriers);

            if (arraysAreDifferent(state.bindings, m_CurrentGraphicsState.bindings))
            {
                for (size_t i = 0; i < state.bindings.size(); i++)
                {
                    setResourceStatesForBindingSet(state.bindings[i]);
                }
            }

            if (state.indexBuffer.buffer && state.indexBuffer.buffer != m_CurrentGraphicsState.indexBuffer.buffer)
            {
                requireBufferState(state.indexBuffer.buffer, ResourceStates::IndexBuffer);
            }
//
            if (arraysAreDifferent(state.vertexBuffers, m_CurrentGraphicsState.vertexBuffers))
            {
                for (const auto& vb : state.vertexBuffers)
                {
                    requireBufferState(vb.buffer, ResourceStates::VertexBuffer);
                }
            }
//
            if (m_CurrentGraphicsState.framebuffer != state.framebuffer)
            {
                setResourceStatesForFramebuffer(state.framebuffer);
            }
//
            //if (state.indirectParams && state.indirectParams != m_CurrentGraphicsState.indirectParams)
            //{
            //    requireBufferState(state.indirectParams, ResourceStates::IndirectArgument);
            //}
        }

        void CommandList::setTextureState(ITexture* _texture, TextureSubresourceSet subresources, ResourceStates stateBits)
        {
            Texture* texture = checked_cast<Texture*>(_texture);

            m_StateTracker.endTrackingTextureState(texture, subresources, stateBits, false);
        }

        void CommandList::setPermanentTextureState(ITexture* _texture, ResourceStates stateBits)
        {
            Texture* texture = checked_cast<Texture*>(_texture);

            m_StateTracker.endTrackingTextureState(texture, AllSubresources, stateBits, true);
        }
    }
}