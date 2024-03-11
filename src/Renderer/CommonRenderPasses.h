#pragma once


#include <Renderer/nvrhi.h>
#include <Core/Math/MyMath.h>

#include <unordered_map>

namespace GuGu{
    class BindingCache;
    class ShaderFactory;

    struct BlitConstants
    {
		math::float2 sourceOrigin;
		math::float2 sourceSize;

        math::float2 targetOrigin;
        math::float2 targetSize;

        math::float2 sharpenFactor;
    };  

	enum class BlitSampler
	{
		Point,
		Linear,
		Sharpen
	};

    struct BlitParameters
    {
        nvrhi::IFramebuffer* targetFramebuffer = nullptr;
        nvrhi::Viewport targetViewport;
        math::box2 targetBox = math::box2(0.0f, 1.0f);

        nvrhi::ITexture* sourceTexture = nullptr;
        uint32_t sourceArraySlice = 0;
        uint32_t sourceMip = 0;
        math::box2 sourceBox = math::box2(0.0f, 1.0f);
        nvrhi::Format sourceFormat = nvrhi::Format::UNKNOWN;

        BlitSampler sampler = BlitSampler::Linear;
        nvrhi::BlendState::RenderTarget blendState;
        Color blendConstantColor = Color(0.0f);
    };
    class CommonRenderPasses
    {
    protected:
        nvrhi::DeviceHandle m_Device;

		struct PsoCacheKey
		{
			nvrhi::FramebufferInfo fbinfo;
			nvrhi::IShader* shader;
			nvrhi::BlendState::RenderTarget blendState;
			
			bool operator==(const PsoCacheKey& other) const { return fbinfo == other.fbinfo && shader == other.shader && blendState == other.blendState; }
			bool operator!=(const PsoCacheKey& other) const { return !(*this == other); }
			
			struct Hash
			{
				size_t operator ()(const PsoCacheKey& s) const
				{
					size_t hash = 0;
					nvrhi::hash_combine(hash, s.fbinfo);
					nvrhi::hash_combine(hash, s.shader);
					nvrhi::hash_combine(hash, s.blendState);
					return hash;
				}
			};
		};
		
		std::unordered_map<PsoCacheKey, nvrhi::GraphicsPipelineHandle, PsoCacheKey::Hash> m_BlitPsoCache;

    public:
        //nvrhi::ShaderHandle m_FullscreenVS;
        //nvrhi::ShaderHandle m_FullscreenAtOneVS;
        nvrhi::ShaderHandle m_RectVS;
        nvrhi::ShaderHandle m_BlitPS;
        //nvrhi::ShaderHandle m_BlitArrayPS;
        nvrhi::ShaderHandle m_SharpenPS;
        //nvrhi::ShaderHandle m_SharpenArrayPS;
//
        //nvrhi::TextureHandle m_BlackTexture;
        //nvrhi::TextureHandle m_GrayTexture;
        //nvrhi::TextureHandle m_WhiteTexture;
        //nvrhi::TextureHandle m_BlackTexture2DArray;
        //nvrhi::TextureHandle m_WhiteTexture2DArray;
        //nvrhi::TextureHandle m_BlackCubeMapArray;

        nvrhi::SamplerHandle m_PointClampSampler;
        nvrhi::SamplerHandle m_LinearClampSampler;
        nvrhi::SamplerHandle m_LinearWrapSampler;
        nvrhi::SamplerHandle m_AnisotropicWrapSampler;

        nvrhi::BindingLayoutHandle m_BlitBindingLayout;

        CommonRenderPasses(nvrhi::IDevice* device, std::shared_ptr<ShaderFactory> shaderFactory);

        void BlitTexture(nvrhi::ICommandList* commandList, const BlitParameters& params, BindingCache* bindingCache = nullptr);
//
        // Simplified form of BlitTexture that blits the entire source texture, mip 0 slice 0, into the entire target framebuffer using a linear sampler.
        void BlitTexture(nvrhi::ICommandList* commandList, nvrhi::IFramebuffer* targetFramebuffer, nvrhi::ITexture* sourceTexture, BindingCache* bindingCache = nullptr);
    };
}