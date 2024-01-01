#include <pch.h>

#include "VulkanRenderer.h"

//---other includes---
#include "vk_types.h"
#include "vk_initializers.h"

#include "VkBootstrap.h"

#include <Application/Platform/Android/AndroidApplication.h>
#include <Window/Platform/Android/AndroidWindow.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>//native window
#include <android/log.h>
#include <android/asset_manager_jni.h>

#define LOG_TAG "AndroidLog"
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

namespace GuGu{

    constexpr bool bUseValidationLayers = true;

    #define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
            ALOGD("Detected Vulkan error: ");                       \
            ALOGD("%d, %d", err, __LINE__ );                                       \
			abort();                                                \
		}                                                           \
	} while (0)

inline VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                             void*) {
    auto ms = vkb::to_string_message_severity(messageSeverity);
    auto mt = vkb::to_string_message_type(messageType);
    ALOGD("[%s: %s]\n%s\n", ms, mt, pCallbackData->pMessage);

    return VK_FALSE; // Applications must return false here
}

std::vector<uint8_t> LoadBinaryFileToVector(const char *file_path,
                                            AAssetManager *assetManager) {
    std::vector<uint8_t> file_content;
    assert(assetManager);
    AAsset *file =
            AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
    size_t file_length = AAsset_getLength(file);

    file_content.resize(file_length);

    AAsset_read(file, file_content.data(), file_length);
    AAsset_close(file);
    return file_content;
}
//#else
//    constexpr bool bUseValidationLayers = false;
//
//    #define VK_CHECK(x)
//#endif

    VulkanRenderer::VulkanRenderer() {

    }

    VulkanRenderer::~VulkanRenderer() {

    }

    void VulkanRenderer::init() {
        m_frameNumber = 0;
        initVulkan();
        initSwapchain();
        initCommands();
        initDefaultRenderPass();
        initFrameBuffers();
        initSyncStructures();
        initPipelines();
        //AndroidApplication::getApplication()->setExit(true);
    }

    void VulkanRenderer::onRender() {
        //wait until the GPU has finished rendering the last frame. Timeout of 1 second
        VK_CHECK(vkWaitForFences(m_device, 1, &m_renderFence, true, std::numeric_limits<uint64_t>::max()));
        VK_CHECK(vkResetFences(m_device, 1, &m_renderFence));

        //request image from the swapchain, one second timeout
        uint32_t swapChainImageIndex;
        VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapChain, std::numeric_limits<uint64_t>::max(), m_presentSemaphore, VK_NULL_HANDLE, &swapChainImageIndex));

        //now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
        VK_CHECK(vkResetCommandBuffer(m_mainCommandBuffer, 0));
        //naming it cmd for shorter writing
        VkCommandBuffer cmd = m_mainCommandBuffer;

        //begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
        VkCommandBufferBeginInfo cmdBeginInfo = {};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;

        cmdBeginInfo.pInheritanceInfo = nullptr;
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        //make a clear-color from frame number. This will flash with a 120*pi frame period.
        VkClearValue clearValue;
        float flash = abs(sin(m_frameNumber / 120.f));
        clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

        //start the main renderpass.
        //We will use the clear color from above, and the framebuffer of the index the swapchain gave us
        VkRenderPassBeginInfo rpInfo = {};
        rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpInfo.pNext = nullptr;

        rpInfo.renderPass = m_renderPass;
        rpInfo.renderArea.offset.x = 0;
        rpInfo.renderArea.offset.y = 0;
        //------width and height------
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
        vkb::SwapchainBuilder swapchainBuilder{m_chosenGPU, m_device, m_surface};
        //int32_t height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
        //int32_t width = ANativeWindow_getWidth(androidWindow->getNativeHandle());
        //------width and height------
        VkExtent2D  extent;
        extent.width = m_width;
        extent.height = m_height;
        rpInfo.renderArea.extent = extent;
        rpInfo.framebuffer = m_frameBuffers[swapChainImageIndex];

        //connect clear values
        rpInfo.clearValueCount = 1;
        rpInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_trianglePipeline);
        vkCmdDraw(cmd, 3, 1, 0, 0);

        //finalize the render pass
        vkCmdEndRenderPass(cmd);
        //finalize the command buffer (we can no longer add commands, but it can now be executed)
        VK_CHECK(vkEndCommandBuffer(cmd));

        //prepare the submission to the queue.
        //we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
        //we will signal the _renderSemaphore, to signal that rendering has finished

        VkSubmitInfo submit = {};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pNext = nullptr;

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submit.pWaitDstStageMask = &waitStage;

        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &m_presentSemaphore;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &m_renderSemaphore;

        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;

        //submit command buffer to the queue and execute it.
        // _renderFence will now block until the graphic commands finish execution
        VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, m_renderFence));

        // this will put the image we just rendered into the visible window.
        // we want to wait on the _renderSemaphore for that,
        // as it's necessary that drawing commands have finished before the image is displayed to the user
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;

        presentInfo.pSwapchains = &m_swapChain;
        presentInfo.swapchainCount = 1;

        presentInfo.pWaitSemaphores = &m_renderSemaphore;
        presentInfo.waitSemaphoreCount = 1;

        presentInfo.pImageIndices = &swapChainImageIndex;

        VK_CHECK(vkQueuePresentKHR(m_graphicsQueue, &presentInfo));

        //increase the number of frames drawn
        m_frameNumber++;

        ALOGD("%d", m_frameNumber);
    }

    void VulkanRenderer::onDestroy() {
        vkWaitForFences(m_device, 1, &m_renderFence, true, 1000000000);

        m_mainDeletionQueue.flush();

        vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        //todo:destroy debug messenger
        vkDestroyInstance(m_instance, nullptr);
    }

    void VulkanRenderer::initVulkan() {
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
        vkb::InstanceBuilder builder;

        //make the vulkan instance, with basic debug features
        auto instRet = builder.set_app_name("vulkan application")
                .request_validation_layers(bUseValidationLayers)
                //.use_default_debug_messenger() //this will cause error
                .set_debug_callback(default_debug_callback)
                .require_api_version(1, 1, 0)
                .build();

        vkb::Instance vkbInst = instRet.value();
        //grab the instance
        m_instance = vkbInst.instance;
        m_debugMessenger = vkbInst.debug_messenger;

        //vulkan 1.3 features
        //VkPhysicalDeviceVulkan13Features features{};
        //features.dynamicRendering = true;
        //features.synchronization2 = true;

        //vulkan 1.2 features
        //VkPhysicalDeviceVulkan12Features features12{};
        //features12.bufferDeviceAddress = true;
        //features12.descriptorIndexing = true;

        //create surface
        const VkAndroidSurfaceCreateInfoKHR create_info{
                .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .window = androidWindow->getNativeHandle()};

        VK_CHECK(vkCreateAndroidSurfaceKHR(m_instance, &create_info,
                                           nullptr /* pAllocator */, &m_surface));

       //use vkbootstrap to select a gpu
       //we want a gpu that can write to surface and supports vulkan 1.3 with the correct features
        vkb::PhysicalDeviceSelector selector{ vkbInst };
        vkb::PhysicalDevice physicalDevice = selector
                .set_minimum_version(1, 1)
                .set_surface(m_surface)
                .select()
                .value();

        //create the final vulkan device
        vkb::DeviceBuilder deviceBuilder{physicalDevice};
        vkb::Device vkbDevice = deviceBuilder.build().value();

        //get the VkDevice handle used in the rest of a vulkan application
        m_device = vkbDevice.device;
        m_chosenGPU = physicalDevice.physical_device;

        //use vkbootstrap to get a graphics queue
        m_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

        ++m_frameNumber;
    }

    void VulkanRenderer::initSwapchain() {
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
        vkb::SwapchainBuilder swapchainBuilder{m_chosenGPU, m_device, m_surface};
        m_height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
        m_width = ANativeWindow_getWidth(androidWindow->getNativeHandle());
        vkb::Swapchain vkbSwapchain = swapchainBuilder
                .use_default_format_selection()
                //use vsync present mode
                .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                .set_desired_extent(m_width, m_height)
                .build()
                .value();

        //store the swapchain and its related images
        m_swapChain = vkbSwapchain.swapchain;
        m_swapchainImages = vkbSwapchain.get_images().value();
        m_swapchainImageViews = vkbSwapchain.get_image_views().value();
        m_swapchainImageFormat = vkbSwapchain.image_format;

        m_mainDeletionQueue.push_function([=](){
            vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
        });
    }

    void VulkanRenderer::initCommands() {
        //create a command pool for commands submitted to the graphics queue.
        //we also want the pool to allow for resetting of individual command buffers
        VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool));

        //allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(m_commandPool, 1);

        VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_mainCommandBuffer));

        m_mainDeletionQueue.push_function([=]() {
            vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        });
    }

    void VulkanRenderer::initSyncStructures() {
        //create synchronization structures

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.pNext = nullptr;

        //we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_renderFence));

        m_mainDeletionQueue.push_function([=]() {
            vkDestroyFence(m_device, m_renderFence, nullptr);
        });


        //for the semaphores we don't need any flags
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = nullptr;
        semaphoreCreateInfo.flags = 0;

        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_presentSemaphore));
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_renderSemaphore));

        m_mainDeletionQueue.push_function([=]() {
            vkDestroySemaphore(m_device, m_presentSemaphore, nullptr);
            vkDestroySemaphore(m_device, m_renderSemaphore, nullptr);
        });
    }

    void VulkanRenderer::initDefaultRenderPass() {
        // the renderpass will use this color attachment.
        VkAttachmentDescription color_attachment = {};
        //the attachment will have the format needed by the swapchain
        color_attachment.format = m_swapchainImageFormat;
        //1 sample, we won't be doing MSAA
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        // we Clear when this attachment is loaded
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // we keep the attachment stored when the renderpass ends
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //we don't care about stencil
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        //we don't know or care about the starting layout of the attachment
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        //after the renderpass ends, the image has to be on a layout ready for display
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref = {};
        //attachment number will index into the pAttachments array in the parent renderpass itself
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        //we are going to create 1 subpass, which is the minimum you can do
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

        //connect the color attachment to the info
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &color_attachment;
        //connect the subpass to the info
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;


        VK_CHECK(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass));

        m_mainDeletionQueue.push_function([=](){
           vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        });
    }

    void VulkanRenderer::initFrameBuffers() {
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();
        std::shared_ptr<AndroidWindow> androidWindow = androidApplication->getPlatformWindow();
        vkb::SwapchainBuilder swapchainBuilder{m_chosenGPU, m_device, m_surface};
        //m_height = ANativeWindow_getHeight(androidWindow->getNativeHandle());
        //m_width = ANativeWindow_getWidth(androidWindow->getNativeHandle());

        //create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
        VkFramebufferCreateInfo fb_info = {};
        fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.pNext = nullptr;

        fb_info.renderPass = m_renderPass;
        fb_info.attachmentCount = 1;
        fb_info.width = m_width;
        fb_info.height = m_height;
        fb_info.layers = 1;

        //grab how many images we have in the swapchain
        const uint32_t swapchainImageCount = m_swapchainImages.size();
        m_frameBuffers = std::vector<VkFramebuffer>(swapchainImageCount);

        //create framebuffers for each of the swapchain image views
        //for (int i = 0; i < swapchainImageCount; i++) {
//
        //    fb_info.pAttachments = &m_swapchainImageViews[i];
        //    VK_CHECK(vkCreateFramebuffer(m_device, &fb_info, nullptr, &m_frameBuffers[i]));
        //}

        for (int i = 0; i < swapchainImageCount; i++) {

            fb_info.pAttachments = &m_swapchainImageViews[i];
            VK_CHECK(vkCreateFramebuffer(m_device, &fb_info, nullptr, &m_frameBuffers[i]));

            m_mainDeletionQueue.push_function([=]() {
                vkDestroyFramebuffer(m_device, m_frameBuffers[i], nullptr);
                vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
            });
        }
    }

    //void VulkanRenderer::setAssetManager(AAssetManager* assetManager) {
    //    m_assetManager = assetManager;
    //}

    bool VulkanRenderer::loadShaderModule(const char *filePath, VkShaderModule *outShaderModule) {
        std::shared_ptr<AndroidApplication> androidApplication = AndroidApplication::getApplication();

        std::vector<uint8_t> buffer = LoadBinaryFileToVector(filePath, androidApplication->getAssetManager());
        //create a new shader module, using the buffer we loaded
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;

        //codeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
        createInfo.codeSize = buffer.size() / 4 * sizeof(uint32_t);
        createInfo.pCode = reinterpret_cast<uint32_t*>(buffer.data());

        //check that the creation goes well.
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            return false;
        }
        *outShaderModule = shaderModule;
        return true;
    }

    void VulkanRenderer::initPipelines() {
        VkShaderModule triangleFragShader;
        if (!loadShaderModule("triangle.frag.spv", &triangleFragShader))
        {
            ALOGD("Error when building the triangle fragment shader module");
        }
        else {
            ALOGD("Triangle fragment shader successfully loaded");
        }

        VkShaderModule triangleVertexShader;
        if (!loadShaderModule("triangle.vert.spv", &triangleVertexShader))
        {
            ALOGD("Error when building the triangle vertex shader module");

        }
        else {
            ALOGD("Triangle vertex shader successfully loaded");
        }

        //build the pipeline layout that controls the inputs/outputs of the shader
        //we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
        VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();

        VK_CHECK(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_trianglePipelineLayout));

        //build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
        PipelineBuilder pipelineBuilder;

        pipelineBuilder._shaderStages.push_back(
                vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader));

        pipelineBuilder._shaderStages.push_back(
                vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));


        //vertex input controls how to read vertices from vertex buffers. We aren't using it yet
        pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

        //input assembly is the configuration for drawing triangle lists, strips, or individual points.
        //we are just going to draw triangle list
        pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        //build viewport and scissor from the swapchain extents
        pipelineBuilder._viewport.x = 0.0f;
        pipelineBuilder._viewport.y = 0.0f;
        pipelineBuilder._viewport.width = (float)m_width;
        pipelineBuilder._viewport.height = (float)m_height;
        pipelineBuilder._viewport.minDepth = 0.0f;
        pipelineBuilder._viewport.maxDepth = 1.0f;

        VkExtent2D extent;
        extent.width = m_width;
        extent.height = m_height;
        pipelineBuilder._scissor.offset = { 0, 0 };
        pipelineBuilder._scissor.extent = extent;

        //configure the rasterizer to draw filled triangles
        pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

        //we don't use multisampling, so just run the default one
        pipelineBuilder._multisampling = vkinit::multisampling_state_create_info();

        //a single blend attachment with no blending and writing to RGBA
        pipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

        //use the triangle layout we created
        pipelineBuilder._pipelineLayout = m_trianglePipelineLayout;

        //finally build the pipeline
        m_trianglePipeline = pipelineBuilder.build_pipeline(m_device, m_renderPass);

        //destroy all shader modules, outside of the queue
        //vkDestroyShaderModule(m_device, redTriangleVertShader, nullptr);
        //vkDestroyShaderModule(m_device, redTriangleFragShader, nullptr);
        vkDestroyShaderModule(m_device, triangleFragShader, nullptr);
        vkDestroyShaderModule(m_device, triangleVertexShader, nullptr);

        m_mainDeletionQueue.push_function([=]() {
            //destroy the 2 pipelines we have created
            //vkDestroyPipeline(m_device, m_redTrianglePipeline, nullptr);
            vkDestroyPipeline(m_device, m_trianglePipeline, nullptr);

            //destroy the pipeline layout that they use
            vkDestroyPipelineLayout(m_device, m_trianglePipelineLayout, nullptr);
        });
    }

    VkPipeline PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass pass) {
        //make viewport state from our stored viewport and scissor.
        //at the moment we won't support multiple viewports or scissors
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;

        viewportState.viewportCount = 1;
        viewportState.pViewports = &_viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &_scissor;

        //setup dummy color blending. We aren't using transparent objects yet
        //the blending is just "no blend", but we do write to the color attachment
        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;

        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &_colorBlendAttachment;

        //build the actual pipeline
        //we now use all of the info structs we have been writing into into this one to create the pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;

        pipelineInfo.stageCount = _shaderStages.size();
        pipelineInfo.pStages = _shaderStages.data();
        pipelineInfo.pVertexInputState = &_vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &_inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &_rasterizer;
        pipelineInfo.pMultisampleState = &_multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = _pipelineLayout;
        pipelineInfo.renderPass = pass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        //it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
        VkPipeline newPipeline;
        if (vkCreateGraphicsPipelines(
                device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
            std::cout << "failed to create pipeline\n";
            return VK_NULL_HANDLE; // failed to create graphics pipeline
        }
        else
        {
            return newPipeline;
        }
    }
}