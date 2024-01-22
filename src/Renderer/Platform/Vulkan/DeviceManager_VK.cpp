#include <pch.h>

#include <Renderer/DeviceManager.h>

#include <Core/GuGuUtf8Str.h>

#include <unordered_set>

#if ANDROID
#include <Application/Platform/Android/AndroidApplication.h>
#include <Window/Platform/Android/AndroidWindow.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>//native window
#endif

#include "vulkan-constants.h"

namespace GuGu{
    struct VulkanExtensions
    {
        std::unordered_set<GuGuUtf8Str> instance;
        std::unordered_set<GuGuUtf8Str> layers;
        std::unordered_set<GuGuUtf8Str> device;
    };
    //minimal set of required extensions
    VulkanExtensions enabledExtensions = {
            //instance
            {
                    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
            },
            //layers
            {},
            //device
            {
                    VK_KHR_MAINTENANCE1_EXTENSION_NAME
            }
    };

    //optional extensions
    VulkanExtensions optionalExtensions = {
            //instance
            {
                    VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
                    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            },
            //layers
            {

            },
            // device
            {
                    VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
                    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                    VK_NV_MESH_SHADER_EXTENSION_NAME,
                    VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
                    //VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
                    //VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
                    VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME
            },
    };

    static std::vector<const char*> stringSetToVector(const std::unordered_set<GuGuUtf8Str>& set)
    {
        std::vector<const char*> ret;
        for(const auto& s : set)
        {
            ret.push_back(s.getStr());
        }
        return ret;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {

        GuGu_LOGE("validation layer:%s\n", pCallbackData->pMessage);
        //std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    class DeviceManager_VK : public DeviceManager{
    public:

    protected:
        bool CreateInstanceInternal() override;
        void DestroyDeviceAndSwapChain() override;
        bool CreateDevice() override;
    private:
        bool createInstance();
        bool createWindowSurface();
        void installDebugCallback();
        bool pickPhysicalDevice();
        bool findQueueFamilies(VkPhysicalDevice physicalDevice);
        bool createDevice();

        VkInstance m_VulkanInstance;
        //VkDebugReportCallbackEXT m_debugReportCallback;
        VkDebugUtilsMessengerEXT m_debugMessenger;//debug messenger
        VkSurfaceKHR m_windowSurface;

        VkPhysicalDevice m_VulkanPhysicalDevice;
        int32_t m_GraphicsQueueFamily = -1;
        int32_t m_ComputeQueueFamily = -1;
        int32_t m_TransferQueueFamily = -1;
        int32_t m_PresentQueueFamily = -1;
    };

    bool DeviceManager_VK::CreateInstanceInternal() {
        if(m_deviceParams.enableDebugRuntime)
        {
            enabledExtensions.instance.insert(u8"VK_EXT_debug_report");
            enabledExtensions.layers.insert(u8"VK_LAYER_KHRONOS_validation");
        }

        return createInstance();
    }

    bool DeviceManager_VK::createInstance() {
        //todo:handle headless device
        if(!m_deviceParams.headlessDevice)
        {
            enabledExtensions.instance.insert("VK_KHR_surface");
            enabledExtensions.instance.insert(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
        }

        //add instance extensions requested by the user
        for(const GuGuUtf8Str& name : m_deviceParams.requiredVulkanInstanceExtensions)
        {
            enabledExtensions.instance.insert(name);
        }
        for(const GuGuUtf8Str& name : m_deviceParams.optionalVulkanInstanceExtensions)
        {
            optionalExtensions.instance.insert(name);
        }

        //add layers requested by the user
        for(const GuGuUtf8Str& name : m_deviceParams.requiredVulkanLayers)
        {
            enabledExtensions.layers.insert(name);
        }
        for(const GuGuUtf8Str& name : m_deviceParams.optionalVulkanLayers)
        {
            optionalExtensions.layers.insert(name);
        }

        std::unordered_set<GuGuUtf8Str> requiredExtensions = enabledExtensions.instance;

        uint32_t enumerationExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &enumerationExtensionCount, nullptr);
        std::vector<VkExtensionProperties> enumerationExtension(enumerationExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &enumerationExtensionCount, enumerationExtension.data());

        //figure out which optional extensions are supported
        for(const auto& instanceExt : enumerationExtension)
        {
            const GuGuUtf8Str name = instanceExt.extensionName;
            if(optionalExtensions.instance.find(name) != optionalExtensions.instance.end())
            {
                enabledExtensions.instance.insert(name);
            }

            requiredExtensions.erase(name);
        }

        if(!requiredExtensions.empty())
        {
            GuGu_LOGD("cannot create vulkan instance because the following required extension(s) are not supported!");
            for(const auto& ext : requiredExtensions)
            {
                GuGu_LOGD("%s", ext.getStr());
            }

            return false;
        }

        GuGu_LOGD("enabled vulkan instance extensions:");
        for(const auto& ext : enabledExtensions.instance)
        {
            GuGu_LOGD("%s\n", ext.getStr());
        }

        std::unordered_set<GuGuUtf8Str> requiredLayers = enabledExtensions.layers;
        uint32_t enumerationLayerExtensionCount = 0;
        vkEnumerateInstanceLayerProperties(&enumerationLayerExtensionCount, nullptr);
        std::vector<VkLayerProperties> enumerationLayerExtension(enumerationExtensionCount);
        vkEnumerateInstanceLayerProperties(&enumerationLayerExtensionCount, enumerationLayerExtension.data());

        for(const auto& layer : enumerationLayerExtension)
        {
            const GuGuUtf8Str name = layer.layerName;
            if(optionalExtensions.layers.find(name) != optionalExtensions.layers.end())
            {
                enabledExtensions.layers.insert(name);
            }

            requiredLayers.erase(name);
        }

        if(!requiredLayers.empty())
        {
            GuGu_LOGD("cannot create vulkan instance because the following required layer(s) are not supported!");
            for(const auto& ext : requiredLayers)
            {
                GuGu_LOGD("%s", ext.getStr());
            }

            return false;
        }

        GuGu_LOGD("enabled vulkan instance extensions:");
        for(const auto& layer : enabledExtensions.layers)
        {
            GuGu_LOGD("%s\n", layer.getStr());
        }

        auto instanceExtVec = stringSetToVector(enabledExtensions.instance);
        auto layerVec = stringSetToVector(enabledExtensions.layers);

        VkApplicationInfo appInfo{};

        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "GuGu";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = static_cast<uint32_t>(layerVec.size());
        createInfo.ppEnabledLayerNames = layerVec.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtVec.size());
        createInfo.ppEnabledExtensionNames = instanceExtVec.data();
        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance));

        return true;
    }

    void DeviceManager_VK::DestroyDeviceAndSwapChain() {
        if(m_debugMessenger)
        {
            DestroyDebugUtilsMessengerEXT(m_VulkanInstance, m_debugMessenger, nullptr);
        }
        if(m_VulkanInstance)
        {
            vkDestroyInstance(m_VulkanInstance, nullptr);
            m_VulkanInstance = nullptr;
        }
    }

    bool DeviceManager_VK::CreateDevice() {
        if(m_deviceParams.enableDebugRuntime)
        {
            installDebugCallback();
        }

        //add device extensions requested by the user
        for(const GuGuUtf8Str& name : m_deviceParams.requiredVulkanDeviceExtensions)
        {
            enabledExtensions.device.insert(name);
        }

        for(const GuGuUtf8Str& name : m_deviceParams.optionalVulkanDeviceExtensions)
        {
            optionalExtensions.device.insert(name);
        }

        if(!m_deviceParams.headlessDevice)
        {
            //need to adjust the swap chain format before creating the device because it affects physical device selection
            if(m_deviceParams.swapChainFormat == nvrhi::Format::SRGBA8_UNORM)
                m_deviceParams.swapChainFormat = nvrhi::Format::SBGRA8_UNORM;
            else if(m_deviceParams.swapChainFormat == nvrhi::Format::RGBA8_UNORM)
                m_deviceParams.swapChainFormat = nvrhi::Format::BGRA8_UNORM;

            createWindowSurface();
        }

        pickPhysicalDevice();
        findQueueFamilies(m_VulkanPhysicalDevice);

        return true;
    }

    void DeviceManager_VK::installDebugCallback() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(m_VulkanInstance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
            GuGu_LOGE("validation layers requested, but not available!\n");
        }
    }

    bool DeviceManager_VK::createWindowSurface() {
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();

        const VkAndroidSurfaceCreateInfoKHR create_info{
                .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .window = androidWindow->getNativeHandle()};

        VK_CHECK(vkCreateAndroidSurfaceKHR(m_VulkanInstance, &create_info,
                                           nullptr /* pAllocator */, &m_windowSurface));
    }

    bool DeviceManager_VK::pickPhysicalDevice() {
        VkFormat requestedFormat = nvrhi::vulkan::convertFormat(m_deviceParams.swapChainFormat);
        VkExtent2D requestedExtent;
        requestedExtent.width = m_deviceParams.backBufferWidth;
        requestedExtent.height = m_deviceParams.backBufferHeight;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, physicalDevices.data());

        int32_t firstDevice = 0;
        int32_t lastDevice = deviceCount - 1;
        if(m_deviceParams.adapterIndex >= 0)
        {
            if(m_deviceParams.adapterIndex > lastDevice)
            {
                GuGu_LOGE("the specified vulkan physical device %d does not exits.", m_deviceParams.adapterIndex);
                return false;
            }
            firstDevice = m_deviceParams.adapterIndex;
            lastDevice = m_deviceParams.adapterIndex;
        }

        //start building an error message in case we cannot find a device
        GuGuUtf8Str errorStr(u8"cannot find a vulkan that support all the required extensions and properties");

        //build a list of gpus
        std::vector<VkPhysicalDevice> discreteGPUs;
        std::vector<VkPhysicalDevice> otherGPUs;
        for(int32_t deviceIndex = firstDevice; deviceIndex <= lastDevice; ++deviceIndex)
        {
            const VkPhysicalDevice& dev = physicalDevices[deviceIndex];
            VkPhysicalDeviceProperties prop;
            vkGetPhysicalDeviceProperties(dev, &prop);
            errorStr.append(u8"\n");
            errorStr.append(prop.deviceName);//todo:fix this

            //check that all required device extensions are present
            std::unordered_set<GuGuUtf8Str> requiredExtensions = enabledExtensions.device;

            uint32_t deviceExtensionsCount;
            vkEnumerateDeviceExtensionProperties(dev, nullptr, &deviceExtensionsCount, nullptr);
            std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionsCount);
            vkEnumerateDeviceExtensionProperties(dev, nullptr, &deviceExtensionsCount, deviceExtensions.data());

            for(const auto& ext : deviceExtensions)
            {
                requiredExtensions.erase(GuGuUtf8Str(ext.extensionName));
            }

            bool deviceIsGood = true;
            if(!requiredExtensions.empty())
            {
                //device is missing one or more required extensions
                for(const auto& ext : requiredExtensions)
                {
                    errorStr.append(u8"\n");
                    errorStr.append(u8"- missing");
                    errorStr.append(ext);
                }
                deviceIsGood = false;
            }
            VkPhysicalDeviceFeatures physicalDeviceFeatures;
            vkGetPhysicalDeviceFeatures(dev, &physicalDeviceFeatures);
            //auto deviceFeatures =

            if(!physicalDeviceFeatures.samplerAnisotropy)
            {
                errorStr.append(u8"\n");
                errorStr.append(u8"- does not support samplerAnisotropy");
                deviceIsGood = false;
            }

            if(!physicalDeviceFeatures.textureCompressionBC)
            {
                errorStr.append(u8"\n");
                errorStr.append(u8"- doest not support textureCompressionBC");
                deviceIsGood = false;
            }

            if(!findQueueFamilies(dev)) //todo:fix android platform
            {
                //device doesn't have all the queue families we need
                errorStr.append(u8"- does not support the necessary queue types");
                deviceIsGood = false;
            }

            if(m_windowSurface)
            {
                //check that this device supports our intended swap chain creation parameters
                VkSurfaceCapabilitiesKHR surfaceCaps;
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, m_windowSurface, &surfaceCaps);

                uint32_t surfaceFmtCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(dev, m_windowSurface, &surfaceFmtCount, nullptr);
                std::vector<VkSurfaceFormatKHR> surfaceFmts(surfaceFmtCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(dev, m_windowSurface, &surfaceFmtCount, surfaceFmts.data());

                uint32_t presentModeCount;
                //VkPresentModeKHR surfacePModes;
                vkGetPhysicalDeviceSurfacePresentModesKHR(dev, m_windowSurface, &presentModeCount, nullptr);
                std::vector<VkPresentModeKHR> surfacePModes(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(dev, m_windowSurface, &presentModeCount, surfacePModes.data());

                if(surfaceCaps.minImageCount > m_deviceParams.swapChainBufferCount
                || (surfaceCaps.maxImageCount < m_deviceParams.swapChainBufferCount && surfaceCaps.maxImageCount > 0))
                {
                    errorStr.append(u8"  - cannot support the requested swap chain image count:");
                    errorStr.append(u8" requested ");
                    std::string count = std::to_string(m_deviceParams.swapChainBufferCount);
                    //GuGuUtf8Str swapChainBufferCount(count.c_str());
                    errorStr.append(count);
                    errorStr.append(u8", available ");
                    std::string minImageCount = std::to_string(surfaceCaps.minImageCount);
                    //GuGuUtf8Str minImageCount(minImageCount);
                    errorStr.append(minImageCount);
                    errorStr.append(u8" - ");
                    std::string maxImageCount = std::to_string(surfaceCaps.maxImageCount);
                    errorStr.append(maxImageCount);
                    deviceIsGood = false;
                }

                if(surfaceCaps.minImageExtent.width > requestedExtent.width ||
                        surfaceCaps.minImageExtent.height > requestedExtent.height ||
                        surfaceCaps.maxImageExtent.width < requestedExtent.width ||
                        surfaceCaps.maxImageExtent.height < requestedExtent.height)
                {
                    errorStr.append(u8"\n");
                    errorStr.append(u8" - cannot supported the requested swap chain size:");
                    errorStr.append(u8" requested ");
                    std::string width = std::to_string(requestedExtent.width);
                    std::string height = std::to_string(requestedExtent.height);
                    std::string minImageExtentWidth = std::to_string(surfaceCaps.minImageExtent.width);
                    std::string minImageExtentHeight = std::to_string(surfaceCaps.minImageExtent.height);
                    std::string maxImageExtentWidth = std::to_string(surfaceCaps.maxImageExtent.width);
                    std::string maxImageExtentHeight = std::to_string(surfaceCaps.maxImageExtent.height);
                    std::string totalStr = width + u8"x" + height + u8", available " + minImageExtentWidth + u8"x" + minImageExtentHeight + u8" - " + maxImageExtentWidth + u8"x" + maxImageExtentHeight;
                    errorStr.append(totalStr);
                    deviceIsGood = false;
                }
                bool surfaceFormatPresent = false;
                for(const VkSurfaceFormatKHR& surfaceFmt : surfaceFmts)
                {
                    if(surfaceFmt.format == VkFormat(requestedFormat))
                    {
                        surfaceFormatPresent = true;
                        break;
                    }
                }

                if(!surfaceFormatPresent)
                {
                    //can't create a swap chain using the format requested
                    errorStr.append(u8"\n");
                    errorStr.append(u8" - does not support the requested swap chain format");
                    deviceIsGood = false;
                }

                //check that we can present from the graphics queue
                uint32_t canPresent;
                vkGetPhysicalDeviceSurfaceSupportKHR(dev, m_GraphicsQueueFamily, m_windowSurface, &canPresent);
                if(!canPresent)
                {
                    errorStr.append(u8"\n");
                    errorStr.append(u8" - cannot present");
                    deviceIsGood = false;
                }
            }

            if(!deviceIsGood)
                continue;

            if(prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                discreteGPUs.push_back(dev);
            }
            else
            {
                otherGPUs.push_back(dev);
            }
        }

        //pick the first discrete gpu if exists, otherwise the first integrated gpu
        if(!discreteGPUs.empty())
        {
            m_VulkanPhysicalDevice = discreteGPUs[0];
            return true;
        }

        if(!otherGPUs.empty())
        {
            m_VulkanPhysicalDevice = otherGPUs[0];
            return true;
        }
        GuGu_LOGE("%s", errorStr.getStr());
        return false;
    }

    bool DeviceManager_VK::findQueueFamilies(VkPhysicalDevice physicalDevice) {
        uint32_t propCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propCount, nullptr);
        std::vector<VkQueueFamilyProperties> props(propCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propCount, props.data());

        for(int32_t i = 0; i < propCount; ++i)
        {
            const auto& queueFamily = props[i];

            if(m_GraphicsQueueFamily == -1)
            {
                if(queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    m_GraphicsQueueFamily = i;
                }
            }

            if(m_ComputeQueueFamily == -1)
            {
                if(queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
                && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    m_ComputeQueueFamily = i;
                }
            }

            if(m_TransferQueueFamily == -1)
            {
                if(queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
                   && !(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
                   && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    m_TransferQueueFamily = i;
                }
            }

            if(m_PresentQueueFamily == -1)
            {
                VkBool32 support = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_windowSurface, &support);
                if(queueFamily.queueCount > 0 &&
                        support == VK_TRUE)
                {
                    m_PresentQueueFamily = i;
                }
            }
        }

        if(m_GraphicsQueueFamily == -1 || m_PresentQueueFamily == -1 && !m_deviceParams.headlessDevice
        || (m_ComputeQueueFamily == -1 && m_deviceParams.enableComputeQueue) ||
           (m_TransferQueueFamily == -1 && m_deviceParams.enableCopyQueue))
        {
            return false;
        }
        return true;
    }

    bool DeviceManager_VK::createDevice() {
        //figure out which optional extensions are supported
        
    }


    DeviceManager *DeviceManager::CreateVK() {
        return new DeviceManager_VK();
    }


}