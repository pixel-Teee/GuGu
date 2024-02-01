#include <pch.h>

#include "vulkan-backend.h"

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
    }
}