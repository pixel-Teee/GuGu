#include <pch.h>

#include "vulkan-backend.h"

#include <Renderer/containers.h>

#include <Renderer/misc.h>



namespace GuGu{

    namespace nvrhi::vulkan{
        template <typename T>
        using attachment_vector = ::GuGu::nvrhi::static_vector<T, ::GuGu::nvrhi::c_MaxRenderTargets + 1>; // render targets + depth

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
            Framebuffer* fb = new Framebuffer(m_Context);
            fb->desc = desc;
            fb->framebufferInfo = FramebufferInfoEx(desc);

            attachment_vector<VkAttachmentDescription2> attachmentDescs(desc.colorAttachments.size());
            attachment_vector<VkAttachmentReference2> colorAttachmentRefs(desc.colorAttachments.size());//note:use for render pass
            VkAttachmentReference2 depthAttachmentRef;

            static_vector<VkImageView, c_MaxRenderTargets + 1> attachmentViews;
            attachmentViews.resize(desc.colorAttachments.size());

            uint32_t numArraySlices = 0;
            for(uint32_t i = 0; i < desc.colorAttachments.size(); ++i)
            {
                const auto& rt = desc.colorAttachments[i];
                Texture* t = checked_cast<Texture*>(rt.texture);

                assert(fb->framebufferInfo.width == std::max(t->desc.width >> rt.subresources.baseMipLevel, 1u));
                assert(fb->framebufferInfo.height == std::max(t->desc.height >> rt.subresources.baseMipLevel, 1u));

                const VkFormat attachmentFormat = (rt.format == Format::UNKNOWN ? t->imageInfo.format : convertFormat(rt.format));

                attachmentDescs[i] = {};
                VkAttachmentDescription2 attachmentDescription2{};
                attachmentDescription2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
                attachmentDescription2.format = attachmentFormat;
                attachmentDescription2.samples = t->imageInfo.samples;
                attachmentDescription2.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                attachmentDescription2.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescription2.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentDescription2.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentDescs[i] = attachmentDescription2;

                colorAttachmentRefs[i] = {};
                VkAttachmentReference2 attachmentReference2{};
                attachmentReference2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                attachmentReference2.attachment = i;
                attachmentReference2.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentRefs[i] = attachmentReference2;

                TextureSubresourceSet subresources = rt.subresources.resolve(t->desc, true);

                //todo:add more logic

                TextureDimension  dimension;
                //dimension =  = getDimensionForFramebuffer(t->desc.dimension, subresources.numArraySlices >> 1);//todo:fix this
                const auto& view = t->getSubresourceView(subresources, dimension, rt.format);
                attachmentViews[i] = view.view;

                fb->resources.push_back(rt.texture);
                if(numArraySlices)
                    assert(numArraySlices == subresources.numArraySlices);
                else
                    numArraySlices = subresources.numArraySlices;
            }

            //todo:add more logic

            return FramebufferHandle::Create(fb);
        }

    }
}