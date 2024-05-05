#pragma once

#include <Renderer/DeviceManager.h>//IRenderPass

#include "AtlasTexture.h"

#include "ListView.h"
#include "ComboBox.h"
#include "ColorWheel.h"

namespace GuGu {
	class Brush;
	class StyleSet;

	struct TextureData;
	class AtlasTexture;
	class TextureCache;
	
	class Widget;
	class WindowWidget;
	class ViewportWidget;
	class TextBlockWidget;

	class ElementList;
	class WidgetGeometry;

	class FontCache;

	class RootFileSystem;

	struct UIData;

	class CommonRenderPasses;
	class UIRenderPass : public IRenderPass
	{
	public:
		//UIRenderPass();
		using IRenderPass::IRenderPass;

		virtual ~UIRenderPass();

		bool Init(std::shared_ptr<UIData> uiData);

		virtual void Render(nvrhi::IFramebuffer* framebuffer);

		virtual void Update(float fElapsedTimeSeconds);
		
		virtual void BackBufferResizing();
		
		virtual void BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount);

		std::shared_ptr<WindowWidget> getWindowWidget();

		std::vector<std::shared_ptr<Widget>> getAllWidgets();

		void setRenderTarget(nvrhi::TextureHandle renderTarget);

		GuGuUtf8Str getSelectUINode() const;//debug use
	private:
		void loadStyleTextures();

		void updateTextAtlasTexture();	

		uint32_t m_atlasSize = 2048;

		nvrhi::CommandListHandle m_CommandList;

		std::shared_ptr<StyleSet> m_styles;

		std::shared_ptr<AtlasTexture> m_atlasTexture;

		//std::vector<uint8_t> m_textureAtlasData;
		std::shared_ptr<TextureCache> m_textureCache;

		nvrhi::SamplerHandle m_pointWrapSampler;
		nvrhi::ShaderHandle m_vertexShader;
		nvrhi::ShaderHandle m_pixelShader;
		nvrhi::ShaderHandle m_pixelFontShader;
		nvrhi::ShaderHandle m_lineShader;
		nvrhi::ShaderHandle m_roundedBoxPixelShader;

		nvrhi::InputLayoutHandle m_inputLayout;
		nvrhi::BindingLayoutHandle m_bindingLayout;
		nvrhi::BindingSetHandle m_bindingSet;
		nvrhi::GraphicsPipelineHandle m_pipeline;
		nvrhi::GraphicsPipelineHandle m_FontPipeline;
		nvrhi::GraphicsPipelineHandle m_LinePipeline;
		nvrhi::GraphicsPipelineHandle m_roundedBoxPipeline;
		struct ConstantBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4 shaderParam;
			dm::float4 shaderParam2;
			float padding[28];
		};

		std::shared_ptr<WindowWidget> m_uiRoot;
		std::shared_ptr<ViewportWidget> m_viewport;
		std::vector<std::shared_ptr<Widget>> m_allWidgets;
		std::vector<std::shared_ptr<Widget>> m_tempAllWidgetCopys;//for input test
		std::shared_ptr<ElementList> m_elementList;
		std::vector<nvrhi::BufferHandle> m_VertexBuffers;
		std::vector<nvrhi::BufferHandle> m_IndexBuffers;
		std::vector<nvrhi::BufferHandle> m_constantBuffers;

		void calculateWidgetsFixedSize(std::shared_ptr<WindowWidget> widget);

		void generateWidgetElement(WidgetGeometry& allocatedWidgetGeometry);

		std::shared_ptr<TextBlockWidget> m_textBlockWidget;

		std::shared_ptr<RootFileSystem> m_rootFileSystem;
		//std::shared_ptr<FontCache> m_fontCache;

		//------test widget functionality------
		std::shared_ptr<WindowWidget> createTestWindow();
		math::float4 m_textColor;

		std::shared_ptr<UIData> m_uiData;

		std::shared_ptr<ListView<GuGuUtf8Str>> m_listView;

		std::shared_ptr<ComboBox<GuGuUtf8Str>> m_comboBox;

		std::shared_ptr<ListView<GuGuUtf8Str>> m_uiListView;

		std::shared_ptr<VerticalBox> m_verticalBox;

		std::vector<GuGuUtf8Str> m_uiNodeNames;
		//------test widget functionality------

		std::shared_ptr<CommonRenderPasses> m_commonRenderPass;

		GuGuUtf8Str getSelectItem() const;

		void getUIChildNode(GuGuUtf8Str, std::vector<GuGuUtf8Str>&);

		void selectionChanged(GuGuUtf8Str item, SelectInfo::Type);

		GuGuUtf8Str m_selectUINode;

		std::shared_ptr<ColorWheel> m_colorWheel;
		void setColor(math::float4 inColor);
		math::float4 m_color = math::float4(0.0f, 0.0f, 1.0f, 1.0f);
		math::float4 getColor() const;//for color block
	};
}