#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{

    namespace nvrhi::vulkan{
        static VkMemoryPropertyFlags pickBufferMemoryProperties(const BufferDesc& d)
        {
            VkMemoryPropertyFlags flags{};

            switch(d.cpuAccess)
            {
                case CpuAccessMode::None:
                    flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                    break;
                case CpuAccessMode::Read:
                    flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
                    break;
                case CpuAccessMode::Write:
                    flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                    break;
            }

            return flags;
        }

		VkResult
			VulkanAllocator::allocateBufferMemory(Buffer* buffer, bool enableDeviceAddress) const {
			//figure out memory requirements
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(m_Context.device, buffer->buffer, &memRequirements);

			//allocate memory
			const bool enableMemoryExport = (buffer->desc.sharedResourceFlags & SharedResourceFlags::Shared) != 0;
			const VkResult res = allocateMemory(buffer, memRequirements, pickBufferMemoryProperties(buffer->desc), enableDeviceAddress, enableMemoryExport, VK_NULL_HANDLE, buffer->buffer);
			VK_CHECK(res);

			vkBindBufferMemory(m_Context.device, buffer->buffer, buffer->memory, 0);

			return VK_SUCCESS;
		}

		void VulkanAllocator::freeBufferMemory(Buffer* buffer) const {
			freeMemory(buffer);
		}

		VkResult VulkanAllocator::allocateTextureMemory(Texture* texture) const
		{
			// grab the image memory requirements
			VkMemoryRequirements memRequirements;
			//vk::MemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(m_Context.device, texture->image, &memRequirements);
			//m_Context.device.getImageMemoryRequirements(texture->image, &memRequirements);

			// allocate memory
			const VkMemoryPropertyFlags memProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			const bool enableDeviceAddress = false;
			const bool enableMemoryExport = (texture->desc.sharedResourceFlags & SharedResourceFlags::Shared) != 0;
			const VkResult res = allocateMemory(texture, memRequirements, memProperties, enableDeviceAddress, enableMemoryExport, texture->image, VK_NULL_HANDLE);
			//CHECK_VK_RETURN(res)

			VK_CHECK(res);

			//m_Context.device.bindImageMemory(texture->image, texture->memory, 0);

			vkBindImageMemory(m_Context.device, texture->image, texture->memory, 0);

			return VK_SUCCESS;
		}

        void VulkanAllocator::freeTextureMemory(Texture *texture) const {
            freeMemory(texture);
        }

		VkResult VulkanAllocator::allocateMemory(MemoryResource* res,
			VkMemoryRequirements memRequirements,
			VkMemoryPropertyFlags memPropertyFlags,
			bool enableDeviceAddress,
			bool enableExportMemory,
			VkImage dedicatedImage,
			VkBuffer dedicateBuffer) const {
			res->managed = true;
			//find a memory space that satisfies the requirements
			VkPhysicalDeviceMemoryProperties memProperties = {};
			vkGetPhysicalDeviceMemoryProperties(m_Context.physicalDevice, &memProperties);

			uint32_t memTypeIndex;
			for (memTypeIndex = 0; memTypeIndex < memProperties.memoryTypeCount; ++memTypeIndex)
			{
				if ((memRequirements.memoryTypeBits & (1 << memTypeIndex)) &&
					((memProperties.memoryTypes[memTypeIndex].propertyFlags & memPropertyFlags) == memPropertyFlags))
				{
					break;
				}
			}

			if (memTypeIndex == memProperties.memoryTypeCount)
			{
				return VK_ERROR_OUT_OF_DEVICE_MEMORY;
			}

			//allocate memory
			VkMemoryAllocateFlagsInfo allocFlags = {};
			allocFlags.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
			if (enableDeviceAddress)
				allocFlags.flags |= VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

			const void* pNext = &allocFlags;

			VkMemoryDedicatedAllocateInfo dedicatedAllocateInfo = {};
			dedicatedAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
			dedicatedAllocateInfo.image = dedicatedImage;
			dedicatedAllocateInfo.buffer = dedicateBuffer;
			dedicatedAllocateInfo.pNext = pNext;

			if (dedicatedImage || dedicateBuffer)
			{
				// append the VkMemoryDedicatedAllocateInfo structure to the chain
				pNext = &dedicatedAllocateInfo;
			}
#if WIN32
			VkExternalMemoryHandleTypeFlags handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else
			VkExternalMemoryHandleTypeFlags handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
#endif
			VkExportMemoryAllocateInfo exportInfo = {};
			exportInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
			exportInfo.handleTypes = handleType;
			exportInfo.pNext = pNext;

			if (enableExportMemory)
			{
				// Append the VkExportMemoryAllocateInfo structure to the chain
				pNext = &exportInfo;
			}

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = memTypeIndex;
			allocInfo.pNext = pNext;

			return vkAllocateMemory(m_Context.device, &allocInfo, m_Context.allocationCallbacks, &res->memory);
		}

        void VulkanAllocator::freeMemory(MemoryResource *res) const {
            assert(res->managed);

            //m_Context.device.freeMemory(res->memory, m_Context.allocationCallbacks);
            //res->memory = vk::DeviceMemory(nullptr);
            vkFreeMemory(m_Context.device, res->memory, m_Context.allocationCallbacks);
            res->memory = VK_NULL_HANDLE;
        }         
    }
}