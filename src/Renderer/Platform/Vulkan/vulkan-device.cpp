#include <pch.h>

#include "vulkan-constants.h"

#include "vulkan-backend.h"

#include <unordered_map>
#include <Core/GuGuUtf8Str.h>

namespace GuGu{
    namespace nvrhi::vulkan {
        DeviceHandle createDevice(const DeviceDesc &desc) {
            Device* device = new Device(desc);
            return DeviceHandle::Create(device);
        }

        Device::Device(const nvrhi::vulkan::DeviceDesc &desc)
        : m_Context(desc.instance, desc.physicalDevice, desc.device, reinterpret_cast<VkAllocationCallbacks*>(desc.allocationCallbacks))
        , m_Allocator(m_Context)
        , m_TimerQueryAllocator(desc.maxTimerQueries, true) //256
        {
            if(desc.graphicsQueue)
            {
                m_Queues[uint32_t(CommandQueue::Graphics)] = std::make_unique<Queue>(m_Context,
                                                                                     CommandQueue::Graphics, desc.graphicsQueue,
                                                                                     desc.graphicsQueueIndex);
            }

            if (desc.computeQueue)
            {
                m_Queues[uint32_t(CommandQueue::Compute)] = std::make_unique<Queue>(m_Context,
                                                                                    CommandQueue::Compute, desc.computeQueue, desc.computeQueueIndex);
            }

            if (desc.transferQueue)
            {
                m_Queues[uint32_t(CommandQueue::Copy)] = std::make_unique<Queue>(m_Context,
                                                                                 CommandQueue::Copy, desc.transferQueue, desc.transferQueueIndex);
            }

            //todo:add feature checking

            //maps vulkan extensions strings into the corresponding boolean flags in device
            const std::unordered_map<GuGuUtf8Str, bool*> extensionStringMap = {
                    //{ VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, &m_Context.extensions.KHR_synchronization2 },
                    { VK_KHR_MAINTENANCE1_EXTENSION_NAME, &m_Context.extensions.KHR_maintenance1 },
                    { VK_EXT_DEBUG_REPORT_EXTENSION_NAME, &m_Context.extensions.EXT_debug_report },
                    { VK_EXT_DEBUG_MARKER_EXTENSION_NAME, &m_Context.extensions.EXT_debug_marker },
                    //{ VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, &m_Context.extensions.KHR_acceleration_structure },
                    { VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, &m_Context.extensions.buffer_device_address },
                    //{ VK_KHR_RAY_QUERY_EXTENSION_NAME,&m_Context.extensions.KHR_ray_query },
                    //{ VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, &m_Context.extensions.KHR_ray_tracing_pipeline },
                    { VK_NV_MESH_SHADER_EXTENSION_NAME, &m_Context.extensions.NV_mesh_shader },
                    { VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, &m_Context.extensions.EXT_conservative_rasterization},
                    { VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, &m_Context.extensions.KHR_fragment_shading_rate },
                    //{ VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME, &m_Context.extensions.EXT_opacity_micromap },
                    //{ VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME, &m_Context.extensions.NV_ray_tracing_invocation_reorder },
            };

            //parse the extensions/layer lists and figure out which extensions are enabled
            for(size_t i = 0; i < desc.numInstanceExtensions; i++)
            {
                auto ext = extensionStringMap.find(desc.instanceExtensions[i]);
                if (ext != extensionStringMap.end())
                {
                    *(ext->second) = true;
                }
            }

            for(size_t i = 0; i < desc.numDeviceExtensions; i++)
            {
                auto ext = extensionStringMap.find(desc.deviceExtensions[i]);
                if (ext != extensionStringMap.end())
                {
                    *(ext->second) = true;
                }
            }

            // The Vulkan 1.2 way of enabling bufferDeviceAddress
            if (desc.bufferDeviceAddressSupported)
                m_Context.extensions.buffer_device_address = true;

            void* pNext = nullptr;
            //VkPhysicalDeviceAccelerationStructurePropertiesKHR accelStructProperties;
            //VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties;
            VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservativeRasterizationProperties;
            VkPhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateProperties;
            //VkPhysicalDeviceOpacityMicromapPropertiesEXT opacityMicromapProperties;
            //VkPhysicalDeviceRayTracingInvocationReorderPropertiesNV nvRayTracingInvocationReorderProperties;

            VkPhysicalDeviceProperties2 deviceProperties2;

            //if (m_Context.extensions.KHR_acceleration_structure)
            //{
            //    accelStructProperties.pNext = pNext;
            //    pNext = &accelStructProperties;
            //}

            //if (m_Context.extensions.KHR_ray_tracing_pipeline)
            //{
            //    rayTracingPipelineProperties.pNext = pNext;
            //    pNext = &rayTracingPipelineProperties;
            //}

            if (m_Context.extensions.KHR_fragment_shading_rate)
            {
                shadingRateProperties.pNext = pNext;
                pNext = &shadingRateProperties;
            }

            if (m_Context.extensions.EXT_conservative_rasterization)
            {
                conservativeRasterizationProperties.pNext = pNext;
                pNext = &conservativeRasterizationProperties;
            }

            //if (m_Context.extensions.EXT_opacity_micromap)
            //{
            //    opacityMicromapProperties.pNext = pNext;
            //    pNext = &opacityMicromapProperties;
            //}

            //if (m_Context.extensions.NV_ray_tracing_invocation_reorder)
            //{
            //    nvRayTracingInvocationReorderProperties.pNext = pNext;
            //    pNext = &nvRayTracingInvocationReorderProperties;
            //}

            deviceProperties2.pNext = pNext;

            vkGetPhysicalDeviceProperties2(m_Context.physicalDevice, &deviceProperties2);
            //m_Context.physicalDevice.getProperties2(&deviceProperties2);

            m_Context.physicalDeviceProperties = deviceProperties2.properties;
            //m_Context.accelStructProperties = accelStructProperties;
            //.rayTracingPipelineProperties = rayTracingPipelineProperties;
            m_Context.conservativeRasterizationProperties = conservativeRasterizationProperties;
            m_Context.shadingRateProperties = shadingRateProperties;
            //m_Context.opacityMicromapProperties = opacityMicromapProperties;
            //m_Context.nvRayTracingInvocationReorderProperties = nvRayTracingInvocationReorderProperties;
            m_Context.messageCallback = desc.errorCB;

            if (m_Context.extensions.EXT_opacity_micromap && !m_Context.extensions.KHR_synchronization2)
            {
                m_Context.warning(
                        "EXT_opacity_micromap is used without KHR_synchronization2 which is nessesary for OMM Array state transitions. Feature::RayTracingOpacityMicromap will be disabled.");
            }

            if (m_Context.extensions.KHR_fragment_shading_rate)
            {
                VkPhysicalDeviceFeatures2 deviceFeatures2;
                VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures;
                deviceFeatures2.pNext = &shadingRateFeatures;
                //m_Context.physicalDevice.getFeatures2(&deviceFeatures2);
                vkGetPhysicalDeviceFeatures2(m_Context.physicalDevice, &deviceFeatures2);
                m_Context.shadingRateFeatures = shadingRateFeatures;
            }

            //auto pipelineInfo = VkPipelineCacheCreateInfo();

            VkPipelineCacheCreateInfo pipelineInfo{};

            VkResult res = vkCreatePipelineCache(m_Context.device, &pipelineInfo, m_Context.allocationCallbacks,
                                  &m_Context.pipelineCache);
            //vk::Result res = m_Context.device.createPipelineCache(&pipelineInfo,
             //                                                     m_Context.allocationCallbacks,
                                                                 // &m_Context.pipelineCache);

            if (res != VK_SUCCESS)
            {
                m_Context.error("Failed to create the pipeline cache");
            }
        }

        Device::~Device() {
            //if (m_TimerQueryPool)
            //{
            //    m_Context.device.destroyQueryPool(m_TimerQueryPool);
            //    m_TimerQueryPool = vk::QueryPool();
            //}

            if (m_Context.pipelineCache)
            {
                vkDestroyPipelineCache(m_Context.device, m_Context.pipelineCache, m_Context.allocationCallbacks);
                //m_Context.device.destroyPipelineCache(m_Context.pipelineCache);
                m_Context.pipelineCache = VK_NULL_HANDLE;
            }
        }


    }
}