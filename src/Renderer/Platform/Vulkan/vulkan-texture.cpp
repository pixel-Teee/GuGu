#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan{
        TextureHandle Device::createHandleForNativeTexture(ObjectType objectType, Object _texture, const TextureDesc& desc) {
            if (_texture.integer == 0)
                return nullptr;

            if (objectType != ObjectTypes::VK_Image)
                return nullptr;

            VkImage image{};
            image = (VkImage)_texture.integer;

            //todo:implement texture
            //Texture *texture = new Texture(m_Context, m_Allocator);
            //fillTextureInfo(texture, desc);
//
            //texture->image = image;
            //texture->managed = false;
//
            //return TextureHandle::Create(texture);
            return nullptr;
        }
    }
}