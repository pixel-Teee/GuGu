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
}