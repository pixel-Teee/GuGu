#pragma once

#include <vector>

#include <Renderer/MyMath.h>

#include "BasicElement.h"

#include <Renderer/nvrhi.h>

namespace GuGu {
	class Element;
	class TextInfo;
	class WidgetGeometry;
	class Brush;
	class GuGuUtf8Str;
	struct BatchData
	{
		std::vector<UIVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		//std::shared_ptr<Brush> m_brush;
		nvrhi::TextureHandle m_texture;
	};
	class ElementList
	{
	public:
		ElementList();

		virtual ~ElementList();

		void clear();

		static void addBoxElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer);

		static void addTextElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<TextInfo> textInfo, GuGuUtf8Str& text, uint32_t layer);

		void generateBatches();

		const std::vector<std::shared_ptr<BatchData>>& getBatches() const;
	private:
		std::vector<std::shared_ptr<Element>> m_elements;

		std::vector<std::shared_ptr<BatchData>> m_batches;

		void generateBoxBatch(std::shared_ptr<BatchData> boxBatch, std::shared_ptr<Element> element);

		void generateTextBatch(std::shared_ptr<Element> element, std::vector<std::shared_ptr<BatchData>>& batches);
	};
}