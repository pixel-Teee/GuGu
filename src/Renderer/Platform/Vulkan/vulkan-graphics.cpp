#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan{
        Framebuffer::~Framebuffer() {
            if (framebuffer && managed)
            {
                vkDestroyFramebuffer(m_Context.device, framebuffer, nullptr);

                framebuffer = VK_NULL_HANDLE;
            }

            if (renderPass && managed)
            {
                vkDestroyRenderPass(m_Context.device, renderPass, nullptr);
                //m_Context.device.destroyRenderPass(renderPass);
                renderPass = VK_NULL_HANDLE;
            }
        }

        Object Framebuffer::getNativeObject(ObjectType objectType) {
            switch (objectType)
            {
                case ObjectTypes::VK_RenderPass:
                    return Object(renderPass);
                case ObjectTypes::VK_Framebuffer:
                    return Object(framebuffer);
                default:
                    return nullptr;
            }
        }


        FramebufferHandle Device::createFramebuffer(const FramebufferDesc &desc) {
            return NULL;//todo:implement this
        }
    }
}