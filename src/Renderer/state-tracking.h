#pragma once

#include <Renderer/nvrhi.h>
#include <memory>
#include <unordered_map>

namespace GuGu{
    namespace nvrhi{
        struct TextureStateExtension
        {
            const TextureDesc& descRef;
            ResourceStates permanentState = ResourceStates::Unknown;
            bool stateInitialized = false;

            explicit TextureStateExtension(const TextureDesc& desc)
                    : descRef(desc)
            { }
        };

        struct TextureState
        {
            std::vector<ResourceStates> subresourceStates;
            ResourceStates state = ResourceStates::Unknown;
            bool enableUavBarriers = true;
            bool firstUavBarrierPlaced = false;
            bool permanentTransition = false;
        };
    }
}