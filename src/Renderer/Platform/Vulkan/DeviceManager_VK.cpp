#include <pch.h>

#include "DeviceManager_VK.h"

#include <Renderer/DeviceManager.h>

#include <Core/GuGuUtf8Str.h>

#include <unordered_set>

#if ANDROID
    #include <Application/Platform/Android/AndroidApplication.h>
    #include <Window/Platform/Android/AndroidWindow.h>
    #include <game-activity/native_app_glue/android_native_app_glue.h>//native window
    #include <vulkan/vulkan_android.h>
#else 
    #if WIN32

        #include <Application/Platform/Windows/WindowsApplication.h>
        #include <Window/Platform/Windows/WindowsWindow.h>
        #include <vulkan/vulkan_win32.h>
    #endif
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
                    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
                    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                   // VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
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
                    //VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
                    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,//todo:maybe need to remove
                    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                    //VK_NV_MESH_SHADER_EXTENSION_NAME,
                    //VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
                    //VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
                    //VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
                    //VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME
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

    template <typename T>
    static std::vector<T> setToVector(const std::unordered_set<T>& set)
    {
        std::vector<T> ret;
        for(const auto& s : set)
        {
            ret.push_back(s);
        }

        return ret;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {

        GuGu_LOGD("validation layer:%s\n", pCallbackData->pMessage);
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


    bool DeviceManager_VK::CreateInstanceInternal() {
        if(m_deviceParams.enableDebugRuntime)
        {
            //enabledExtensions.instance.insert(u8"VK_EXT_debug_report");
            //enabledExtensions.instance.insert(u8"VK_EXT_debug_utils");
            enabledExtensions.layers.insert(u8"VK_LAYER_KHRONOS_validation");
        }

        return createInstance();
    }

    bool DeviceManager_VK::createInstance() {
        //todo:handle headless device
        if(!m_deviceParams.headlessDevice)
        {
            enabledExtensions.instance.insert("VK_KHR_surface");
#if WIN32
            enabledExtensions.instance.insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else ANDROID
            enabledExtensions.instance.insert(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif
            //enabledExtensions.instance.insert(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
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
            GuGu_LOGD("%s\n", name.getStr());
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

        if(m_deviceParams.enableDebugRuntime) {
            enabledExtensions.instance.insert("VK_EXT_debug_utils");//todo:fix this
        }

        std::unordered_set<GuGuUtf8Str> requiredLayers = enabledExtensions.layers;
        uint32_t enumerationLayerExtensionCount = 0;
        vkEnumerateInstanceLayerProperties(&enumerationLayerExtensionCount, nullptr);
        std::vector<VkLayerProperties> enumerationLayerExtension(enumerationLayerExtensionCount);
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

        GuGu_LOGD("enabled vulkan layers:");
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
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = static_cast<uint32_t>(layerVec.size());
        createInfo.ppEnabledLayerNames = layerVec.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtVec.size());
        createInfo.ppEnabledExtensionNames = instanceExtVec.data();

        if(m_deviceParams.enableDebugRuntime)
        {
            //enabledExtensions.instance.insert("VK_EXT_debug_utils");//todo:fix this
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        }

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance));

        return true;
    }

    void DeviceManager_VK::DestroyDeviceAndSwapChain() {
        destroySwapChain();

        if(m_PresentSemaphore)
        {
            vkDestroySemaphore(m_VulkanDevice, m_PresentSemaphore, nullptr);//todo:fix this
            m_PresentSemaphore = VK_NULL_HANDLE;
        }
        m_BarrierCommandList = nullptr;
        m_NvrhiDevice = nullptr;
        m_rendererString.clear();

        if(m_VulkanDevice)
        {
            vkDestroyDevice(m_VulkanDevice, nullptr);
            m_VulkanDevice = nullptr;
        }
        if(m_windowSurface)
        {
            vkDestroySurfaceKHR(m_VulkanInstance, m_windowSurface, nullptr);
            m_windowSurface = VK_NULL_HANDLE;
        }
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
#if WIN32
            if(m_deviceParams.swapChainFormat == nvrhi::Format::SRGBA8_UNORM)
                m_deviceParams.swapChainFormat = nvrhi::Format::SBGRA8_UNORM;
            else if(m_deviceParams.swapChainFormat == nvrhi::Format::RGBA8_UNORM)
                m_deviceParams.swapChainFormat = nvrhi::Format::BGRA8_UNORM;
#else

#endif

            createWindowSurface();
        }

        pickPhysicalDevice();
        findQueueFamilies(m_VulkanPhysicalDevice);
        createDevice();

        auto vecInstanceExt = stringSetToVector(enabledExtensions.instance);
        auto vecLayers = stringSetToVector(enabledExtensions.layers);
        auto vecDeviceExt = stringSetToVector(enabledExtensions.device);

        nvrhi::vulkan::DeviceDesc deviceDesc;
        deviceDesc.errorCB = &DefaultMessageCallback::GetInstance();
        deviceDesc.instance = m_VulkanInstance;
        deviceDesc.physicalDevice = m_VulkanPhysicalDevice;
        deviceDesc.device = m_VulkanDevice;
        deviceDesc.graphicsQueue = m_GraphicsQueue;
        deviceDesc.graphicsQueueIndex = m_GraphicsQueueFamily;
        if (m_deviceParams.enableComputeQueue)
        {
            deviceDesc.computeQueue = m_ComputeQueue;
            deviceDesc.computeQueueIndex = m_ComputeQueueFamily;
        }
        if (m_deviceParams.enableCopyQueue)
        {
            deviceDesc.transferQueue = m_TransferQueue;
            deviceDesc.transferQueueIndex = m_TransferQueueFamily;
        }
        deviceDesc.instanceExtensions = vecInstanceExt.data();
        deviceDesc.numInstanceExtensions = vecInstanceExt.size();
        deviceDesc.deviceExtensions = vecDeviceExt.data();
        deviceDesc.numDeviceExtensions = vecDeviceExt.size();
        deviceDesc.bufferDeviceAddressSupported = m_BufferDeviceAddressSupported;

        m_NvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

        //todo:add validation layer functionality
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
#if ANDROID
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();

        const VkAndroidSurfaceCreateInfoKHR create_info{
                .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .window = androidWindow->getNativeHandle()};

        VK_CHECK(vkCreateAndroidSurfaceKHR(m_VulkanInstance, &create_info,
                                           nullptr /* pAllocator */, &m_windowSurface));
#else 
        #if WIN32
        std::shared_ptr<WindowsApplication> windowsApplication = WindowsApplication::getApplication();
        std::shared_ptr<WindowsWindow> windowsWindow = windowsApplication->getPlatformWindows()[0]; //todo:fix this

        VkWin32SurfaceCreateInfoKHR create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        create_info.pNext = nullptr;
        create_info.hwnd = windowsWindow->getNativeWindowHandle();
        create_info.hinstance = GetModuleHandle(nullptr);
        create_info.flags = 0;
        VK_CHECK(vkCreateWin32SurfaceKHR(m_VulkanInstance, &create_info,
            nullptr /* pAllocator */, &m_windowSurface));
        #endif
#endif
        return true;//todo:fix this
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
                //GuGu_LOGD("%s\n", ext.extensionName);
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

            //if(!physicalDeviceFeatures.textureCompressionBC)
            //{
            //    errorStr.append(u8"\n");
            //    errorStr.append(u8"- doest not support textureCompressionBC");
            //    deviceIsGood = false;
            //}

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
        uint32_t deviceExtensionsCount;
        vkEnumerateDeviceExtensionProperties(m_VulkanPhysicalDevice, nullptr, &deviceExtensionsCount, nullptr);
        std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionsCount);
        vkEnumerateDeviceExtensionProperties(m_VulkanPhysicalDevice, nullptr, &deviceExtensionsCount, deviceExtensions.data());
        for(const auto& ext : deviceExtensions)
        {
            const GuGuUtf8Str name(ext.extensionName);
            GuGu_LOGD("%s\n", name.getStr());
            if(optionalExtensions.device.find(name) != optionalExtensions.device.end())
            {
                if (name == VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME && m_deviceParams.headlessDevice)
                    continue;
                enabledExtensions.device.insert(name);
            }

            //todo:add ray tracing check
            //if(m_deviceParams.enableRayTracingExtensions && )
            //{
            //    enabledExtensions.device.insert(name);
            //}
        }
        if(!m_deviceParams.headlessDevice)
        {
            enabledExtensions.device.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(m_VulkanPhysicalDevice, &physicalDeviceProperties);
        m_rendererString = physicalDeviceProperties.deviceName;

        bool accelStructSupported = false;
        bool rayPipelineSupported = false;
        bool rayQuerySupported = false;
        bool meshletsSupported = false;
        bool vrsSupported = false;
        bool synchronization2Supported = false;
        bool maintenance4Supported = false;
        GuGu_LOGD("Enabled Vulkan device extensions:");
        for(const auto& ext : enabledExtensions.device)
        {
            GuGu_LOGD("%s", ext.getStr());
            //if (ext == VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)
            //    accelStructSupported = true;
            //else if (ext == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)
            //    rayPipelineSupported = true;
            //else if (ext == VK_KHR_RAY_QUERY_EXTENSION_NAME)
            //    rayQuerySupported = true;
            if (ext == VK_NV_MESH_SHADER_EXTENSION_NAME)
                meshletsSupported = true;
            else if (ext == VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)
                vrsSupported = true;
            //else if (ext == VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)
            //    synchronization2Supported = true;
            //else if (ext == VK_KHR_MAINTENANCE_4_EXTENSION_NAME)
            //    maintenance4Supported = true;
            else if (ext == VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME)
                m_SwapChainMutableFormatSupported = true;
        }
#define APPEND_EXTENSION(condition, desc) if(condition) { (desc).pNext = pNext; pNext = &(desc); }
        void* pNext = nullptr;
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures = {};
        //determine support for buffer device address, the vulkan 1.2 way
        //VkPhysicalDeviceBufferDeviceAddressFeaturesEXT bufferDeviceAddressFeatures{};
        //bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT;
        //determine support for maintenance4
        //todo:fix this
        //VkPhysicalDeviceMaintenance4Features maintenance4Features = {};

        //APPEND_EXTENSION(true, bufferDeviceAddressFeatures);
        //APPEND(maintenance4Supported, maintenance4Features);
        physicalDeviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        physicalDeviceFeatures.pNext = pNext;
        vkGetPhysicalDeviceFeatures2(m_VulkanPhysicalDevice, &physicalDeviceFeatures);

        //bufferDeviceAddressFeatures.bufferDeviceAddress = true;
        //bufferDeviceAddressFeatures.bufferDeviceAddressCaptureReplay = true;
        //bufferDeviceAddressFeatures.bufferDeviceAddressMultiDevice = true;

        std::unordered_set<int32_t> uniqueQueueFamilies = { m_GraphicsQueueFamily };

        if(!m_deviceParams.headlessDevice)
            uniqueQueueFamilies.insert(m_PresentQueueFamily);

        if (m_deviceParams.enableComputeQueue)
            uniqueQueueFamilies.insert(m_ComputeQueueFamily);

        if (m_deviceParams.enableCopyQueue)
            uniqueQueueFamilies.insert(m_TransferQueueFamily);

        float priority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueDesc;
        queueDesc.reserve(uniqueQueueFamilies.size());
        for(int queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &priority;
            queueDesc.push_back(queueCreateInfo);
        }

        //VkPhysicalDeviceAccelerationStructureFeaturesKHR accelStructFeatures = {};
        //accelStructFeatures.accelerationStructure = true;

        //VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayPipelineFeatures = {};
        //rayPipelineFeatures.rayTracingPipeline = true;
        //rayPipelineFeatures.rayTraversalPrimitiveCulling = true;

        //VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{};
        //rayQueryFeatures.rayQuery = true;

        VkPhysicalDeviceMeshShaderFeaturesNV meshletFeatures{};
        meshletFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV;
        meshletFeatures.taskShader = true;
        meshletFeatures.meshShader = true;

        VkPhysicalDeviceFragmentShadingRateFeaturesKHR vrsFeatures{};
        vrsFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
        vrsFeatures.pipelineFragmentShadingRate = true;
        vrsFeatures.primitiveFragmentShadingRate = true;
        vrsFeatures.attachmentFragmentShadingRate = true;

        //VkPhysicalDeviceVulkan13Features vulkan13Features{};
        //vulkan13Features.synchronization2 = synchronization2Supported;
        //vulkan13Features.maintenance4 =

        pNext = nullptr;
        //APPEND_EXTENSION(accelStructSupported, accelStructFeatures)
        //APPEND_EXTENSION(rayPipelineSupported, rayPipelineFeatures)
        //APPEND_EXTENSION(rayQuerySupported, rayQueryFeatures)
        APPEND_EXTENSION(meshletsSupported, meshletFeatures)
        APPEND_EXTENSION(vrsSupported, vrsFeatures)
        //APPEND_EXTENSION(physicalDeviceProperties.apiVersion >= VK_API_VERSION_1_3, vulkan13features)
        //APPEND_EXTENSION(physicalDeviceProperties.apiVersion < VK_API_VERSION_1_3 && maintenance4Supported, maintenance4Features);
#undef APPEND_EXTENSION
        VkPhysicalDeviceFeatures deviceFeatures{};
        //deviceFeatures.shaderImageGatherExtended = true;
        //deviceFeatures.samplerAnisotropy = true;
        //deviceFeatures.tessellationShader = true;
        //deviceFeatures.textureCompressionBC = true;
        //deviceFeatures.geometryShader = true;
        //deviceFeatures.imageCubeArray = true;
        //deviceFeatures.dualSrcBlend = true;
        deviceFeatures.shaderImageGatherExtended = false;
        deviceFeatures.samplerAnisotropy = true;
        deviceFeatures.tessellationShader = false;
        deviceFeatures.textureCompressionBC = false;
        deviceFeatures.geometryShader = false;
        deviceFeatures.imageCubeArray = false;
        deviceFeatures.dualSrcBlend = false;

        //VkPhysicalDeviceVulkan11Features vulkan11Features{};

        VkPhysicalDeviceVulkan12Features vulkan12Features{};
        ////vulkan12Features.timelineSemaphore = true;//todo:remove this
        vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vulkan12Features.descriptorIndexing = true;
        vulkan12Features.runtimeDescriptorArray = true;
        vulkan12Features.descriptorBindingPartiallyBound = true;
        vulkan12Features.descriptorBindingVariableDescriptorCount = true;
        vulkan12Features.timelineSemaphore = true;
        vulkan12Features.shaderSampledImageArrayNonUniformIndexing = true;
        //vulkan12Features.bufferDeviceAddress = bufferDeviceAddressFeatures.bufferDeviceAddress;
        vulkan12Features.bufferDeviceAddress = false;
        vulkan12Features.pNext = pNext;

        auto layerVec = stringSetToVector(enabledExtensions.layers);
        auto extVec = stringSetToVector(enabledExtensions.device);

        VkDeviceCreateInfo deviceDesc = {};
        deviceDesc.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceDesc.pQueueCreateInfos = queueDesc.data();
        deviceDesc.queueCreateInfoCount = queueDesc.size();
        deviceDesc.pEnabledFeatures = &deviceFeatures;
        deviceDesc.enabledExtensionCount = extVec.size();
        deviceDesc.ppEnabledExtensionNames = extVec.data();
        deviceDesc.enabledLayerCount = layerVec.size();
        deviceDesc.ppEnabledLayerNames = layerVec.data();
        deviceDesc.pNext = &vulkan12Features;

        if (m_deviceParams.deviceCreateInfoCallback)
            m_deviceParams.deviceCreateInfoCallback(deviceDesc);

        VK_CHECK(vkCreateDevice(m_VulkanPhysicalDevice, &deviceDesc, nullptr, &m_VulkanDevice));

        vkGetDeviceQueue(m_VulkanDevice, m_GraphicsQueueFamily, 0, &m_GraphicsQueue);
        if (m_deviceParams.enableComputeQueue)
            vkGetDeviceQueue(m_VulkanDevice, m_ComputeQueueFamily, 0, &m_ComputeQueue);
        if (m_deviceParams.enableCopyQueue)
            vkGetDeviceQueue(m_VulkanDevice, m_TransferQueueFamily, 0, &m_TransferQueue);
        if (!m_deviceParams.headlessDevice)
            vkGetDeviceQueue(m_VulkanDevice, m_PresentQueueFamily, 0, &m_PresentQueue);

        // remember the bufferDeviceAddress feature enablement
        m_BufferDeviceAddressSupported = vulkan12Features.bufferDeviceAddress;

        GuGu_LOGD("Created Vulkan device: %s", m_rendererString.getStr());
        return true;
    }
    bool DeviceManager_VK::CreateSwapChain() {
        createSwapChain();

        //todo:create command list
        m_BarrierCommandList = m_NvrhiDevice->createCommandList();

        //todo:create present semaphore
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(m_VulkanDevice, &semaphoreCreateInfo, nullptr, &m_PresentSemaphore);
        return true;
    };
#if 1
    VkInstance DeviceManager_VK::getInstance() {
        return m_VulkanInstance;
    }

    VkDebugUtilsMessengerEXT DeviceManager_VK::getDebugMessenger() {
        return m_debugMessenger;
    }

    VkDevice DeviceManager_VK::getDevice() {
        return m_VulkanDevice;
    }

    VkPhysicalDevice DeviceManager_VK::getPhysicalDevice() {
        return m_VulkanPhysicalDevice;
    }

    VkQueue DeviceManager_VK::getQueue() {
        return m_GraphicsQueue;
    }

    int32_t DeviceManager_VK::getQueueFamilyIndex() {
        return m_GraphicsQueueFamily;
    }

    VkSurfaceKHR DeviceManager_VK::getSurface() {
        return m_windowSurface;
    }

    VkSwapchainKHR DeviceManager_VK::getSwapChain() {
        return m_SwapChain;
    }
    std::vector<VkImage> DeviceManager_VK::getSwapChainImage() {
        //return m_SwapChainImages;
        std::vector<VkImage> vkImages;
        for(size_t i = 0; i < m_SwapChainImages.size(); ++i)
        {
            vkImages.push_back(m_SwapChainImages[i].image);
        }
        return vkImages;
    }
    std::vector<VkImageView> DeviceManager_VK::getSwapChainImageViews() {
        std::vector<VkImageView> vkImageViews;
        nvrhi::TextureSubresourceSet subresourceSet{};
        //subresourceSet.baseMipLevel = 1;
        //subresourceSet.baseArraySlice = 1;
        for(size_t i = 0; i < m_SwapChainImages.size(); ++i)
        {
            vkImageViews.push_back(reinterpret_cast<VkImageView>((m_SwapChainImages[i].rhiHandle->getNativeView(nvrhi::ObjectTypes::VK_ImageView, nvrhi::Format::UNKNOWN, subresourceSet,
                                                                                 nvrhi::TextureDimension::Texture2D, false).pointer)));
        }
        return vkImageViews;
    }

    VkFormat DeviceManager_VK::getSwapChainFormat() {
        return m_SwapChainFormat.format;
    }

#endif


    DeviceManager *DeviceManager::CreateVK() {
        return new DeviceManager_VK();
    }
    bool DeviceManager_VK::createSwapChain() {
        destroySwapChain();

        m_SwapChainFormat.format = nvrhi::vulkan::convertFormat(m_deviceParams.swapChainFormat);
        m_SwapChainFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        //todo:add more logic

#if ANDROID
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
        int32_t height  = ANativeWindow_getHeight(androidWindow->getNativeHandle());
        int32_t width = ANativeWindow_getWidth(androidWindow->getNativeHandle());
#else 
    #if WIN32
        std::shared_ptr<WindowsApplication> windowsApplication = WindowsApplication::getApplication();
        std::shared_ptr<WindowsWindow> windowsWindow = windowsApplication->getPlatformWindows()[0]; //todo:fix this

        int32_t width = 0;
        int32_t height = 0;
        RECT rect;
        if (GetClientRect(windowsWindow->getNativeWindowHandle(), &rect))
        {
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
    #endif
#endif

        //std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        //std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
        //
        //int32_t height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
        //int32_t width = ANativeWindow_getWidth(androidWindow->getNativeHandle());

        VkExtent2D extent{};
        extent.width = width; //todo:fix this
        extent.height = height;

        std::unordered_set<uint32_t> uniqueQueues = {
                (uint32_t)(m_GraphicsQueueFamily),
                (uint32_t)(m_PresentQueueFamily)
        };

        std::vector<uint32_t> queues = setToVector(uniqueQueues);

        const bool enableSwapChainSharing = queues.size() > 1;

        VkSwapchainCreateInfoKHR swapchainCreateInfoKhr{};
        swapchainCreateInfoKhr.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfoKhr.surface = m_windowSurface;
        swapchainCreateInfoKhr.minImageCount = m_deviceParams.swapChainBufferCount;
        swapchainCreateInfoKhr.imageFormat = m_SwapChainFormat.format;
        swapchainCreateInfoKhr.imageColorSpace = m_SwapChainFormat.colorSpace;
        swapchainCreateInfoKhr.imageExtent = extent;
        swapchainCreateInfoKhr.imageArrayLayers = 1;
        swapchainCreateInfoKhr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                VK_IMAGE_USAGE_SAMPLED_BIT;
        swapchainCreateInfoKhr.imageSharingMode = enableSwapChainSharing ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfoKhr.flags = m_SwapChainMutableFormatSupported ? VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR : 0;//note:this may be error
        swapchainCreateInfoKhr.queueFamilyIndexCount = enableSwapChainSharing ? uint32_t(queues.size()) : 0;
        swapchainCreateInfoKhr.pQueueFamilyIndices = enableSwapChainSharing ? queues.data() : nullptr;
        swapchainCreateInfoKhr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
#ifdef ANDROID
        swapchainCreateInfoKhr.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
#else
        swapchainCreateInfoKhr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
#endif
        swapchainCreateInfoKhr.presentMode = m_deviceParams.vsyncEnabled ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
        swapchainCreateInfoKhr.clipped = true;
        swapchainCreateInfoKhr.oldSwapchain = VK_NULL_HANDLE;

        std::vector<VkFormat> imageFormats = { m_SwapChainFormat.format };
        switch(m_SwapChainFormat.format)
        {
            case VK_FORMAT_R8G8B8A8_UNORM:
                imageFormats.push_back(VK_FORMAT_R8G8B8A8_SRGB);
                break;
            case VK_FORMAT_R8G8B8A8_SRGB:
                imageFormats.push_back(VK_FORMAT_R8G8B8A8_UNORM);
                break;
            case VK_FORMAT_B8G8R8A8_UNORM:
                imageFormats.push_back(VK_FORMAT_B8G8R8A8_SRGB);
                break;
            case VK_FORMAT_B8G8R8A8_SRGB:
                imageFormats.push_back(VK_FORMAT_B8G8R8A8_UNORM);
                break;
        }

        VkImageFormatListCreateInfo imageFormatListCreateInfo{};
        imageFormatListCreateInfo.pViewFormats = imageFormats.data();

        if(m_SwapChainMutableFormatSupported)
            swapchainCreateInfoKhr.pNext = &imageFormatListCreateInfo;

        VkResult res = vkCreateSwapchainKHR(m_VulkanDevice, &swapchainCreateInfoKhr, nullptr, &m_SwapChain);
        if(res != VK_SUCCESS)
        {
            GuGu_LOGE("failed to create a vulkan swap chain, error code = %s", nvrhi::vulkan::resultToString(VkResult(res)));
            return false;
        }

        //retrieve swap chain images
        uint32_t swapChainImageCount = 0;
        vkGetSwapchainImagesKHR(m_VulkanDevice, m_SwapChain, &swapChainImageCount, nullptr);
        std::vector<VkImage> images(swapChainImageCount);
        vkGetSwapchainImagesKHR(m_VulkanDevice, m_SwapChain, &swapChainImageCount, images.data());
        for(auto image : images)
        {
            SwapChainImage sci;
            sci.image = image;

            //todo:remove these
#if ANDROID
            std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
            std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
            int32_t height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
            int32_t width = ANativeWindow_getWidth(androidWindow->getNativeHandle());
#else 
            #if WIN32
            std::shared_ptr<WindowsApplication> windowsApplication = WindowsApplication::getApplication();
            std::shared_ptr<WindowsWindow> windowsWindow = windowsApplication->getPlatformWindows()[0]; //todo:fix this

            int32_t width = 0;
            int32_t height = 0;
            RECT rect;
            if (GetClientRect(windowsWindow->getNativeWindowHandle(), &rect))
            {
                width = rect.right - rect.left;
                height = rect.bottom - rect.top;
            }
            #endif
#endif

            nvrhi::TextureDesc textureDesc;
            //textureDesc.width = m_deviceParams.backBufferWidth;
            //textureDesc.height = m_deviceParams.backBufferHeight;
            textureDesc.width = width;
            textureDesc.height = height;
            textureDesc.format = m_deviceParams.swapChainFormat;
            textureDesc.debugName = "Swap chain image";
            textureDesc.initialState = nvrhi::ResourceStates::Present;
            textureDesc.keepInitialState = true;
            textureDesc.isRenderTarget = true;

            sci.rhiHandle = m_NvrhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image, nvrhi::Object(sci.image), textureDesc);
            m_SwapChainImages.push_back(sci);
        }

        m_SwapChainIndex = 0;

        return true;
    }

    void DeviceManager_VK::destroySwapChain() {
        if(m_VulkanDevice)
        {
            //todo:add wait idle
        }

        if(m_SwapChain)
        {
            vkDestroySwapchainKHR(m_VulkanDevice, m_SwapChain, VK_NULL_HANDLE);//todo:fix this
            m_SwapChain = VK_NULL_HANDLE;
        }
        m_SwapChainImages.clear();
    }

    void DeviceManager_VK::BeginFrame() {

        VK_CHECK(vkAcquireNextImageKHR(m_VulkanDevice, m_SwapChain, std::numeric_limits<uint64_t>::max(), m_PresentSemaphore,
                              VK_NULL_HANDLE, &m_SwapChainIndex));

        //assert(res == VK_SUCCESS);

        m_NvrhiDevice->queueWaitForSemaphore(nvrhi::CommandQueue::Graphics, m_PresentSemaphore, 0);
    }

    nvrhi::ITexture *DeviceManager_VK::GetBackBuffer(uint32_t index) {
        if(index < m_SwapChainImages.size())
            return m_SwapChainImages[index].rhiHandle;
        return nullptr;
    }

    uint32_t DeviceManager_VK::GetCurrentBackBufferIndex() {
        return m_SwapChainIndex;
    }

    uint32_t DeviceManager_VK::GetBackBufferCount() {
        return uint32_t(m_SwapChainImages.size());
    }

    void DeviceManager_VK::Present() {
        m_NvrhiDevice->queueSignalSemaphore(nvrhi::CommandQueue::Graphics, m_PresentSemaphore, 0);

        m_BarrierCommandList->open(); // umm...
        m_BarrierCommandList->close();
        m_NvrhiDevice->executeCommandList(m_BarrierCommandList);

        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &m_PresentSemaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &m_SwapChain;
        info.pImageIndices = &m_SwapChainIndex;

        VkResult res = vkQueuePresentKHR(m_PresentQueue, &info);

        //vk::PresentInfoKHR info = vk::PresentInfoKHR()
        //        .setWaitSemaphoreCount(1)
        //        .setPWaitSemaphores(&m_PresentSemaphore)
        //        .setSwapchainCount(1)
        //        .setPSwapchains(&m_SwapChain)
        //        .setPImageIndices(&m_SwapChainIndex);
//
        //const vk::Result res = m_PresentQueue.presentKHR(&info);
        //assert(res == vk::Result::eSuccess || res == vk::Result::eErrorOutOfDateKHR);

        vkQueueWaitIdle(m_PresentQueue);

//        if (m_DeviceParams.enableDebugRuntime)
//        {
//            // according to vulkan-tutorial.com, "the validation layer implementation expects
//            // the application to explicitly synchronize with the GPU"
//            m_PresentQueue.waitIdle();
//        }
//        else
//        {
//#ifndef _WIN32
//            if (m_DeviceParams.vsyncEnabled)
//            {
//                m_PresentQueue.waitIdle();
//            }
//#endif
//
//            while (m_FramesInFlight.size() > m_DeviceParams.maxFramesInFlight)
//            {
//                auto query = m_FramesInFlight.front();
//                m_FramesInFlight.pop();
//
//                m_NvrhiDevice->waitEventQuery(query);
//
//                m_QueryPool.push_back(query);
//            }
//
//            nvrhi::EventQueryHandle query;
//            if (!m_QueryPool.empty())
//            {
//                query = m_QueryPool.back();
//                m_QueryPool.pop_back();
//            }
//            else
//            {
//                query = m_NvrhiDevice->createEventQuery();
//            }
//
//            m_NvrhiDevice->resetEventQuery(query);
//            m_NvrhiDevice->setEventQuery(query, nvrhi::CommandQueue::Graphics);
//            m_FramesInFlight.push(query);
//        }
    }


}