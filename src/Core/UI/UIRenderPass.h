#pragma once

#include <Renderer/DeviceManager.h>//IRenderPass

namespace GuGu {
	class Style;
	class TextureCache;
	struct TextureData;
	class Brush;
	class WindowWidget;
	class Widget;
	class ElementList;
	class WidgetGeometry;
	class FontCache;
	class RootFileSystem;
	struct AtlasedTextureSlot
	{
		uint32_t x;
		uint32_t y;
		uint32_t width;
		uint32_t height;
		uint32_t padding;
		AtlasedTextureSlot(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inPadding)
			: x(inX)
			, y(inY)
			, width(inWidth)
			, height(inHeight)
			, padding(inPadding)
		{}
	};
	struct FCopyRowData
	{
		//source data to copy
		const uint8_t* srcData;
		//place to copy data to
		uint8_t* destData;
		//the row number to copy
		uint32_t srcRow;
		//the row number to copy to
		uint32_t destRow;
		//the width of a source row
		uint32_t rowWidth;
		//the width of the source texture
		uint32_t srcTextureWidth;
		//the width of the dest texture
		uint32_t destTextureWidth;
	};
	class TextBlockWidget;
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
		void initAtlasData();
		void updateTextAtlasTexture();
		nvrhi::CommandListHandle m_CommandList;

		nvrhi::TextureHandle m_textureAtlas;

		uint32_t m_atlasSize = 1024;

		std::shared_ptr<Style> m_styles;

		void loadStyleTextures();
		void copyRow(const FCopyRowData& copyRowData);
		void zeroRow(const FCopyRowData& copyRowData);
		void copyDataIntoSlot(std::shared_ptr<AtlasedTextureSlot> slot, const std::vector<uint8_t>& data);
		std::list<std::shared_ptr<AtlasedTextureSlot>> m_textureAtlasEmptySlots;
		std::list<std::shared_ptr<AtlasedTextureSlot>> m_textureAtlasUsedSlots;
		std::vector<uint8_t> m_textureAtlasData;
		std::shared_ptr<TextureCache> m_textureCache;
		std::shared_ptr<AtlasedTextureSlot> loadAtlasSlots(std::shared_ptr<TextureData> texture, std::shared_ptr<Brush> brush);

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
		void calculateWidgetsFixedSize(std::shared_ptr<Widget> widget);
		void generateWidgetElement(WidgetGeometry& allocatedWidgetGeometry);
		std::shared_ptr<TextBlockWidget> m_textBlockWidget;

		std::shared_ptr<RootFileSystem> m_rootFileSystem;
		//std::shared_ptr<FontCache> m_fontCache;
	};
}