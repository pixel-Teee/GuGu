#include <pch.h>

#include "vulkan-backend.h"

#include <Renderer/state-tracking.h>

#include <sstream>

#include <Renderer/misc.h>

namespace GuGu{
    namespace nvrhi::vulkan{
        BufferHandle Device::createBuffer(const BufferDesc& desc)
        {
            if(desc.isVolatile && desc.maxVersions == 0)
                return nullptr;

            if(desc.isVolatile && !desc.isConstantBuffer)
                return nullptr;

            if(desc.byteSize == 0)
                return nullptr;//note:what this constraint

            Buffer* buffer = new Buffer(m_Context, m_Allocator);
            buffer->desc = desc;

            VkBufferUsageFlags usageFlags = {};
            usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

            if(desc.isVertexBuffer)
                usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

            if (desc.isIndexBuffer)
                usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

            if (desc.isDrawIndirectArgs)
                usageFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

            if (desc.isConstantBuffer)
                usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

            if (desc.structStride != 0 || desc.canHaveUAVs || desc.canHaveRawViews)
                usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

            if (desc.canHaveTypedViews)
                usageFlags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

            if (desc.canHaveTypedViews && desc.canHaveUAVs)
                usageFlags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;

            if (desc.isAccelStructBuildInput)
                usageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

            if (desc.isAccelStructStorage)
                usageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;

            if (desc.isShaderBindingTable)
                usageFlags |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;

            if (m_Context.extensions.buffer_device_address)
                usageFlags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

            uint64_t size = desc.byteSize;

            if(desc.isVolatile)
            {
                assert(!desc.isVirtual);

                uint64_t alignment = m_Context.physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;

                uint64_t atomSize = m_Context.physicalDeviceProperties.limits.nonCoherentAtomSize;
                alignment = std::max(alignment, atomSize);

                assert((alignment & (alignment - 1)) == 0); //check if it's a power of 2

                size = (size + alignment - 1) & ~(alignment - 1);
                buffer->desc.byteSize = size;

                size *= desc.maxVersions;

                buffer->versionTracking.resize(desc.maxVersions);
                std::fill(buffer->versionTracking.begin(), buffer->versionTracking.end(), 0);

                //to get the right memory type allocated
                buffer->desc.cpuAccess = CpuAccessMode::Write;
            }
            else if(desc.byteSize < 65536)
            {
                //vulkan allows for <= 64kb buffer updates to be done inline via vkCmdUpdateBuffer
                //but the data size must always be a multiple of 4
                //enlarge the buffer slightly to allow for this
                size = (size + 3) & ~3ull;
            }

            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.size = size;
            bufferInfo.usage = usageFlags;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

#if WIN32
            VkExternalMemoryHandleTypeFlags handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else
            VkExternalMemoryHandleTypeFlags handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
#endif
            VkExternalMemoryBufferCreateInfo externalBuffer = {};
            externalBuffer.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO;
            externalBuffer.handleTypes = handleType;

            if(desc.sharedResourceFlags == SharedResourceFlags::Shared)
                bufferInfo.pNext = &externalBuffer;

            VkResult res = vkCreateBuffer(m_Context.device, &bufferInfo, m_Context.allocationCallbacks, &buffer->buffer);

            VK_CHECK(res);

            m_Context.nameVKObject((void*)buffer->buffer, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, desc.debugName.getStr());

            if(!desc.isVirtual)
            {
                res = m_Allocator.allocateBufferMemory(buffer, (usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0);

                VK_CHECK(res);

                m_Context.nameVKObject((void*)buffer->memory, VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT, desc.debugName.getStr());

                if(desc.isVolatile)
                {
                    vkMapMemory(m_Context.device, buffer->memory, 0, size, 0, &buffer->mappedMemory);
                    assert(buffer->mappedMemory);
                }

                if(m_Context.extensions.buffer_device_address)
                {
                    VkBufferDeviceAddressInfo addressInfo = {};
                    addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
                    addressInfo.buffer = buffer->buffer;
                    //todo:fix this
                    PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) vkGetInstanceProcAddr(m_Context.instance, "vkGetBufferDeviceAddress");
                    buffer->deviceAddress = vkGetBufferDeviceAddress(m_Context.device, &addressInfo);
                }
                if (desc.sharedResourceFlags == SharedResourceFlags::Shared)
                {
#ifdef _WIN32
                    //buffer->sharedHandle = m_Context.device.getMemoryWin32HandleKHR({ buffer->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 });
                    //todo:implement this
#else
                    //todo:fix this
                    PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR = (PFN_vkGetMemoryFdKHR) vkGetInstanceProcAddr(m_Context.instance, "vkGetMemoryFdKHR");
                    //buffer->sharedHandle = (void*)(size_t)m_Context.device.getMemoryFdKHR({ buffer->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd });
                    VkMemoryGetFdInfoKHR memoryGetFdInfoKhr = {};
                    memoryGetFdInfoKhr.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR;
                    memoryGetFdInfoKhr.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
                    memoryGetFdInfoKhr.memory = buffer->memory;
                    int fd = 0;
                    vkGetMemoryFdKHR(m_Context.device, &memoryGetFdInfoKhr, &fd);
                    buffer->sharedHandle = (void*)(size_t)(fd);
#endif
                }
            }

            return BufferHandle::Create(buffer);
        }

        Buffer::~Buffer()
        {
            if (mappedMemory)
            {
                vkUnmapMemory(m_Context.device, memory);
                //m_Context.device.unmapMemory(memory);
                mappedMemory = VK_NULL_HANDLE;
            }

            for (auto&& iter : viewCache)
            {
                vkDestroyBufferView(m_Context.device, iter.second, m_Context.allocationCallbacks);
                //m_Context.device.destroyBufferView(iter.second, m_Context.allocationCallbacks);
            }

            viewCache.clear();

            if (managed)
            {
                assert(buffer != VK_NULL_HANDLE);

                vkDestroyBuffer(m_Context.device, buffer, m_Context.allocationCallbacks);
                //m_Context.device.destroyBuffer(buffer, m_Context.allocationCallbacks);
                buffer = VK_NULL_HANDLE;

                if (memory)
                {
                    m_Allocator.freeBufferMemory(this);
                    memory = VK_NULL_HANDLE;
                }
            }
        }

        Object Buffer::getNativeObject(ObjectType objectType) {
            switch (objectType)
            {
                case ObjectTypes::VK_Buffer:
                    return Object(buffer);
                case ObjectTypes::VK_DeviceMemory:
                    return Object(memory);
                case ObjectTypes::SharedHandle:
                    return Object(sharedHandle);
                default:
                    return nullptr;
            }
        }

        static uint64_t getQueueLastFinishedID(Device* device, CommandQueue queueIndex)
        {
            Queue* queue = device->getQueue(queueIndex);
            if (queue)
                return queue->getLastFinishedID();
            return 0;
        }

        void CommandList::writeVolatileBuffer(Buffer* buffer, const void* data, size_t dataSize)
        {
            VolatileBufferState& state = m_VolatileBufferStates[buffer];

            if (!state.initialized)
            {
                state.minVersion = int(buffer->desc.maxVersions);
                state.maxVersion = -1;
                state.initialized = true;
            }

            std::array<uint64_t, uint32_t(CommandQueue::Count)> queueCompletionValues = {
                    getQueueLastFinishedID(m_Device, CommandQueue::Graphics),
                    getQueueLastFinishedID(m_Device, CommandQueue::Compute),
                    getQueueLastFinishedID(m_Device, CommandQueue::Copy)
            };

            uint32_t searchStart = buffer->versionSearchStart;
            uint32_t maxVersions = buffer->desc.maxVersions;
            uint32_t version = 0;

            uint64_t originalVersionInfo = 0;

            // Since versionTracking[] can be accessed by multiple threads concurrently,
            // perform the search in a loop ending with compare_exchange until the exchange is successful.
            while (true)
            {
                bool found = false;

                // Search through the versions of this buffer, looking for either unused (0)
                // or submitted and already finished versions

                for (uint32_t searchIndex = 0; searchIndex < maxVersions; searchIndex++)
                {
                    version = searchIndex + searchStart;
                    version = (version >= maxVersions) ? (version - maxVersions) : version;

                    originalVersionInfo = buffer->versionTracking[version];

                    if (originalVersionInfo == 0)
                    {
                        // Previously unused version - definitely available
                        found = true;
                        break;
                    }

                    // Decode the bitfield
                    bool isSubmitted = (originalVersionInfo & c_VersionSubmittedFlag) != 0;
                    uint32_t queueIndex = uint32_t(originalVersionInfo >> c_VersionQueueShift) & c_VersionQueueMask;
                    uint64_t id = originalVersionInfo & c_VersionIDMask;

                    // If the version is in a recorded but not submitted command list,
                    // we can't use it. So, only compare the version ID for submitted CLs.
                    if (isSubmitted)
                    {
                        // Versions can potentially be used in CLs submitted to different queues.
                        // So we store the queue index and use look at the last finished CL in that queue.

                        if (queueIndex >= uint32_t(CommandQueue::Count))
                        {
                            // If the version points at an invalid queue, assume it's available. Signal the error too.
                            utils::InvalidEnum();
                            found = true;
                            break;
                        }

                        if (id <= queueCompletionValues[queueIndex])
                        {
                            // If the version was used in a completed CL, it's available.
                            found = true;
                            break;
                        }
                    }
                }

                if (!found)
                {
                    // Not enough versions - need to relay this information to the developer.
                    // This has to be a real message and not assert, because asserts only happen in the
                    // debug mode, and buffer versioning will behave differently in debug vs. release,
                    // or validation on vs. off, because it is timing related.

                    std::stringstream ss;
                    ss << "Volatile constant buffer " << utils::DebugNameToString(buffer->desc.debugName) <<
                       " has maxVersions = " << buffer->desc.maxVersions << ", which is insufficient.";

                    m_Context.error(ss.str());
                    return;
                }

                // Encode the current CL ID for this version of the buffer, in a "pending" state
                uint64_t newVersionInfo = (uint64_t(m_CommandListParameters.queueType) << c_VersionQueueShift) | (m_CurrentCmdBuf->recordingID);

                // Try to store the new version info, end the loop if we actually won this version, i.e. no other thread has claimed it
                if (buffer->versionTracking[version].compare_exchange_weak(originalVersionInfo, newVersionInfo))
                    break;
            }

            buffer->versionSearchStart = (version + 1 < maxVersions) ? (version + 1) : 0;

            // Store the current version and expand the version range in this CL
            state.latestVersion = int(version);
            state.minVersion = std::min(int(version), state.minVersion);
            state.maxVersion = std::max(int(version), state.maxVersion);

            // Finally, write the actual data
            void* hostData = (char*)buffer->mappedMemory + version * buffer->desc.byteSize;
            memcpy(hostData, data, dataSize);

            m_AnyVolatileBufferWrites = true;
        }

        void CommandList::writeBuffer(IBuffer* _buffer, const void *data, size_t dataSize, uint64_t destOffsetBytes)
        {
            Buffer* buffer = checked_cast<Buffer*>(_buffer);

            assert(dataSize <= buffer->desc.byteSize);

            assert(m_CurrentCmdBuf);

            endRenderPass();

            m_CurrentCmdBuf->referencedResources.push_back(buffer);

            if (buffer->desc.isVolatile)
            {
                assert(destOffsetBytes == 0);

                writeVolatileBuffer(buffer, data, dataSize);

                return;
            }

            const size_t commandBufferWriteLimit = 65536;

            if (dataSize <= commandBufferWriteLimit)
            {
                if (m_EnableAutomaticBarriers)
                {
                    requireBufferState(buffer, ResourceStates::CopyDest);
                }
                commitBarriers();

                int64_t remaining = dataSize;
                const char* base = (const char*)data;
                while (remaining > 0)
                {
                    // vulkan allows <= 64kb transfers via VkCmdUpdateBuffer
                    int64_t thisGulpSize = std::min(remaining, int64_t(commandBufferWriteLimit));

                    // we bloat the read size here past the incoming buffer since the transfer must be a multiple of 4; the extra garbage should never be used anywhere
                    thisGulpSize = (thisGulpSize + 3) & ~3ll;
                    vkCmdUpdateBuffer(m_CurrentCmdBuf->cmdBuf, buffer->buffer, destOffsetBytes + dataSize - remaining, thisGulpSize, &base[dataSize - remaining]);
                    //m_CurrentCmdBuf->cmdBuf.updateBuffer(buffer->buffer, destOffsetBytes + dataSize - remaining, thisGulpSize, &base[dataSize - remaining]);
                    remaining -= thisGulpSize;
                }
            }
            else
            {
                if (buffer->desc.cpuAccess != CpuAccessMode::Write)
                {
                    // use the upload manager
                    Buffer* uploadBuffer;
                    uint64_t uploadOffset;
                    void* uploadCpuVA;
                    m_UploadManager->suballocateBuffer(dataSize, &uploadBuffer, &uploadOffset, &uploadCpuVA, MakeVersion(m_CurrentCmdBuf->recordingID, m_CommandListParameters.queueType, false));

                    memcpy(uploadCpuVA, data, dataSize);

                    copyBuffer(buffer, destOffsetBytes, uploadBuffer, uploadOffset, dataSize);
                }
                else
                {
                    m_Context.error("Using writeBuffer on mappable buffers is invalid");
                }
            }
        }

        void *Device::mapBuffer(IBuffer* _buffer, CpuAccessMode flags, uint64_t offset, size_t size) const
        {
            Buffer* buffer = checked_cast<Buffer*>(_buffer);

            assert(flags != CpuAccessMode::None);

            // If the buffer has been used in a command list before, wait for that CL to complete
            if (buffer->lastUseCommandListID != 0)
            {
                auto& queue = m_Queues[uint32_t(buffer->lastUseQueue)];
                queue->waitCommandList(buffer->lastUseCommandListID, ~0ull);
            }

            VkAccessFlagBits accessFlags;

            switch(flags)
            {
                case CpuAccessMode::Read:
                    accessFlags = VK_ACCESS_HOST_READ_BIT;
                    break;

                case CpuAccessMode::Write:
                    accessFlags = VK_ACCESS_HOST_WRITE_BIT;
                    break;

                case CpuAccessMode::None:
                default:
                    utils::InvalidEnum();
                    break;
            }

            // TODO: there should be a barrier... But there can't be a command list here
            // buffer->barrier(cmd, vk::PipelineStageFlagBits::eHost, accessFlags);

            void* ptr = nullptr;
            VkResult res = vkMapMemory(m_Context.device, buffer->memory, offset, size, VkMemoryMapFlags(0), &ptr);
            //[[maybe_unused]] const vk::Result res = m_Context.device.mapMemory(buffer->memory, offset, size, vk::MemoryMapFlags(), &ptr);
            assert(res == VK_SUCCESS);

            return ptr;
        }

        void *Device::mapBuffer(IBuffer* _buffer, CpuAccessMode flags)
        {
            Buffer* buffer = checked_cast<Buffer*>(_buffer);

            return mapBuffer(buffer, flags, 0, buffer->desc.byteSize);
        }

        void Device::unmapBuffer(IBuffer* _buffer)
        {
            Buffer* buffer = checked_cast<Buffer*>(_buffer);

            vkUnmapMemory(m_Context.device, buffer->memory);
            //m_Context.device.unmapMemory(buffer->memory);

            // TODO: there should be a barrier
            // buffer->barrier(cmd, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferRead);
        }

        void CommandList::copyBuffer(IBuffer* _dest, uint64_t destOffsetBytes,
                                     IBuffer* _src, uint64_t srcOffsetBytes,
                                     uint64_t dataSizeBytes)
        {
            Buffer* dest = checked_cast<Buffer*>(_dest);
            Buffer* src = checked_cast<Buffer*>(_src);

            assert(destOffsetBytes + dataSizeBytes <= dest->desc.byteSize);
            assert(srcOffsetBytes + dataSizeBytes <= src->desc.byteSize);

            assert(m_CurrentCmdBuf);

            if (dest->desc.cpuAccess != CpuAccessMode::None)
                m_CurrentCmdBuf->referencedStagingBuffers.push_back(dest);
            else
                m_CurrentCmdBuf->referencedResources.push_back(dest);

            if (src->desc.cpuAccess != CpuAccessMode::None)
                m_CurrentCmdBuf->referencedStagingBuffers.push_back(src);
            else
                m_CurrentCmdBuf->referencedResources.push_back(src);

            if (m_EnableAutomaticBarriers)
            {
                requireBufferState(src, ResourceStates::CopySource);
                requireBufferState(dest, ResourceStates::CopyDest);
            }
            commitBarriers();

            VkBufferCopy copyRegion = {};
            copyRegion.size = dataSizeBytes;
            copyRegion.srcOffset = srcOffsetBytes;
            copyRegion.dstOffset = destOffsetBytes;

            //auto copyRegion = vk::BufferCopy()
            //        .setSize(dataSizeBytes)
            //        .setSrcOffset(srcOffsetBytes)
            //        .setDstOffset(destOffsetBytes);

            vkCmdCopyBuffer(m_CurrentCmdBuf->cmdBuf, src->buffer, dest->buffer, 1, &copyRegion);
            //m_CurrentCmdBuf->cmdBuf.copyBuffer(src->buffer, dest->buffer, { copyRegion });
        }
    }
}