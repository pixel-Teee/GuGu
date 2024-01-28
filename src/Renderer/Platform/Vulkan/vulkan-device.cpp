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

        const char *resultToString(VkResult result) {
            switch(result)
            {
                case VK_SUCCESS:
                    return "VK_SUCCESS";
                case VK_NOT_READY:
                    return "VK_NOT_READY";
                case VK_TIMEOUT:
                    return "VK_TIMEOUT";
                case VK_EVENT_SET:
                    return "VK_EVENT_SET";
                case VK_EVENT_RESET:
                    return "VK_EVENT_RESET";
                case VK_INCOMPLETE:
                    return "VK_INCOMPLETE";
                case VK_ERROR_OUT_OF_HOST_MEMORY:
                    return "VK_ERROR_OUT_OF_HOST_MEMORY";
                case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                    return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
                case VK_ERROR_INITIALIZATION_FAILED:
                    return "VK_ERROR_INITIALIZATION_FAILED";
                case VK_ERROR_DEVICE_LOST:
                    return "VK_ERROR_DEVICE_LOST";
                case VK_ERROR_MEMORY_MAP_FAILED:
                    return "VK_ERROR_MEMORY_MAP_FAILED";
                case VK_ERROR_LAYER_NOT_PRESENT:
                    return "VK_ERROR_LAYER_NOT_PRESENT";
                case VK_ERROR_EXTENSION_NOT_PRESENT:
                    return "VK_ERROR_EXTENSION_NOT_PRESENT";
                case VK_ERROR_FEATURE_NOT_PRESENT:
                    return "VK_ERROR_FEATURE_NOT_PRESENT";
                case VK_ERROR_INCOMPATIBLE_DRIVER:
                    return "VK_ERROR_INCOMPATIBLE_DRIVER";
                case VK_ERROR_TOO_MANY_OBJECTS:
                    return "VK_ERROR_TOO_MANY_OBJECTS";
                case VK_ERROR_FORMAT_NOT_SUPPORTED:
                    return "VK_ERROR_FORMAT_NOT_SUPPORTED";
                case VK_ERROR_FRAGMENTED_POOL:
                    return "VK_ERROR_FRAGMENTED_POOL";
                case VK_ERROR_UNKNOWN:
                    return "VK_ERROR_UNKNOWN";
                case VK_ERROR_OUT_OF_POOL_MEMORY:
                    return "VK_ERROR_OUT_OF_POOL_MEMORY";
                case VK_ERROR_INVALID_EXTERNAL_HANDLE:
                    return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
                case VK_ERROR_FRAGMENTATION:
                    return "VK_ERROR_FRAGMENTATION";
                case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
                    return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
                case VK_ERROR_SURFACE_LOST_KHR:
                    return "VK_ERROR_SURFACE_LOST_KHR";
                case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                    return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
                case VK_SUBOPTIMAL_KHR:
                    return "VK_SUBOPTIMAL_KHR";
                case VK_ERROR_OUT_OF_DATE_KHR:
                    return "VK_ERROR_OUT_OF_DATE_KHR";
                case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                    return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
                case VK_ERROR_VALIDATION_FAILED_EXT:
                    return "VK_ERROR_VALIDATION_FAILED_EXT";
                case VK_ERROR_INVALID_SHADER_NV:
                    return "VK_ERROR_INVALID_SHADER_NV";
                case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
                    return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
                case VK_ERROR_NOT_PERMITTED_EXT:
                    return "VK_ERROR_NOT_PERMITTED_EXT";
                case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
                    return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
                case VK_THREAD_IDLE_KHR:
                    return "VK_THREAD_IDLE_KHR";
                case VK_THREAD_DONE_KHR:
                    return "VK_THREAD_DONE_KHR";
                case VK_OPERATION_DEFERRED_KHR:
                    return "VK_OPERATION_DEFERRED_KHR";
                case VK_OPERATION_NOT_DEFERRED_KHR:
                    return "VK_OPERATION_NOT_DEFERRED_KHR";
                case VK_PIPELINE_COMPILE_REQUIRED_EXT:
                    return "VK_PIPELINE_COMPILE_REQUIRED_EXT";

                default: {
                    // Print the value into a static buffer - this is not thread safe but that shouldn't matter
                    static char buf[24];
                    snprintf(buf, sizeof(buf), "Unknown (%d)", result);
                    return buf;
                }
            }
        }

        Device::Device(const nvrhi::vulkan::DeviceDesc &desc)
        : m_Context(desc.instance, desc.physicalDevice, desc.device, reinterpret_cast<VkAllocationCallbacks*>(desc.allocationCallbacks))
        , m_Allocator(m_Context)
        , m_TimerQueryAllocator(desc.maxTimerQueries, true) //256
        {
            //todo:fix these
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

            //VkPhysicalDeviceProperties2 deviceProperties2;
            //deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

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

            //deviceProperties2.pNext = pNext;

            //vkGetPhysicalDeviceProperties2(m_Context.physicalDevice, &deviceProperties2);
            //m_Context.physicalDevice.getProperties2(&deviceProperties2);

            //m_Context.physicalDeviceProperties = deviceProperties2.properties;
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
            pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

            VkResult res = vkCreatePipelineCache(m_Context.device, &pipelineInfo, m_Context.allocationCallbacks,
                                  &m_Context.pipelineCache);
            //vk::Result res = m_Context.device.createPipelineCache(&pipelineInfo,
             //                                                     m_Context.allocationCallbacks,
                                                                 // &m_Context.pipelineCache);

            if (res != VK_SUCCESS)
            {
                m_Context.error("Failed to create the pipeline cache");
            }

            //vkDebugMarkerSetObjectTagEXT = PFN_vkDebugMarkerSetObjectTagEXT( vkGetDeviceProcAddr( m_Context.device, "vkDebugMarkerSetObjectTagEXT" ) );
            //vkDebugMarkerSetObjectNameEXT = PFN_vkDebugMarkerSetObjectNameEXT( vkGetDeviceProcAddr( m_Context.device, "vkDebugMarkerSetObjectNameEXT" ) );
            //vkCmdDebugMarkerBeginEXT = PFN_vkCmdDebugMarkerBeginEXT( vkGetDeviceProcAddr( m_Context.device, "vkCmdDebugMarkerBeginEXT" ) );
            //vkCmdDebugMarkerEndEXT = PFN_vkCmdDebugMarkerEndEXT( vkGetDeviceProcAddr( m_Context.device, "vkCmdDebugMarkerEndEXT" ) );
            //vkCmdDebugMarkerInsertEXT = PFN_vkCmdDebugMarkerInsertEXT( vkGetDeviceProcAddr( m_Context.device, "vkCmdDebugMarkerInsertEXT" ) );
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

        void VulkanContext::nameVKObject(const void *handle, VkDebugReportObjectTypeEXT objtype,
                                         const char *name) const {
            if (extensions.EXT_debug_marker && name && *name && handle)
            {
                VkDebugMarkerObjectNameInfoEXT info = {};
                info.objectType = objtype;
                info.object = reinterpret_cast<uint64_t>(handle);
                info.pObjectName = name;
                //auto info = vk::DebugMarkerObjectNameInfoEXT()
                //        .setObjectType(objtype)
                //        .setObject(reinterpret_cast<uint64_t>(handle))
                //        .setPObjectName(name);
#ifndef ANDROID
                vkDebugMarkerSetObjectNameEXT(device, &info); //todo:fix this
#endif
                //(void)device.debugMarkerSetObjectNameEXT(&info);
            }
        }

        void VulkanContext::warning(const GuGuUtf8Str &message) const {
            messageCallback->message(MessageSeverity::Warning, message.getStr());
        }

        void VulkanContext::error(const GuGuUtf8Str &message) const {
            messageCallback->message(MessageSeverity::Warning, message.getStr());
        }

    }
}