#pragma once

#include <Renderer/DeviceManager.h>//IRenderPass

#include "AtlasTexture.h"

namespace GuGu {
	class Brush;
	class StyleSet;

	struct TextureData;
	class AtlasTexture;
	class TextureCache;
	
	class Widget;
	class WindowWidget;
	class TextBlockWidget;

	class ElementList;
	class WidgetGeometry;

	class FontCache;

	class RootFileSystem;
	class UIRenderPass : public IRenderPass
	{
	public:
		//UIRenderPass();
		using IRenderPass::IRenderPass;

		virtual ~UIRenderPass();

		bool Init();

		virtual void Render(nvrhi::IFramebuffer* framebuffer);

		virtual void Animate(float fElapsedTimeSeconds);
		
		virtual void BackBufferResizing();
		
		virtual void BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount);

	private:
		void loadStyleTextures();

		void updateTextAtlasTexture();	

		uint32_t m_atlasSize = 1024;

		nvrhi::CommandListHandle m_CommandList;

		std::shared_ptr<StyleSet> m_styles;

		std::shared_ptr<AtlasTexture> m_atlasTexture;

		//std::vector<uint8_t> m_textureAtlasData;
		std::shared_ptr<TextureCache> m_textureCache;

		nvrhi::SamplerHandle m_pointWrapSampler;
		nvrhi::ShaderHandle m_vertexShader;
		nvrhi::ShaderHandle m_pixelShader;
		nvrhi::ShaderHandle m_pixelFontShader;

		nvrhi::InputLayoutHandle m_inputLayout;
		nvrhi::BindingLayoutHandle m_bindingLayout;
		nvrhi::BindingSetHandle m_bindingSet;
		nvrhi::GraphicsPipelineHandle m_pipeline;
		nvrhi::GraphicsPipelineHandle m_FontPipeline;
		struct ConstantBufferEntry {
			dm::float4x4 viewProjMatrix;
			float padding[16 * 3];
		};

		std::shared_ptr<WindowWidget> m_uiRoot;
		std::shared_ptr<ElementList> m_elementList;
		std::vector<nvrhi::BufferHandle> m_VertexBuffers;
		std::vector<nvrhi::BufferHandle> m_IndexBuffers;
		std::vector<nvrhi::BufferHandle> m_constantBuffers;

		void calculateWidgetsFixedSize(std::shared_ptr<WindowWidget> widget);

		void generateWidgetElement(WidgetGeometry& allocatedWidgetGeometry);

		std::shared_ptr<TextBlockWidget> m_textBlockWidget;

		std::shared_ptr<RootFileSystem> m_rootFileSystem;
		//std::shared_ptr<FontCache> m_fontCache;
	};
}