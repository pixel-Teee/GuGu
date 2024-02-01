#pragma once

#include <Renderer/nvrhi.h>
#include <memory>
#include <unordered_map>

namespace GuGu{
    namespace nvrhi{
        struct BufferStateExtension
        {
            const BufferDesc& descRef;
            ResourceStates permanentState = ResourceStates::Unknown;

            explicit BufferStateExtension(const BufferDesc& desc)
                    : descRef(desc)
            { }
        };

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

        bool verifyPermanentResourceState(ResourceStates permanentState, ResourceStates requiredState, bool isTexture, const GuGuUtf8Str& debugName, IMessageCallback* messageCallback);
    }
}