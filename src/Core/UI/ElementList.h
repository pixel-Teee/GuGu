#pragma once

#include <vector>

#include <Core/Math/MyMath.h>
#include <Renderer/nvrhi.h>

#include "Clipping.h"
#include "BasicElement.h"

namespace GuGu {
	class Brush;
	class Element;
	class TextInfo;
	class WidgetGeometry;
	class GuGuUtf8Str;

	enum class UIShaderType
	{
		Default,
		Font,
		Line
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

		void generateBatches();

		const std::vector<std::shared_ptr<BatchData>>& getBatches() const;

		int32_t pushClip(const ClippingZone& inClipZone);

		void popClip();

		int32_t getClippintIndex() const;

		const ClippingState* getClippingState(uint32_t clippingIndex) const;
	private:
		std::vector<std::shared_ptr<Element>> m_elements;

		std::vector<std::shared_ptr<BatchData>> m_batches;

		void generateBoxBatch(std::shared_ptr<BatchData> boxBatch, std::shared_ptr<Element> element);

		void generateTextBatch(std::shared_ptr<Element> element);
		
		void generateLineBatch(std::shared_ptr<Element> element);

		ClippingManager m_clippingManager;
	};
}