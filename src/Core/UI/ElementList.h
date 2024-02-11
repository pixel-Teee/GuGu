#pragma once

#include <vector>

#include <Renderer/MyMath.h>

#include "BasicElement.h"

namespace GuGu {
	class Element;
	class WidgetGeometry;
	class Brush;
	struct BatchData
	{
		std::vector<UIVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		std::shared_ptr<Brush> m_brush;
	};
	class ElementList
	{
	public:
		ElementList();

		virtual ~ElementList();

		void clear();

		static void addBoxElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer);

		void generateBatches();

		const std::vector<std::shared_ptr<BatchData>>& getBatches() const;
	private:
		std::vector<std::shared_ptr<Element>> m_elements;

		std::vector<std::shared_ptr<BatchData>> m_batches;

		void generateBoxBatch(std::shared_ptr<BatchData> boxBatch, std::shared_ptr<Element> element);
	};
}