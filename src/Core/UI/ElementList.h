#pragma once

#include <vector>

#include <Core/Math/MyMath.h>
#include <Renderer/nvrhi.h>

#include "Clipping.h"
#include "BasicElement.h"
#include "Element.h"

namespace GuGu {
	class Brush;
	class Element;
	class TextInfo;
	class WidgetGeometry;
	class GuGuUtf8Str;
	class ShapedGlyphSequence;
	class FontCache;
	enum class UIShaderType
	{
		Default,
		RoundedBox,
		Font,
		Line
	};

	struct ShaderParam
	{
		math::float4 pixelParams;
		math::float4 pixelParams2;

		ShaderParam()
			: pixelParams(0, 0, 0, 0)
			, pixelParams2(0, 0, 0, 0)
		{}

		ShaderParam(const math::float4& inPixelParams, const math::float4& inPixelParams2 = math::float4(0, 0, 0, 0))
			: pixelParams(inPixelParams)
			, pixelParams2(inPixelParams2)
		{}

		bool operator==(const ShaderParam& rhs) const
		{
			return (pixelParams.x == rhs.pixelParams.x && pixelParams.y == rhs.pixelParams.y && pixelParams.z == rhs.pixelParams.z && pixelParams.w == rhs.pixelParams.w)
				&& (pixelParams2.x == rhs.pixelParams2.x && pixelParams2.y == rhs.pixelParams2.y && pixelParams2.z == rhs.pixelParams2.z && pixelParams2.w == rhs.pixelParams2.w);
		}
	};

	struct BatchData
	{
		UIShaderType shaderType = UIShaderType::Default;
		std::vector<UIVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		//std::shared_ptr<Brush> m_brush;
		nvrhi::TextureHandle m_texture;
		uint32_t m_layer;//note:useful for batch
		const ClippingState* m_clippingState;//weak
		ShaderParam m_shaderParams;
	};

	struct ShapedTextBuildContext
	{
		const ShapedGlyphSequence* shapedGlyphSequence;
		Element* element;
		FontCache* fontCache;
		float textBaseLine;
		float maxHeight;
		float startLineX;
		float startLineY;
		int32_t layerId;
		math::float4 fontTint;
	};
	class ElementList
	{
	public:
		ElementList();

		virtual ~ElementList();

		void clear();

		static void addBoxElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer);

		static void addTextElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<TextInfo> textInfo, const GuGuUtf8Str& text, uint32_t layer);

		static void addLineElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, const std::vector<math::float2>& points, float thickNess, uint32_t layer);

		static void addSplineElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, const math::float2& inStart, const math::float2& inStartDir, const math::float2& inEnd, const math::float2& inEndDir, float thickNess, uint32_t layer);

		static void addViewportElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, nvrhi::TextureHandle renderTarget, uint32_t layer);

		static void addShapedTextElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<ShapedGlyphSequence> shapedText, uint32_t layer);

		static void addGradientElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, std::vector<GradientStop> gradientStops, Orientation orientation, math::float4 cornerRadius, uint32_t layer);

		void generateBatches();

		const std::vector<std::shared_ptr<BatchData>>& getBatches() const;

		int32_t pushClip(const ClippingZone& inClipZone);

		void popClip();

		int32_t getClippintIndex() const;

		const ClippingState* getClippingState(uint32_t clippingIndex) const;
	private:
		std::vector<std::shared_ptr<Element>> m_elements;

		std::vector<std::shared_ptr<BatchData>> m_batches;

		void generateBoxBatch(std::shared_ptr<Element> element);

		void generateTextBatch(std::shared_ptr<Element> element);
		
		void generateLineBatch(std::shared_ptr<Element> element);

		void generateSplineBatch(std::shared_ptr<Element> element);

		void generateViewportBatch(std::shared_ptr<Element> element);

		void generateShapedTextBatch(std::shared_ptr<Element> element);

		void generateGradientBatch(std::shared_ptr<Element> element);

		void buildShapedTextSequence(const ShapedTextBuildContext& context);

		ClippingManager m_clippingManager;
	};
}