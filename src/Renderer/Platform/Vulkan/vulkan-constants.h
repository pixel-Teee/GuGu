#pragma once

#include <Renderer/nvrhi.h>

#include <Renderer/resource.h>

namespace GuGu{
    namespace nvrhi{
        class IMessageCallback;
    }
    namespace nvrhi::vulkan{
        VkFormat convertFormat(nvrhi::Format format);
        const char* resultToString(VkResult result);
        //typedef RefCountPtr<IDevice> DeviceHandle;

        struct DeviceDesc{
            //todo:add message call back
            IMessageCallback* errorCB = nullptr;
            VkInstance instance;
            VkPhysicalDevice physicalDevice;
            VkDevice device;

            //any of the queues can be null if this context doesn't intend to use them
            VkQueue graphicsQueue;
            int graphicsQueueIndex = -1;
            VkQueue transferQueue;
            int transferQueueIndex = -1;
            VkQueue computeQueue;
            int computeQueueIndex = -1;

            VkAllocationCallbacks *allocationCallbacks = nullptr;

            const char **instanceExtensions = nullptr;
            size_t numInstanceExtensions = 0;

            const char **deviceExtensions = nullptr;
            size_t numDeviceExtensions = 0;

            uint32_t maxTimerQueries = 256;

            // Indicates if VkPhysicalDeviceVulkan12Features::bufferDeviceAddress was set to 'true' at device creation time
            bool bufferDeviceAddressSupported = false;
        };

        class IDevice : public nvrhi::IDevice
        {
        public:
            // Additional Vulkan-specific public methods
            //virtual VkSemaphore getQueueSemaphore(CommandQueue queue) = 0;
            virtual void queueWaitForSemaphore(CommandQueue waitQueue, VkSemaphore semaphore, uint64_t value) = 0;
            //virtual void queueSignalSemaphore(CommandQueue executionQueue, VkSemaphore semaphore, uint64_t value) = 0;
            //virtual uint64_t queueGetCompletedInstance(CommandQueue queue) = 0;
            //virtual FramebufferHandle createHandleForNativeFramebuffer(VkRenderPass renderPass,
            //                                                           VkFramebuffer framebuffer, const FramebufferDesc& desc, bool transferOwnership) = 0;
        };

        typedef RefCountPtr<IDevice> DeviceHandle;

        DeviceHandle createDevice(const DeviceDesc& desc);
    }
}