#include <pch.h>

#include "utils.h"

#include <Core/GuGuUtf8Str.h>

//#include <algorithm>

namespace GuGu{
    namespace nvrhi::utils{


        BitSetAllocator::BitSetAllocator(size_t capacity, bool multithreaded)
        : m_MultiThreaded(multithreaded){

            m_Allocated.resize(capacity);
        }

        int32_t BitSetAllocator::allocate() {
            if(m_MultiThreaded)
                m_Mutex.lock();

            int32_t result = -1;

            int32_t capacity = static_cast<int32_t>(m_Allocated.size());
            for(int32_t i = 0; i < capacity; ++i)
            {
                int32_t ii = (m_NextAvailable + i) % capacity;

                if(!m_Allocated[ii])
                {
                    result = ii;
                    m_NextAvailable = (ii + 1) % capacity;
                    m_Allocated[ii] = true;
                    break;
                }
            }

            if(m_MultiThreaded)
                m_Mutex.unlock();

            return result;
        }

        void BitSetAllocator::release(int32_t index) {
            if (index >= 0 && index < static_cast<int>(m_Allocated.size()))
            {
                if (m_MultiThreaded)
                    m_Mutex.lock();

                m_Allocated[index] = false;
                m_NextAvailable = std::min(m_NextAvailable, index);//todo:what this mean?

                if (m_MultiThreaded)
                    m_Mutex.unlock();
            }
        }

        void InvalidEnum()
        {
            assert(!"Invalid Enumeration Value");  // NOLINT(clang-diagnostic-string-conversion)
        }

        void NotSupported()
        {
            assert(!"Not Supported");  // NOLINT(clang-diagnostic-string-conversion)
        }

        const char* DebugNameToString(const GuGuUtf8Str& debugName)
        {
            return debugName.len() == 0 ? "<UNNAMED>" : debugName.getStr();
        }

        BufferDesc CreateStaticConstantBufferDesc(uint32_t byteSize, const char *debugName) {
            BufferDesc constantBufferDesc;
            constantBufferDesc.byteSize = byteSize;
            constantBufferDesc.debugName = debugName;
            constantBufferDesc.isConstantBuffer = true;
            constantBufferDesc.isVolatile = false;
            return constantBufferDesc;
        }

        void ClearColorAttachment(ICommandList* commandList, IFramebuffer* framebuffer, uint32_t attachmentIndex, Color color)
        {
            const FramebufferAttachment& att = framebuffer->getDesc().colorAttachments[attachmentIndex];
            if (att.texture)
            {
                commandList->clearTextureFloat(att.texture, att.subresources, color);
            }
        }

        bool CreateBindingSetAndLayout(IDevice *device, nvrhi::ShaderType visibility,
                                       uint32_t registerSpace, const BindingSetDesc &bindingSetDesc,
                                       BindingLayoutHandle &bindingLayout,
                                       BindingSetHandle &bindingSet) {
            //note:convert set to layout
            auto convertSetToLayout = [](const BindingSetItemArray& setDesc, BindingLayoutItemArray& layoutDesc)
            {
                for (auto& item : setDesc)
                {
                    BindingLayoutItem layoutItem{};
                    layoutItem.slot = item.slot;
                    layoutItem.type = item.type;
                    if (item.type == ResourceType::PushConstants)
                        layoutItem.size = uint32_t(item.range.byteSize);
                    layoutDesc.push_back(layoutItem);
                }
            };

            if (!bindingLayout)
            {
                nvrhi::BindingLayoutDesc bindingLayoutDesc;
                bindingLayoutDesc.visibility = visibility;
                bindingLayoutDesc.registerSpace = registerSpace;
                convertSetToLayout(bindingSetDesc.bindings, bindingLayoutDesc.bindings);

                bindingLayout = device->createBindingLayout(bindingLayoutDesc);

                if (!bindingLayout)
                    return false;
            }

            if (!bindingSet)
            {
                bindingSet = device->createBindingSet(bindingSetDesc, bindingLayout);

                if (!bindingSet)
                    return false;
            }

            return true;
        }
		Format ChooseFormat(IDevice* device, nvrhi::FormatSupport requiredFeatures, const nvrhi::Format* requestedFormats, size_t requestedFormatCount)
		{
			assert(device);
			assert(requestedFormats || requestedFormatCount == 0);

			for (size_t i = 0; i < requestedFormatCount; i++)
			{
				if ((device->queryFormatSupport(requestedFormats[i]) & requiredFeatures) == requiredFeatures)
					return requestedFormats[i];
			}

			return Format::UNKNOWN;
		}
    }
}