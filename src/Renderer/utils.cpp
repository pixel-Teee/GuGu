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
    }
}