#include <pch.h>

#include "vulkan-backend.h"
#include "vk_types.h"
#include "VulkanRenderer.h"

#include <Renderer/containers.h>

#include <Renderer/misc.h>



namespace GuGu{

    namespace nvrhi::vulkan{
        template <typename T>
        using attachment_vector = ::GuGu::nvrhi::static_vector<T, ::GuGu::nvrhi::c_MaxRenderTargets + 1>; // render targets + depth

        static TextureDimension getDimensionForFramebuffer(TextureDimension dimension, bool isArray)
        {
            // Can't render into cubes and 3D textures directly, convert them to 2D arrays
            if (dimension == TextureDimension::TextureCube || dimension == TextureDimension::TextureCubeArray || dimension == TextureDimension::Texture3D)
                dimension = TextureDimension::Texture2DArray;

            if (!isArray)
            {
                // Demote arrays to single textures if we just need one layer
                switch(dimension)  // NOLINT(clang-diagnostic-switch-enum)
                {
                    case TextureDimension::Texture1DArray:
                        dimension = TextureDimension::Texture1D;
                        break;
                    case TextureDimension::Texture2DArray:
                        dimension = TextureDimension::Texture2D;
                        break;
                    case TextureDimension::Texture2DMSArray:
                        dimension = TextureDimension::Texture2DMS;
                        break;
                    default:
                        break;
                }
            }

            return dimension;
        }

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

                TextureDimension  dimension;
                dimension = getDimensionForFramebuffer(t->desc.dimension, subresources.numArraySlices >> 1);//todo:fix this
                const auto& view = t->getSubresourceView(subresources, dimension, rt.format);
                attachmentViews[i] = view.view;

                fb->resources.push_back(rt.texture);
                if(numArraySlices)
                    assert(numArraySlices == subresources.numArraySlices);
                else
                    numArraySlices = subresources.numArraySlices;
            }

            //add depth/stencil attachment if present
            if(desc.depthAttachment.valid())
            {
                const auto& att = desc.depthAttachment;

                Texture* texture = checked_cast<Texture*>(att.texture);
                assert(fb->framebufferInfo.width == std::max(texture->desc.width >> att.subresources.baseMipLevel, 1u));
                assert(fb->framebufferInfo.height == std::max(texture->desc.height >> att.subresources.baseMipLevel, 1u));

                VkImageLayout depthLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                if(desc.depthAttachment.isReadOnly)
                {
                    depthLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                }

                VkAttachmentDescription2 attachmentDescription2 = {};
                attachmentDescription2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
                attachmentDescription2.format = texture->imageInfo.format;
                attachmentDescription2.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                attachmentDescription2.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescription2.initialLayout = depthLayout;
                attachmentDescription2.finalLayout = depthLayout;

                VkAttachmentReference2 attachmentReference2{};
                attachmentReference2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                attachmentReference2.attachment = uint32_t(attachmentDescs.size() - 1);
                attachmentReference2.layout = depthLayout;

                TextureSubresourceSet subresources = att.subresources.resolve(texture->desc, true);
                TextureDimension dimension = getDimensionForFramebuffer(texture->desc.dimension, subresources.numArraySlices > 1);

                const auto& view = texture->getSubresourceView(subresources, dimension, att.format);
                attachmentViews.push_back(view.view);

                fb->resources.push_back(att.texture);

                if (numArraySlices)
                    assert(numArraySlices == subresources.numArraySlices);
                else
                    numArraySlices = subresources.numArraySlices;
            }

            VkSubpassDescription2 subpassDescription2 = {};
            subpassDescription2.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
            subpassDescription2.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescription2.colorAttachmentCount = uint32_t(desc.colorAttachments.size());
            subpassDescription2.pColorAttachments = colorAttachmentRefs.data();
            subpassDescription2.pDepthStencilAttachment = desc.depthAttachment.valid() ? &depthAttachmentRef : nullptr;

            //add vrs attachment
            //declare the structures here to avoid using pointers to out-of-scope objects in renderPassInfo further
            VkAttachmentReference2 vrsAttachmentRef = {};
            VkFragmentShadingRateAttachmentInfoKHR shadingRateAttachmentInfoKhr{};

            if(desc.shadingRateAttachment.valid())
            {
                const auto& vrsAttachment = desc.shadingRateAttachment;
                Texture* vrsTexture = checked_cast<Texture*>(vrsAttachment.texture);
                assert(vrsTexture->imageInfo.format == VK_FORMAT_R8_UINT);
                assert(vrsTexture->imageInfo.samples == VK_SAMPLE_COUNT_1_BIT);

                VkAttachmentDescription2 vrsAttachmentDesc = {};
                vrsAttachmentDesc.format = VK_FORMAT_R8_UINT;
                vrsAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
                vrsAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                vrsAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                vrsAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
                vrsAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

                attachmentDescs.push_back(vrsAttachmentDesc);

                TextureSubresourceSet subresources = vrsAttachment.subresources.resolve(vrsTexture->desc, true);
                TextureDimension dimension = getDimensionForFramebuffer(vrsTexture->desc.dimension, subresources.numArraySlices > 1);

                const auto& view = vrsTexture->getSubresourceView(subresources, dimension, vrsAttachment.format);
                attachmentViews.push_back(view.view);

                fb->resources.push_back(vrsAttachment.texture);

                if (numArraySlices)
                    assert(numArraySlices == subresources.numArraySlices);
                else
                    numArraySlices = subresources.numArraySlices;

                VkPhysicalDeviceFragmentShadingRatePropertiesKHR vkPhysicalDeviceFragmentShadingRatePropertiesKhr{};
                vkPhysicalDeviceFragmentShadingRatePropertiesKhr.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

                VkPhysicalDeviceProperties2 physicalDeviceProperties2 = {};
                physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
                physicalDeviceProperties2.pNext = &vkPhysicalDeviceFragmentShadingRatePropertiesKhr;

                vkGetPhysicalDeviceProperties2(m_Context.physicalDevice, &physicalDeviceProperties2);

                //auto rateProps = vk::PhysicalDeviceFragmentShadingRatePropertiesKHR();
                //auto props = vk::PhysicalDeviceProperties2();
                //props.pNext = &rateProps;
                //m_Context.physicalDevice.getProperties2(&props);

                VkAttachmentReference2 attachmentReference2{};
                attachmentReference2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                attachmentReference2.attachment = uint32_t (attachmentDescs.size()) - 1;
                attachmentReference2.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

                //vrsAttachmentRef = vk::AttachmentReference2()
                //        .setAttachment(uint32_t(attachmentDescs.size()) - 1)
                //        .setLayout(vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR);

                VkFragmentShadingRateAttachmentInfoKHR fragmentShadingRateAttachmentInfoKhr{};
                fragmentShadingRateAttachmentInfoKhr.sType = VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;
                fragmentShadingRateAttachmentInfoKhr.pFragmentShadingRateAttachment = &vrsAttachmentRef;
                fragmentShadingRateAttachmentInfoKhr.shadingRateAttachmentTexelSize = vkPhysicalDeviceFragmentShadingRatePropertiesKhr.minFragmentShadingRateAttachmentTexelSize;

                //shadingRateAttachmentInfo = vk::FragmentShadingRateAttachmentInfoKHR()
                //        .setPFragmentShadingRateAttachment(&vrsAttachmentRef)
                //        .setShadingRateAttachmentTexelSize(rateProps.minFragmentShadingRateAttachmentTexelSize);

                subpassDescription2.pNext = &shadingRateAttachmentInfoKhr;
            }

            VkRenderPassCreateInfo2 renderPassCreateInfo2 = {};
            renderPassCreateInfo2.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
            renderPassCreateInfo2.attachmentCount = uint32_t(attachmentDescs.size());
            renderPassCreateInfo2.pAttachments = attachmentDescs.data();
            renderPassCreateInfo2.subpassCount = 1;
            renderPassCreateInfo2.pSubpasses = &subpassDescription2;//subpass

            //todo:fix this
            PFN_vkCreateRenderPass2 vkCreateRenderPass2 = (PFN_vkCreateRenderPass2) vkGetInstanceProcAddr(m_Context.instance, "vkCreateRenderPass2");
            VkResult result = vkCreateRenderPass2(m_Context.device, &renderPassCreateInfo2, m_Context.allocationCallbacks, &fb->renderPass);

            VK_CHECK(result);

            VkFramebufferCreateInfo vkFramebufferCreateInfo = {};
            vkFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            vkFramebufferCreateInfo.renderPass = fb->renderPass;
            vkFramebufferCreateInfo.attachmentCount = uint32_t(attachmentViews.size());
            vkFramebufferCreateInfo.pAttachments = attachmentViews.data();
            vkFramebufferCreateInfo.width = fb->framebufferInfo.width;
            vkFramebufferCreateInfo.height = fb->framebufferInfo.height;
            vkFramebufferCreateInfo.layers = numArraySlices;


            result = vkCreateFramebuffer(m_Context.device, &vkFramebufferCreateInfo, m_Context.allocationCallbacks, &fb->framebuffer);

            VK_CHECK(result);

            return FramebufferHandle::Create(fb);
        }

    }
}