#pragma once

#include <vector>

#include <Core/Math/MyMath.h>
#include <Renderer/nvrhi.h>

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
		Font
	};

	struct BatchData
	{
		UIShaderType shaderType = UIShaderType::Default;
		std::vector<UIVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		//std::shared_ptr<Brush> m_brush;
		nvrhi::TextureHandle m_texture;
		uint32_t m_layer;//note:useful for batch
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

		void generateTextBatch(std::shared_ptr<Element> element);
	};
}