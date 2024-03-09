#include <pch.h>

#include "CommonRenderPasses.h"
#include "nvrhi.h"

namespace GuGu{

    CommonRenderPasses::CommonRenderPasses(nvrhi::IDevice *device,
                                           std::shared_ptr <ShaderFactory> shaderFactory)
                                           : m_Device(device){

        auto samplerDesc = nvrhi::SamplerDesc()
                .setAllFilters(false)
                .setAllAddressModes(nvrhi::SamplerAddressMode::Clamp);
        m_PointClampSampler = m_Device->createSampler(samplerDesc);

        samplerDesc.setAllFilters(true);
        m_LinearClampSampler = m_Device->createSampler(samplerDesc);

        samplerDesc.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);
        m_LinearWrapSampler = m_Device->createSampler(samplerDesc);

        samplerDesc.setMaxAnisotropy(16);
        m_AnisotropicWrapSampler = m_Device->createSampler(samplerDesc);
    }
    void CommonRenderPasses::BlitTexture(nvrhi::ICommandList* commandList, const BlitParameters& params, BindingCache* bindingCache)
    {
        const nvrhi::FramebufferDesc& targetFramebufferDesc = params.targetFramebuffer->getDesc();

        const nvrhi::FramebufferInfoEx& fbInfo = params.targetFramebuffer->getFramebufferInfo();
        const nvrhi::TextureDesc& sourceDesc = params.sourceTexture->getDesc();

        //todo:add check texture array

        nvrhi::Viewport targetViewport = params.targetViewport;
        if (targetViewport.width() == 0 && targetViewport.height() == 0)
        {
            targetViewport = nvrhi::Viewport(float(fbInfo.width), float(fbInfo.height));
        }

        //todo:add more logic
    }
    void CommonRenderPasses::BlitTexture(nvrhi::ICommandList* commandList, nvrhi::IFramebuffer* targetFramebuffer, nvrhi::ITexture* sourceTexture, BindingCache* bindingCache)
    {
        BlitParameters params;
        params.targetFramebuffer = targetFramebuffer;
        params.sourceTexture = sourceTexture;
        BlitTexture(commandList, params, bindingCache);
    }
}