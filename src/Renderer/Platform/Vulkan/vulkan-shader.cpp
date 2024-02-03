#include <pch.h>

#include "vulkan-backend.h"
#include "vk_types.h"


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

        ShaderHandle Device::createShader(const ShaderDesc& desc, const void *binary, const size_t binarySize)
        {
            Shader *shader = new Shader(m_Context);

            shader->desc = desc;
            shader->stageFlagBits = convertShaderTypeToShaderStageFlagBits(desc.shaderType);

            VkShaderModuleCreateInfo shaderInfo = {};
            shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderInfo.codeSize = binarySize;
            shaderInfo.pCode = (const uint32_t*)binary;

            //auto shaderInfo = vk::ShaderModuleCreateInfo()
            //        .setCodeSize(binarySize)
            //        .setPCode((const uint32_t *)binary);

            VkResult res = vkCreateShaderModule(m_Context.device, &shaderInfo, m_Context.allocationCallbacks, &shader->shaderModule);

            VK_CHECK(res);
            //const vk::Result res = m_Context.device.createShaderModule(&shaderInfo, m_Context.allocationCallbacks, &shader->shaderModule);
            //CHECK_VK_FAIL(res)

            GuGuUtf8Str debugName = desc.debugName;
            debugName.append(":");
            debugName.append(desc.entryName);
            m_Context.nameVKObject((void*)VkShaderModule(shader->shaderModule), VK_OBJECT_TYPE_SHADER_MODULE, debugName.getStr());
            //const std::string debugName = desc.debugName + ":" + desc.entryName;
            //m_Context.nameVKObject(VkShaderModule(shader->shaderModule), vk::DebugReportObjectTypeEXT::eShaderModule, debugName.c_str());
//
            return ShaderHandle::Create(shader);
        }


        InputLayoutHandle
        Device::createInputLayout(const VertexAttributeDesc* attributeDesc, uint32_t attributeCount, IShader* vertexShader) {
            (void)vertexShader;

            InputLayout *layout = new InputLayout();

            int total_attribute_array_size = 0;

            // collect all buffer bindings
            std::unordered_map<uint32_t, VkVertexInputBindingDescription> bindingMap;
            for (uint32_t i = 0; i < attributeCount; i++)
            {
                const VertexAttributeDesc& desc = attributeDesc[i];

                assert(desc.arraySize > 0);

                total_attribute_array_size += desc.arraySize;

                if (bindingMap.find(desc.bufferIndex) == bindingMap.end())
                {
                    VkVertexInputBindingDescription vertexInputBindingDescription = {};
                    vertexInputBindingDescription.binding = desc.bufferIndex;
                    vertexInputBindingDescription.stride = desc.elementStride; //sizeof(Vertex)
                    vertexInputBindingDescription.inputRate = desc.isInstanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;

                    bindingMap[desc.bufferIndex] = vertexInputBindingDescription;

                    //bindingMap[desc.bufferIndex] = vk::VertexInputBindingDescription()
                    //        .setBinding(desc.bufferIndex)
                    //        .setStride(desc.elementStride)
                    //        .setInputRate(desc.isInstanced ? vk::VertexInputRate::eInstance : vk::VertexInputRate::eVertex);
                }
                else {
                    assert(bindingMap[desc.bufferIndex].stride == desc.elementStride);
                    assert(bindingMap[desc.bufferIndex].inputRate == (desc.isInstanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX));
                }
            }

            for (const auto& b : bindingMap)
            {
                layout->bindingDesc.push_back(b.second);
            }

            // build attribute descriptions
            layout->inputDesc.resize(attributeCount);
            layout->attributeDesc.resize(total_attribute_array_size);

            uint32_t attributeLocation = 0;
            for (uint32_t i = 0; i < attributeCount; i++)
            {
                const VertexAttributeDesc& in = attributeDesc[i];
                layout->inputDesc[i] = in;

                uint32_t element_size_bytes = getFormatInfo(in.format).bytesPerBlock;

                uint32_t bufferOffset = 0;

                for (uint32_t slot = 0; slot < in.arraySize; ++slot)
                {
                    auto& outAttrib = layout->attributeDesc[attributeLocation];

                    outAttrib.location = attributeLocation;
                    outAttrib.binding = in.bufferIndex;
                    outAttrib.format = vulkan::convertFormat(in.format);
                    outAttrib.offset = bufferOffset + in.offset;
                    bufferOffset += element_size_bytes;

                    ++attributeLocation;
                }
            }

            return InputLayoutHandle::Create(layout);
        }



        uint32_t InputLayout::getNumAttributes() const
        {
            return uint32_t(inputDesc.size());
        }

        const VertexAttributeDesc* InputLayout::getAttributeDesc(uint32_t index) const
        {
            if (index < uint32_t(inputDesc.size()))
                return &inputDesc[index];
            else
                return nullptr;
        }
    }
}