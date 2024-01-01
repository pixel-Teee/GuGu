#pragma once

#include "Renderer.h"

#include <vector>
#include <vulkan/vulkan.h>

#include <queue>

namespace GuGu{
    class PipelineBuilder{
    public:

        std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
        VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
        VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
        VkViewport _viewport;
        VkRect2D _scissor;
        VkPipelineRasterizationStateCreateInfo _rasterizer;
        VkPipelineColorBlendAttachmentState _colorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo _multisampling;
        VkPipelineLayout _pipelineLayout;

        VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
    };

    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void push_function(std::function<void()>&& function) {
            deletors.push_back(function);
        }

        void flush() {
            // reverse iterate the deletion queue to execute all the functions
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
                (*it)(); //call the function
            }

            deletors.clear();
        }
    };

    class VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer();

        virtual ~VulkanRenderer();

        virtual void init() override;

        virtual void onRender() override;

        virtual void onDestroy() override;

        bool loadShaderModule(const char* filePath, VkShaderModule* outShaderModule);

        //void setAssetManager(AAssetManager*);

    private:
        void initVulkan();

        void initSwapchain();

        void initCommands();

        void initSyncStructures();

        void initDefaultRenderPass();

        void initFrameBuffers();

        void initPipelines();

        VkInstance m_instance; //vulkan api context
        VkDebugUtilsMessengerEXT m_debugMessenger;//vulkan debug output handle
        VkPhysicalDevice m_chosenGPU;//gpu chosen as the default device
        VkDevice m_device;//vulkan device for commands
        VkSurfaceKHR m_surface;//vulkan window surface

        VkSwapchainKHR m_swapChain;
        //image format expected by the windowing system
        VkFormat m_swapchainImageFormat;
        //array of images from the swapchain
        std::vector<VkImage> m_swapchainImages;
        //array of image-views from the swapchain
        std::vector<VkImageView> m_swapchainImageViews;

        VkQueue m_graphicsQueue;//queue we will submit to
        uint32_t m_graphicsQueueFamily;//family of that queue

        VkCommandPool m_commandPool;//the command pool for our commands
        VkCommandBuffer m_mainCommandBuffer;//the buffer we will record into

        VkRenderPass m_renderPass;
        std::vector<VkFramebuffer> m_frameBuffers;

        VkSemaphore m_presentSemaphore, m_renderSemaphore;
        VkFence m_renderFence;

        uint64_t m_frameNumber;

        uint32_t m_width, m_height;

        //AAssetManager* m_assetManager;

        VkPipeline m_trianglePipeline;
        VkPipelineLayout  m_trianglePipelineLayout;
        DeletionQueue m_mainDeletionQueue;
    };
}