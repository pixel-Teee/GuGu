#include <pch.h>

#include "vulkan-backend.h"

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
    }
}