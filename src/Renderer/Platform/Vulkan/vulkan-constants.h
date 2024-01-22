#pragma once

#include <Renderer/nvrhi.h>



namespace GuGu{
    namespace nvrhi::vulkan{
        VkFormat convertFormat(nvrhi::Format format);
    }
}