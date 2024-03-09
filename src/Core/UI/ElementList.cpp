#include <pch.h>

#include "ElementList.h"
#include "Element.h"
#include "BoxElement.h"
#include "TextElement.h"
#include "Brush.h"
#include "FontCache.h"
#include "CharacterList.h"

namespace GuGu {
	ElementList::ElementList()
	{
	}
	ElementList::~ElementList()
	{
	}
	void ElementList::clear()
	{
		m_elements.clear();
		m_batches.clear();
		m_clippingManager.resetClippingState();
	}
	void ElementList::addBoxElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer)
	{
		std::shared_ptr<BoxElement> boxElement = std::make_shared<BoxElement>(Element::ElementType::Box, widgetGeometry, color, brush, layer, brush->m_tiling);
		boxElement->setClipIndex(elementList.getClippintIndex());
		//generate a box element to element list
		elementList.m_elements.push_back(boxElement);
	}
	void ElementList::addTextElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<TextInfo> textInfo, const GuGuUtf8Str& text, uint32_t layer)
	{
		std::shared_ptr<TextElement> textElement = std::make_shared<TextElement>(Element::ElementType::Text, widgetGeometry, color, textInfo, text, layer);
		textElement->setClipIndex(elementList.getClippintIndex());
		elementList.m_elements.push_back(textElement);
	}
	void ElementList::generateBatches()
	{
		std::stable_sort(m_elements.begin(), m_elements.end(), [=](const std::shared_ptr<Element>& lhs, const std::shared_ptr<Element>& rhs) {
				return lhs->m_layer < rhs->m_layer;
			});

		for (size_t i = 0; i < m_elements.size(); ++i)
		{
			switch (m_elements[i]->m_elementType)
			{
				case Element::ElementType::Box:
				{
					std::shared_ptr<BatchData> boxBatch = std::make_shared<BatchData>();
					boxBatch->shaderType = UIShaderType::Default;
					boxBatch->m_layer = m_elements[i]->m_layer;
					boxBatch->m_clippingState = getClippingState(m_elements[i]->m_clipIndex);
					generateBoxBatch(boxBatch, m_elements[i]);
					m_batches.push_back(boxBatch);
					break;
				}
				case Element::ElementType::Text:
				{
					//std::shared_ptr<BatchData> textBatch = std::make_shared<BatchData>();
					//generateTextBatch(textBatch, m_elements[i]);
					//m_batches.push_back(textBatch);
					generateTextBatch(m_elements[i]);
					break;
				}
			}
		}

		std::vector<std::shared_ptr<BatchData>> batches;
		uint32_t lastBatch = 0;
		//merge batches
		for (size_t i = 1; i < m_batches.size(); ++i)
		{
			if (m_batches[i]->shaderType == m_batches[lastBatch]->shaderType
				&& m_batches[i]->m_texture == m_batches[lastBatch]->m_texture
				&& m_batches[i]->m_layer == m_batches[lastBatch]->m_layer)
			{
				int32_t indexOffset = m_batches[lastBatch]->m_vertices.size();
				m_batches[lastBatch]->m_vertices.insert(m_batches[lastBatch]->m_vertices.end(), m_batches[i]->m_vertices.begin(), m_batches[i]->m_vertices.end());
				//m_batches[lastBatch]->m_indices.insert(m_batches[lastBatch]->m_indices.end(), m_batches[i]->m_indices.begin(), m_batches[i]->m_indices.end());			
				for (size_t j = 0; j < m_batches[i]->m_indices.size(); ++j)
				{
					m_batches[lastBatch]->m_indices.push_back(m_batches[i]->m_indices[j] + indexOffset);
				}
			}
			else
			{
				batches.push_back(m_batches[lastBatch]);
				lastBatch = i;
			}
		}
		if(m_batches.size()) //last batch
			batches.push_back(m_batches[lastBatch]);
		m_batches = batches;
	}
	const std::vector<std::shared_ptr<BatchData>>& ElementList::getBatches() const
	{
		return m_batches;
	}
	int32_t ElementList::pushClip(const ClippingZone& inClipZone)
	{
		const int32_t newClipIndex = m_clippingManager.pushClip(inClipZone);
		return newClipIndex;
	}
	void ElementList::popClip()
	{
		m_clippingManager.popClip();
	}
	int32_t ElementList::getClippintIndex() const
	{
		return m_clippingManager.getClippingIndex();
	}
	const ClippingState* ElementList::getClippingState(uint32_t clippingIndex) const
	{
		return m_clippingManager.getClippingState(clippingIndex);
	}
	void ElementList::generateBoxBatch(std::shared_ptr<BatchData> boxBatch, std::shared_ptr<Element> element)
	{
		std::shared_ptr<BoxElement> boxElement = std::static_pointer_cast<BoxElement>(element);
		math::double2 absolutePosition = boxElement->m_geometry.getAbsolutePosition();
		math::double2 localSize = boxElement->m_geometry.getLocalSize();
		//math::double2 localSize = math::double2(200.0f, 200.0f);
		math::float4 color = boxElement->m_color;
		std::shared_ptr<Brush> brush = boxElement->m_brush;
		bool tiling = element->m_tiling;
		math::float2 tile = tiling ?  math::float2(localSize.x / brush->m_actualSize.x, localSize.y / brush->m_actualSize.x) : math::float2(1.0f, 1.0f);
		//math::float2 tile = tiling ? math::float2(4.0f, 4.0f) : math::float2(1.0f, 1.0f);
		boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x, brush->m_startUV.y, tile.x, tile.y), math::float2(absolutePosition.x, absolutePosition.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x + brush->m_sizeUV.x, brush->m_startUV.y + brush->m_sizeUV.y, tile.x, tile.y), math::float2(absolutePosition.x + localSize.x, absolutePosition.y + localSize.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x, brush->m_startUV.y + brush->m_sizeUV.y, tile.x, tile.y), math::float2(absolutePosition.x, absolutePosition.y + localSize.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x + brush->m_sizeUV.x, brush->m_startUV.y, tile.x, tile.y), math::float2(absolutePosition.x + localSize.x, absolutePosition.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_indices.emplace_back(0);
		boxBatch->m_indices.emplace_back(1);
		boxBatch->m_indices.emplace_back(2);
		boxBatch->m_indices.emplace_back(0);
		boxBatch->m_indices.emplace_back(3);
		boxBatch->m_indices.emplace_back(1);

		boxBatch->m_texture = brush->m_texture;
	}
	void ElementList::generateTextBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<TextElement> textElement = std::static_pointer_cast<TextElement>(element);
		float scale = textElement->m_geometry.getAbsoluteScale();
		math::double2 absolutePosition = textElement->m_geometry.getAbsolutePosition();
		//math::double2 localSize = textElement->m_geometry.getLocalSize();
		//math::double2 localSize = math::double2(200.0f, 200.0f);
		math::float4 color = textElement->m_color;

		CharacterList& characterList = *FontCache::getFontCache()->getCharacterList(*(textElement->m_textInfo), scale);

		float invTextureSizeX = 1 / 1024.0f;
		float invTextureSizeY = 1 / 1024.0f;
		float posX = 0;
		float posY = 0;
		float maxHeight = characterList.getMaxHeight();
		float lineX = 0;
		uint32_t numLines = 1;
		GuGuUtf8Str& str = textElement->m_text;
		for (size_t i = 0; i < str.len(); ++i)
		{
			GuGuUtf8Str Char = str[i];

			const bool isNewLine = Char == u8"\n";

			if (isNewLine)
			{
				posY += maxHeight;
				lineX = posX;
				++numLines;
			}
			else
			{
				const GlyphEntry& entry = *characterList.getCharacter(Char);

				std::shared_ptr<BatchData> batchData = std::make_shared<BatchData>();
				batchData->m_clippingState = getClippingState(element->m_clipIndex);

				const bool isWhiteSpace = Char.getUnicode().at(0) == 0;

				if (!isWhiteSpace)
				{
					const float x = lineX + entry.m_glyphFontAtlasData.horizontalOffset;

					const float y = posY - entry.m_glyphFontAtlasData.verticalOffset + maxHeight;//todo:add max height and global descendar
					const float u = entry.m_glyphFontAtlasData.startU * invTextureSizeX;
					const float v = entry.m_glyphFontAtlasData.startV * invTextureSizeY;
					const float sizeX = entry.m_glyphFontAtlasData.uSize;
					const float sizeY = entry.m_glyphFontAtlasData.vSize;
					const float sizeU = entry.m_glyphFontAtlasData.uSize * invTextureSizeX;
					const float sizeV = entry.m_glyphFontAtlasData.vSize * invTextureSizeY;

					math::double2 upperLeft = absolutePosition + math::double2(x, y);
					math::double2 upperRight = absolutePosition + math::double2(x + sizeX, y);
					math::double2 lowerLeft = absolutePosition + math::double2(x, y + sizeY);
					math::double2 lowerRight = absolutePosition + math::double2(x + sizeX, y + sizeY);
					
					batchData->m_vertices.emplace_back(math::float4(u, v, 1.0f, 1.0f), math::float2(upperLeft.x, upperLeft.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
					batchData->m_vertices.emplace_back(math::float4(u + sizeU, v + sizeV, 1.0f, 1.0f), math::float2(lowerRight.x, lowerRight.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
					batchData->m_vertices.emplace_back(math::float4(u, v + sizeV, 1.0f, 1.0f), math::float2(lowerLeft.x, lowerLeft.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
					batchData->m_vertices.emplace_back(math::float4(u + sizeU, v, 1.0f, 1.0f), math::float2(upperRight.x, upperRight.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

					batchData->m_indices.emplace_back(0);
					batchData->m_indices.emplace_back(1);
					batchData->m_indices.emplace_back(2);
					batchData->m_indices.emplace_back(0);
					batchData->m_indices.emplace_back(3);
					batchData->m_indices.emplace_back(1);

					batchData->m_texture = FontCache::getFontCache()->getFontAtlasTexture();
					batchData->shaderType = UIShaderType::Font;
					batchData->m_layer = element->m_layer;
					m_batches.push_back(batchData);
				}

				lineX += entry.xAdvance;
			}
		}
	}
}