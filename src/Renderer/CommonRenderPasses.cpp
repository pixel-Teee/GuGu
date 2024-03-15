#include <pch.h>

#include "CommonRenderPasses.h"
#include "nvrhi.h"
#include "BindingCache.h"
#include "ShaderFactory.h"

namespace GuGu{
	static bool IsSupportedBlitDimension(nvrhi::TextureDimension dimension)
	{
		return dimension == nvrhi::TextureDimension::Texture2D
			|| dimension == nvrhi::TextureDimension::Texture2DArray
			|| dimension == nvrhi::TextureDimension::TextureCube
			|| dimension == nvrhi::TextureDimension::TextureCubeArray;
	}
    CommonRenderPasses::CommonRenderPasses(nvrhi::IDevice *device,
                                           std::shared_ptr <ShaderFactory> shaderFactory)
                                           : m_Device(device){

		m_RectVS = shaderFactory->CreateShader("asset/shader/rectVs", "main", nullptr, nvrhi::ShaderType::Vertex);

		std::vector<ShaderMacro> blitMacros = { ShaderMacro("TEXTURE_ARRAY", "0") };
		m_BlitPS = shaderFactory->CreateShader("asset/shader/blitPs", "main", &blitMacros, nvrhi::ShaderType::Pixel);
		//m_SharpenPS = shaderFactory->CreateShader("asset/sharpen_ps", "main", &blitMacros, nvrhi::ShaderType::Pixel);

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

        {
            nvrhi::BindingLayoutDesc layoutDesc;
            layoutDesc.visibility = nvrhi::ShaderType::All;
            layoutDesc.bindings = {
                nvrhi::BindingLayoutItem::PushConstants(0, sizeof(BlitConstants)),
				nvrhi::BindingLayoutItem::Texture_SRV(0),
				nvrhi::BindingLayoutItem::Sampler(0)
            };

            m_BlitBindingLayout = m_Device->createBindingLayout(layoutDesc);
        }
    }
    void CommonRenderPasses::BlitTexture(nvrhi::ICommandList* commandList, const BlitParameters& params, BindingCache* bindingCache)
    {
        const nvrhi::FramebufferDesc& targetFramebufferDesc = params.targetFramebuffer->getDesc();
        assert(targetFramebufferDesc.colorAttachments.size() == 1);
        assert(targetFramebufferDesc.colorAttachments[0].valid());
        assert(!targetFramebufferDesc.depthAttachment.valid());

        const nvrhi::FramebufferInfoEx& fbInfo = params.targetFramebuffer->getFramebufferInfo();
        const nvrhi::TextureDesc& sourceDesc = params.sourceTexture->getDesc();

        //todo:add check texture array
        assert(IsSupportedBlitDimension(sourceDesc.dimension));

        nvrhi::Viewport targetViewport = params.targetViewport;
        if (targetViewport.width() == 0 && targetViewport.height() == 0)
        {
            targetViewport = nvrhi::Viewport(float(fbInfo.width), float(fbInfo.height));
        }

        nvrhi::IShader* shader = nullptr;
        switch (params.sampler)
        {
        case BlitSampler::Point:
        case BlitSampler::Linear: shader = m_BlitPS; break;
        case BlitSampler::Sharpen: shader = m_SharpenPS; break;
        default: assert(false);
        }

        nvrhi::GraphicsPipelineHandle& pso = m_BlitPsoCache[PsoCacheKey{ fbInfo, shader, params.blendState }];
        if (!pso)
        {
            nvrhi::GraphicsPipelineDesc psoDesc;
            psoDesc.bindingLayouts = { m_BlitBindingLayout };
            psoDesc.VS = m_RectVS;
            psoDesc.PS = shader;
            psoDesc.primType = nvrhi::PrimitiveType::TriangleStrip;
            psoDesc.renderState.rasterState.setCullNone();
            psoDesc.renderState.depthStencilState.depthTestEnable = false;
            psoDesc.renderState.depthStencilState.stencilEnable = false;
            psoDesc.renderState.blendState.targets[0] = params.blendState;

            pso = m_Device->createGraphicsPipeline(psoDesc, params.targetFramebuffer);
        }

        nvrhi::BindingSetDesc bindingSetDesc;
        {
            auto sourceDimension = sourceDesc.dimension;
			if (sourceDimension == nvrhi::TextureDimension::TextureCube || sourceDimension == nvrhi::TextureDimension::TextureCubeArray)
				sourceDimension = nvrhi::TextureDimension::Texture2DArray;

			auto sourceSubresources = nvrhi::TextureSubresourceSet(params.sourceMip, 1, params.sourceArraySlice, 1);

			bindingSetDesc.bindings = {
				nvrhi::BindingSetItem::PushConstants(0, sizeof(BlitConstants)),
				nvrhi::BindingSetItem::Texture_SRV(0, params.sourceTexture, params.sourceFormat, sourceSubresources, sourceDimension),
				nvrhi::BindingSetItem::Sampler(0, params.sampler == BlitSampler::Point ? m_PointClampSampler : m_LinearClampSampler)
			};
        }

        nvrhi::BindingSetHandle sourceBindingSet;
        if (bindingCache)
            sourceBindingSet = bindingCache->GetOrCreateBindingSet(bindingSetDesc, m_BlitBindingLayout);
        else
            sourceBindingSet = m_Device->createBindingSet(bindingSetDesc, m_BlitBindingLayout);

        nvrhi::GraphicsState state;
        state.pipeline = pso;
        state.framebuffer = params.targetFramebuffer;
        state.bindings = { sourceBindingSet };
        state.viewport.addViewport(targetViewport);
        state.viewport.addScissorRect(nvrhi::Rect(targetViewport));
        state.blendConstantColor = params.blendConstantColor;

        BlitConstants blitConstants = {};
        blitConstants.sourceOrigin = math::float2(params.sourceBox.m_mins);
        blitConstants.sourceSize = params.sourceBox.diagonal();
        blitConstants.targetOrigin = math::float2(params.targetBox.m_mins);
        blitConstants.targetSize = params.targetBox.diagonal();

        commandList->setGraphicsState(state);
        commandList->setPushConstants(&blitConstants, sizeof(blitConstants));

        nvrhi::DrawArguments args;
        args.instanceCount = 1;
        args.vertexCount = 4;
        commandList->draw(args);
    }
    void CommonRenderPasses::BlitTexture(nvrhi::ICommandList* commandList, nvrhi::IFramebuffer* targetFramebuffer, nvrhi::ITexture* sourceTexture, BindingCache* bindingCache)
    {
        BlitParameters params;
        params.targetFramebuffer = targetFramebuffer;
        params.sourceTexture = sourceTexture;
        BlitTexture(commandList, params, bindingCache);
    }
}