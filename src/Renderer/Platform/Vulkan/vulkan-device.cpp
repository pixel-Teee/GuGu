#include <pch.h>

#include "vulkan-backend.h"

#include <Renderer/misc.h>

#include <Core/GuGuUtf8Str.h>

#include <unordered_map>

namespace GuGu{
    namespace nvrhi::vulkan {
        DeviceHandle createDevice(const DeviceDesc &desc) {
            Device* device = new Device(desc);
            return DeviceHandle::Create(device);
        }

		Device::Device(const nvrhi::vulkan::DeviceDesc& desc)
			: m_Context(desc.instance, desc.physicalDevice, desc.device, reinterpret_cast<VkAllocationCallbacks*>(desc.allocationCallbacks))
			, m_Allocator(m_Context)
			, m_TimerQueryAllocator(desc.maxTimerQueries, true) //256
		{
			//todo:fix these
			if (desc.graphicsQueue)
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
				//{ VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, &m_Context.extensions.buffer_device_address },
				//{ VK_KHR_RAY_QUERY_EXTENSION_NAME,&m_Context.extensions.KHR_ray_query },
				//{ VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, &m_Context.extensions.KHR_ray_tracing_pipeline },
				{ VK_NV_MESH_SHADER_EXTENSION_NAME, &m_Context.extensions.NV_mesh_shader },
				{ VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, &m_Context.extensions.EXT_conservative_rasterization},
				{ VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, &m_Context.extensions.KHR_fragment_shading_rate },
				//{ VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME, &m_Context.extensions.EXT_opacity_micromap },
				//{ VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME, &m_Context.extensions.NV_ray_tracing_invocation_reorder },
			};

			//parse the extensions/layer lists and figure out which extensions are enabled
			for (size_t i = 0; i < desc.numInstanceExtensions; i++)
			{
				auto ext = extensionStringMap.find(desc.instanceExtensions[i]);
				if (ext != extensionStringMap.end())
				{
					*(ext->second) = true;
				}
			}

			for (size_t i = 0; i < desc.numDeviceExtensions; i++)
			{
				auto ext = extensionStringMap.find(desc.deviceExtensions[i]);
				if (ext != extensionStringMap.end())
				{
					*(ext->second) = true;
				}
			}

			// The Vulkan 1.2 way of enabling bufferDeviceAddress
			//if (desc.bufferDeviceAddressSupported)
			//    m_Context.extensions.buffer_device_address = true;
			//todo:fix this

			void* pNext = nullptr;
			//VkPhysicalDeviceAccelerationStructurePropertiesKHR accelStructProperties;
			//VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties;
			VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservativeRasterizationProperties = {};
			VkPhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateProperties = {};
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

		void Device::runGarbageCollection() {
			for (auto& m_Queue : m_Queues)
			{
				if (m_Queue)
				{
					m_Queue->retireCommandBuffers();
				}
			}
		}

        bool Device::queryFeatureSupport(Feature feature, void* pInfo, size_t infoSize)
        {
            switch (feature)  // NOLINT(clang-diagnostic-switch-enum)
            {
                case Feature::DeferredCommandLists:
                    return true;
                case Feature::RayTracingAccelStruct:
                    return m_Context.extensions.KHR_acceleration_structure;
                case Feature::RayTracingPipeline:
                    return m_Context.extensions.KHR_ray_tracing_pipeline;
                case Feature::RayTracingOpacityMicromap:
#ifdef NVRHI_WITH_RTXMU
                    return false; // RTXMU does not support OMMs
#else
                    return m_Context.extensions.EXT_opacity_micromap && m_Context.extensions.KHR_synchronization2;
#endif
                case Feature::RayQuery:
                    return m_Context.extensions.KHR_ray_query;
                case Feature::ShaderExecutionReordering:
                {
                    if (m_Context.extensions.NV_ray_tracing_invocation_reorder)
                    {
                        //return VK_RAY_TRACING_INVOCATION_REORDER_MODE_REORDER_NV == m_Context.nvRayTracingInvocationReorderProperties.rayTracingInvocationReorderReorderingHint;
                        //return vk::RayTracingInvocationReorderModeNV::eReorder == m_Context.nvRayTracingInvocationReorderProperties.rayTracingInvocationReorderReorderingHint;
                    }
                    return false;
                }
                case Feature::ShaderSpecializations:
                    return true;
                case Feature::Meshlets:
                    return m_Context.extensions.NV_mesh_shader;
                case Feature::VariableRateShading:
                    if (pInfo)
                    {
                        if (infoSize == sizeof(VariableRateShadingFeatureInfo))
                        {
                            auto* pVrsInfo = reinterpret_cast<VariableRateShadingFeatureInfo*>(pInfo);
                            const auto& tileExtent = m_Context.shadingRateProperties.minFragmentShadingRateAttachmentTexelSize;
                            pVrsInfo->shadingRateImageTileSize = std::max(tileExtent.width, tileExtent.height);
                        }
                        else
                            utils::NotSupported();
                    }
                    return m_Context.extensions.KHR_fragment_shading_rate && m_Context.shadingRateFeatures.attachmentFragmentShadingRate;
                case Feature::ConservativeRasterization:
                    return m_Context.extensions.EXT_conservative_rasterization;
                case Feature::VirtualResources:
                    return true;
                case Feature::ComputeQueue:
                    return (m_Queues[uint32_t(CommandQueue::Compute)] != nullptr);
                case Feature::CopyQueue:
                    return (m_Queues[uint32_t(CommandQueue::Copy)] != nullptr);
                case Feature::ConstantBufferRanges:
                    return true;
                default:
                    return false;
            }
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

        CommandListHandle Device::createCommandList(const CommandListParameters &params) {
            if(!m_Queues[uint32_t(params.queueType)])
                return nullptr;

            CommandList* cmdList = new CommandList(this, m_Context, params);

            return CommandListHandle::Create(cmdList);
        }

		uint64_t
		Device::executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists,
				CommandQueue executionQueue) {
			Queue& queue = *m_Queues[uint32_t(executionQueue)];

			uint64_t submissionID = queue.submit(pCommandLists, numCommandLists);

			for (size_t i = 0; i < numCommandLists; i++)
			{
				checked_cast<CommandList*>(pCommandLists[i])->executed(queue, submissionID);
			}

			return submissionID;
		}  

		FormatSupport Device::queryFormatSupport(Format format)
		{
			VkFormat vulkanFormat = convertFormat(format);

			VkFormatProperties props;
			//m_Context.physicalDevice.getFormatProperties(vk::Format(vulkanFormat), &props);
			vkGetPhysicalDeviceFormatProperties(m_Context.physicalDevice, vulkanFormat, &props);

			FormatSupport result = FormatSupport::None;

			if (props.bufferFeatures)
				result = result | FormatSupport::Buffer;

			if (format == Format::R32_UINT || format == Format::R16_UINT) {
				// There is no explicit bit in vk::FormatFeatureFlags for index buffers
				result = result | FormatSupport::IndexBuffer;
			}

			if (props.bufferFeatures & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT)
				result = result | FormatSupport::VertexBuffer;

			if (props.optimalTilingFeatures)
				result = result | FormatSupport::Texture;

			if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				result = result | FormatSupport::DepthStencil;

			if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
				result = result | FormatSupport::RenderTarget;

			if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT)
				result = result | FormatSupport::Blendable;

			if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) ||
				(props.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT))
			{
				result = result | FormatSupport::ShaderLoad;
			}

			if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
				result = result | FormatSupport::ShaderSample;

			if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) ||
				(props.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT))
			{
				result = result | FormatSupport::ShaderUavLoad;
				result = result | FormatSupport::ShaderUavStore;
			}

			if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT) ||
				(props.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT))
			{
				result = result | FormatSupport::ShaderAtomic;
			}

			return result;
		}

        void VulkanContext::nameVKObject(const void *handle, VkObjectType objtype,
                                         const char *name) const {
            if (name && *name && handle) //todo:fix this
            {
                VkDebugUtilsObjectNameInfoEXT info = {};
                info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
                info.objectType = objtype;
                info.objectHandle = reinterpret_cast<uint64_t>(handle);
                info.pObjectName = name;
                //auto info = vk::DebugMarkerObjectNameInfoEXT()
                //        .setObjectType(objtype)
                //        .setObject(reinterpret_cast<uint64_t>(handle))
                //        .setPObjectName(name);
                //VkResult result = VK_SUCCESS;
                //PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT) vkGetInstanceProcAddr(instance, "vkDebugMarkerSetObjectNameEXT");
                //result = vkDebugMarkerSetObjectNameEXT(device, &info);
                //VK_CHECK(result);
#ifdef WIN32
                PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
                vkSetDebugUtilsObjectNameEXT(device, &info);
#else
#if ANDROID
                //PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
                //VkResult result = vkSetDebugUtilsObjectNameEXT(device, &info);
                //VK_CHECK(result);
#endif
#endif
            }
        }     

        void VulkanContext::error(const GuGuUtf8Str &message) const {
            messageCallback->message(MessageSeverity::Warning, message.getStr());
        }

		void VulkanContext::warning(const GuGuUtf8Str& message) const {
			messageCallback->message(MessageSeverity::Warning, message.getStr());
		}    
    }
}