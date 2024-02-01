#include <pch.h>

#include "state-tracking.h"

#include <sstream>

#include <Renderer/utils.h>

namespace GuGu{
    namespace nvrhi{
        bool verifyPermanentResourceState(ResourceStates permanentState, ResourceStates requiredState, bool isTexture, const GuGuUtf8Str& debugName, IMessageCallback* messageCallback)
        {
            if ((permanentState & requiredState) != requiredState)
            {
                std::stringstream ss;
                ss << "Permanent " << (isTexture ? "texture " : "buffer ") << utils::DebugNameToString(debugName)
                   << " doesn't have the right state bits. Required: 0x" << std::hex << uint32_t(requiredState)
                   << ", present: 0x" << std::hex << uint32_t(permanentState);
                messageCallback->message(MessageSeverity::Error, ss.str().c_str());
                return false;
            }

            return true;
        }
    }
}