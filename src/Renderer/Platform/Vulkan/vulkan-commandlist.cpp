#include <pch.h>

#include "vulkan-backend.h"

namespace GuGu{
    namespace nvrhi::vulkan{
        CommandList::CommandList(Device *device, const VulkanContext &context,
                                 const CommandListParameters &parameters)
        : m_Device(device)
        , m_Context(context)
        , m_CommandListParameters(parameters){
            //todo:add state tracker and upload manager

        }

        void CommandList::open() {

        }

        void CommandList::close() {

        }
    }
}