#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan{
        Shader::~Shader()
        {
            if (shaderModule && !baseShader) // do not destroy the module if this is a derived specialization shader or a library entry
            {
                vkDestroyShaderModule(m_Context.device, shaderModule, m_Context.allocationCallbacks);
                shaderModule = VK_NULL_HANDLE;
                //m_Context.device.destroyShaderModule(shaderModule, m_Context.allocationCallbacks);
                //shaderModule = vk::ShaderModule();
            }
        }

        void Shader::getBytecode(const void** ppBytecode, size_t* pSize) const
        {
            // we don't save these for vulkan
            if (ppBytecode) *ppBytecode = nullptr;
            if (pSize) *pSize = 0;
        }

        Object Shader::getNativeObject(ObjectType objectType)
        {
            switch (objectType)
            {
                case ObjectTypes::VK_ShaderModule:
                    return Object(shaderModule);
                default:
                    return nullptr;
            }
        }
    }
}