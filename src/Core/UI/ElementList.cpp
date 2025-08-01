#include <pch.h>

#include "ElementList.h"
#include "Element.h"
#include "BoxElement.h"
#include "LineElement.h"
#include "SplineElement.h"
#include "ViewportElement.h"
#include "TextElement.h"
#include "ShapedTextElement.h"
#include "RoundedBoxElement.h"
#include "Brush.h"
#include "FontCache.h"
#include "CharacterList.h"
#include "LineBuilder.h"
#include "GradientElement.h"

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
		//cull
		if (brush == nullptr)
			return;

		Element::ElementType elementType = (brush->m_drawAs == BrushDrawType::Border) ? Element::ElementType::Border : ((brush->m_drawAs == BrushDrawType::RoundedBox) ? Element::ElementType::RoundedBox : Element::ElementType::Box);
		
		if (elementType == Element::ElementType::RoundedBox)
		{
			std::shared_ptr<RoundedBoxElement> boxElement = std::make_shared<RoundedBoxElement>(Element::ElementType::RoundedBox, widgetGeometry, color, brush, layer, brush->m_tiling);
			boxElement->setClipIndex(elementList.getClippintIndex());
			boxElement->setRadius(brush->m_outlineSettings.m_cornerRadius);
			boxElement->setOutline(brush->m_outlineSettings.m_color, brush->m_outlineSettings.m_width);
			//generate a box element to element list
			elementList.m_elements.push_back(boxElement);
		}
		else
		{
			std::shared_ptr<BoxElement> boxElement = std::make_shared<BoxElement>(elementType, widgetGeometry, color, brush, layer, brush->m_tiling);
			boxElement->setClipIndex(elementList.getClippintIndex());
			//generate a box element to element list
			elementList.m_elements.push_back(boxElement);
		}		
	}
	void ElementList::addTextElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<TextInfo> textInfo, const GuGuUtf8Str& text, uint32_t layer)
	{
		std::shared_ptr<TextElement> textElement = std::make_shared<TextElement>(Element::ElementType::Text, widgetGeometry, color, textInfo, text, layer);
		textElement->setClipIndex(elementList.getClippintIndex());
		elementList.m_elements.push_back(textElement);
	}
	void ElementList::addLineElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, const std::vector<math::float2>& points, float thickNess, uint32_t layer)
	{
		std::shared_ptr<LineElement> lineElement = std::make_shared<LineElement>(Element::ElementType::Line, widgetGeometry, layer, false);
		lineElement->m_thickNess = thickNess;
		lineElement->m_points = points;
		lineElement->m_color = color;
		lineElement->setClipIndex(elementList.getClippintIndex());
		elementList.m_elements.push_back(lineElement);
	}
	void ElementList::addSplineElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, const math::float2& inStart, const math::float2& inStartDir, const math::float2& inEnd, const math::float2& inEndDir, float thickNess, uint32_t layer)
	{
		std::shared_ptr<SplineElement> lineElement = std::make_shared<SplineElement>(Element::ElementType::Spline, widgetGeometry, layer);
		lineElement->setHermiteSpline(inStart, inStartDir, inEnd, inEndDir, thickNess, color);
		//lineElement->m_points = points;
		lineElement->setClipIndex(elementList.getClippintIndex());
		elementList.m_elements.push_back(lineElement);
	}
	void ElementList::addViewportElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, nvrhi::TextureHandle renderTarget, uint32_t layer)
	{
		std::shared_ptr<ViewportElement> viewportElement = std::make_shared<ViewportElement>(Element::ElementType::Viewport, widgetGeometry, layer, false);
		viewportElement->setClipIndex(elementList.getClippintIndex());
		viewportElement->m_renderTargetResouce = renderTarget;
		viewportElement->m_color = color;
		//generate a box element to element list
		elementList.m_elements.push_back(viewportElement);
	}
	void ElementList::addShapedTextElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<ShapedGlyphSequence> shapedText, uint32_t layer)
	{
		std::shared_ptr<ShapedTextElement> shapedTextElement = std::make_shared<ShapedTextElement>(Element::ElementType::ShapedText, widgetGeometry, layer);
		shapedTextElement->setClipIndex(elementList.getClippintIndex());
		shapedTextElement->m_color = color;
		shapedTextElement->m_shapedTextSequence = shapedText;
		elementList.m_elements.push_back(shapedTextElement);
	}
	void ElementList::addGradientElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, std::vector<GradientStop> gradientStops, Orientation orientation, math::float4 cornerRadius, uint32_t layer)
	{
		std::shared_ptr<GradientElement> gradientElement = std::make_shared<GradientElement>(Element::ElementType::Gradient, widgetGeometry, layer);
		gradientElement->setClipIndex(elementList.getClippintIndex());
		gradientElement->m_gradientStops = gradientStops;
		gradientElement->m_cornerRadius = cornerRadius;
		gradientElement->m_orientation = orientation;
		elementList.m_elements.push_back(gradientElement);
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
				case Element::ElementType::RoundedBox:
				{
					generateBoxBatch(m_elements[i]);
					break;
				}
				case Element::ElementType::Border:
				{
					generateBorderBatch(m_elements[i]);
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
				case Element::ElementType::Line:
				{
					generateLineBatch(m_elements[i]);
					break;
				}
				case Element::ElementType::Spline:
				{
					generateSplineBatch(m_elements[i]);
					break;
				}
				case Element::ElementType::Viewport:
				{
					generateViewportBatch(m_elements[i]);
					break;
				}
				case Element::ElementType::ShapedText:
				{
					generateShapedTextBatch(m_elements[i]);
					break;
				}
				case Element::ElementType::Gradient:
				{
					generateGradientBatch(m_elements[i]);
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
				&& m_batches[i]->m_layer == m_batches[lastBatch]->m_layer && m_batches[i]->m_clippingState == m_batches[lastBatch]->m_clippingState && m_batches[i]->m_shaderParams == m_batches[lastBatch]->m_shaderParams)
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
	void ElementList::generateBoxBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<BatchData> boxBatch = std::make_shared<BatchData>();
		boxBatch->m_layer = element->m_layer;
		boxBatch->m_clippingState = getClippingState(element->m_clipIndex);

		std::shared_ptr<BoxElement> boxElement = std::static_pointer_cast<BoxElement>(element);
		//math::double2 absolutePosition = boxElement->m_geometry.getAbsolutePosition();
		math::affine2 transform = boxElement->m_geometry.getAccumulateRenderTransform();
		math::float2 localSize = boxElement->m_geometry.getLocalSize();
		//math::double2 localSize = math::double2(200.0f, 200.0f);
		math::float4 color = boxElement->m_color;
		std::shared_ptr<Brush> brush = boxElement->m_brush;
		bool tiling = element->m_tiling;
		math::float2 tile = tiling ?  math::float2(localSize.x / brush->m_actualSize.x, localSize.y / brush->m_actualSize.x) : math::float2(1.0f, 1.0f);

		math::float4 secondaryColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f);

		uint32_t textureWidth = 1;
		uint32_t textureHeight = 1;

		math::float2 startUV = math::float2(0.0f, 0.0f);
		math::float2 endUV = math::float2(1.0f, 1.0f);
		math::float2 sizeUV;

		sizeUV = math::float2(brush->m_sizeUV.x, brush->m_sizeUV.y);
		endUV = math::float2(brush->m_sizeUV.x + brush->m_startUV.x, brush->m_sizeUV.y + brush->m_startUV.y);
		startUV = math::float2(brush->m_startUV.x, brush->m_startUV.y);

		textureWidth = brush->m_actualSize.x;
		textureHeight = brush->m_actualSize.y;

		if (element->m_elementType == Element::Box)
		{
			boxBatch->shaderType = UIShaderType::Default;
		}
		else if (element->m_elementType == Element::RoundedBox)
		{
			boxBatch->shaderType = UIShaderType::RoundedBox;

			std::shared_ptr<RoundedBoxElement> roundedBoxElement = std::static_pointer_cast<RoundedBoxElement>(element);
			boxBatch->m_shaderParams = ShaderParam(math::float4(0, roundedBoxElement->getOutlineWeight(), localSize.x, localSize.y));
			boxBatch->m_shaderParams.pixelParams2 = roundedBoxElement->getRadius();

			secondaryColor = roundedBoxElement->getOutlineColor();
		}

		Padding padding = brush->m_margin;

		//image 的 margin 是空的
		if (brush->m_drawAs != BrushDrawType::Image && (padding.left != 0.0f || padding.top != 0.0f || padding.right != 0.0f || padding.bottom != 0.0f))
		{
			//创建9个正方形，对于box element

			//决定每个正方形的纹理坐标
			float leftMarginU = (padding.left > 0.0f)
				? startUV.x + padding.left * sizeUV.x : startUV.x;
			float topMarginV = (padding.top > 0.0f)
				? startUV.y + padding.top * sizeUV.y : startUV.y;
			float rightMarginU = (padding.right > 0.0f)
				? endUV.x - padding.right * sizeUV.x : endUV.x;
			float bottomMarginV = (padding.bottom > 0.0f)
				? endUV.y - padding.bottom * sizeUV.y : endUV.x;

			float leftMarginX = textureWidth * padding.left;
			float topMarginY = textureHeight * padding.top;
			float rightMarginX = localSize.x - textureWidth * padding.right;
			float bottomMarginY = localSize.y - textureHeight * padding.bottom;

			math::float2 topLeft = math::float2(0.0f, 0.0f);
			math::float2 bottomRight = math::float2(localSize.x, localSize.y);
			math::float2 topRight = math::float2(localSize.x, 0.0f);
			math::float2 bottomLeft = math::float2(0.0f, localSize.y);

			math::float2 position = topLeft;
			math::float2 endPos = bottomRight;

			boxBatch->m_vertices.emplace_back(math::float4(startUV.x, startUV.y, tile.x, tile.y), transform.transformPoint(position), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(startUV.x, topMarginV, tile.x, tile.y), transform.transformPoint(math::float2(position.x, topMarginY)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, startUV.y, tile.x, tile.y), transform.transformPoint(math::float2(leftMarginX, position.y)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, topMarginV, tile.x, tile.y), transform.transformPoint(math::float2(leftMarginX, topMarginY)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, startUV.y, tile.x, tile.y), transform.transformPoint(math::float2(rightMarginX, position.y)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, topMarginV, tile.x, tile.y), transform.transformPoint(math::float2(rightMarginX, topMarginY)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(endUV.x, startUV.y, tile.x, tile.y), transform.transformPoint(math::float2(endPos.x, position.y)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(endUV.x, topMarginV, tile.x, tile.y), transform.transformPoint(math::float2(endPos.x, topMarginY)), color, secondaryColor);
			//boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, topMarginV, tile.x, tile.y), transform.transformPoint(math::float2(leftMarginX, position.y)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(startUV.x, bottomMarginV, tile.x, tile.y), transform.transformPoint(math::float2(position.x, bottomMarginY)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, bottomMarginV, tile.x, tile.y), transform.transformPoint(math::float2(leftMarginX, bottomMarginY)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, bottomMarginV, tile.x, tile.y), transform.transformPoint(math::float2(rightMarginX, bottomMarginY)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(endUV.x, bottomMarginV, tile.x, tile.y), transform.transformPoint(math::float2(endPos.x, bottomMarginY)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(startUV.x, endUV.y, tile.x, tile.y), transform.transformPoint(math::float2(position.x, endPos.y)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, endUV.y, tile.x, tile.y), transform.transformPoint(math::float2(leftMarginX, endPos.y)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, endUV.y, tile.x, tile.y), transform.transformPoint(math::float2(rightMarginX, endPos.y)), color, secondaryColor);
			boxBatch->m_vertices.emplace_back(math::float4(endUV.x, endUV.y, tile.x, tile.y), transform.transformPoint(math::float2(endPos.x, endPos.y)), color, secondaryColor);

			//top
			boxBatch->m_indices.emplace_back(0);
			boxBatch->m_indices.emplace_back(2);
			boxBatch->m_indices.emplace_back(1);
			boxBatch->m_indices.emplace_back(1);
			boxBatch->m_indices.emplace_back(2);
			boxBatch->m_indices.emplace_back(3);

			boxBatch->m_indices.emplace_back(2);
			boxBatch->m_indices.emplace_back(4);
			boxBatch->m_indices.emplace_back(3);
			boxBatch->m_indices.emplace_back(3);
			boxBatch->m_indices.emplace_back(4);
			boxBatch->m_indices.emplace_back(5);

			boxBatch->m_indices.emplace_back(4);
			boxBatch->m_indices.emplace_back(6);
			boxBatch->m_indices.emplace_back(5);
			boxBatch->m_indices.emplace_back(6);
			boxBatch->m_indices.emplace_back(7);
			boxBatch->m_indices.emplace_back(5);

			//middle
			boxBatch->m_indices.emplace_back(1);
			boxBatch->m_indices.emplace_back(3);
			boxBatch->m_indices.emplace_back(8);
			boxBatch->m_indices.emplace_back(3);
			boxBatch->m_indices.emplace_back(9);
			boxBatch->m_indices.emplace_back(8);

			boxBatch->m_indices.emplace_back(3);
			boxBatch->m_indices.emplace_back(5);
			boxBatch->m_indices.emplace_back(9);
			boxBatch->m_indices.emplace_back(5);
			boxBatch->m_indices.emplace_back(10);
			boxBatch->m_indices.emplace_back(9);

			boxBatch->m_indices.emplace_back(5);
			boxBatch->m_indices.emplace_back(7);
			boxBatch->m_indices.emplace_back(10);
			boxBatch->m_indices.emplace_back(7);
			boxBatch->m_indices.emplace_back(11);
			boxBatch->m_indices.emplace_back(10);

			//bottom
			boxBatch->m_indices.emplace_back(8);
			boxBatch->m_indices.emplace_back(9);
			boxBatch->m_indices.emplace_back(12);
			boxBatch->m_indices.emplace_back(9);
			boxBatch->m_indices.emplace_back(13);
			boxBatch->m_indices.emplace_back(12);

			boxBatch->m_indices.emplace_back(9);
			boxBatch->m_indices.emplace_back(10);
			boxBatch->m_indices.emplace_back(13);
			boxBatch->m_indices.emplace_back(10);
			boxBatch->m_indices.emplace_back(14);
			boxBatch->m_indices.emplace_back(13);

			boxBatch->m_indices.emplace_back(10);
			boxBatch->m_indices.emplace_back(11);
			boxBatch->m_indices.emplace_back(14);
			boxBatch->m_indices.emplace_back(11);
			boxBatch->m_indices.emplace_back(15);
			boxBatch->m_indices.emplace_back(14);

			boxBatch->m_texture = brush->m_texture;
		}
		else
		{
			math::float2 topLeft = math::float2(0.0f, 0.0f);
			math::float2 bottomRight = math::float2(localSize.x, localSize.y);
			math::float2 topRight = math::float2(localSize.x, 0.0f);
			math::float2 bottomLeft = math::float2(0.0f, localSize.y);
			//math::float2 tile = tiling ? math::float2(4.0f, 4.0f) : math::float2(1.0f, 1.0f);
			boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x, brush->m_startUV.y, tile.x, tile.y), transform.transformPoint(topLeft), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x + brush->m_sizeUV.x, brush->m_startUV.y + brush->m_sizeUV.y, tile.x, tile.y), transform.transformPoint(bottomRight), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x, brush->m_startUV.y + brush->m_sizeUV.y, tile.x, tile.y), transform.transformPoint(bottomLeft), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x + brush->m_sizeUV.x, brush->m_startUV.y, tile.x, tile.y), transform.transformPoint(topRight), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

			boxBatch->m_indices.emplace_back(0);
			boxBatch->m_indices.emplace_back(1);
			boxBatch->m_indices.emplace_back(2);
			boxBatch->m_indices.emplace_back(0);
			boxBatch->m_indices.emplace_back(3);
			boxBatch->m_indices.emplace_back(1);

			boxBatch->m_texture = brush->m_texture;
		}

		m_batches.push_back(boxBatch);
	}
	void ElementList::generateTextBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<TextElement> textElement = std::static_pointer_cast<TextElement>(element);
		float scale = textElement->m_geometry.getAbsoluteScale();
		//math::float2 absolutePosition = textElement->m_geometry.getAbsolutePosition();
		math::affine2 transform = textElement->m_geometry.getAccumulateRenderTransform();
		transform.m_linear = math::float2x2::identity();
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
					const float x = lineX + entry.m_glyphFontAtlasData->horizontalOffset;

					//posX, posY is the upper left corner of the bounding box representing the string
					const float y = posY - entry.m_glyphFontAtlasData->verticalOffset + maxHeight + entry.globalDescender;//todo:add max height and global descender
					const float u = entry.m_glyphFontAtlasData->startU * invTextureSizeX;
					const float v = entry.m_glyphFontAtlasData->startV * invTextureSizeY;
					const float sizeX = entry.m_glyphFontAtlasData->uSize;
					const float sizeY = entry.m_glyphFontAtlasData->vSize;
					const float sizeU = entry.m_glyphFontAtlasData->uSize * invTextureSizeX;
					const float sizeV = entry.m_glyphFontAtlasData->vSize * invTextureSizeY;

					math::float2 upperLeft = transform.transformPoint(math::float2(x, y));
					math::float2 upperRight = transform.transformPoint(math::float2(x + sizeX, y));
					math::float2 lowerLeft = transform.transformPoint(math::float2(x, y + sizeY));
					math::float2 lowerRight = transform.transformPoint(math::float2(x + sizeX, y + sizeY));
					
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
	void ElementList::generateLineBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<LineElement> lineElement = std::static_pointer_cast<LineElement>(element);
		math::float2 absolutePosition = lineElement->m_geometry.getAbsolutePosition(); //todo:add scale
		math::float2 fAbsolutePosition = math::float2(absolutePosition.x, absolutePosition.y);
		std::vector<math::float2> points = lineElement->m_points;
		std::shared_ptr<BatchData> batchData = std::make_shared<BatchData>();
		math::float4 color = lineElement->m_color;//todo:fix this
		batchData->m_clippingState = getClippingState(element->m_clipIndex);
		batchData->m_layer = element->m_layer;
		batchData->shaderType = UIShaderType::Line;
		
		const float filterScale = 1.0f;
		float requestedThickness = lineElement->m_thickNess;
		static const float twoRootTwo = 2.0f * 1.4142135623730950488016887242097f;//todo:fix this
		const float lineThickness = twoRootTwo + requestedThickness;
		const float halfThickness = lineThickness * 0.5f + filterScale;

		batchData->m_shaderParams = ShaderParam(math::float4(0.0f, 0.0f, 0.0f, 0.0f));

		math::float2 startPos = points[0];
		math::float2 endPos = points[1];

		math::float2 normal = math::normalize(math::float2(startPos.y - endPos.y, endPos.x - startPos.x));
		math::float2 up = normal * halfThickness;

		batchData->m_vertices.emplace_back(math::float4(1.0f, 0.0f, 0.0f, 0.0f), fAbsolutePosition + startPos + up, color, math::float4(1.0f, 0.0f, 0.0f, 0.0f));
		batchData->m_vertices.emplace_back(math::float4(1.0f, 0.0f, 0.0f, 0.0f), fAbsolutePosition + startPos - up, color, math::float4(0.0f, 0.0f, 0.0f, 0.0f));

		for (int32_t point = 1; point < points.size(); ++point)
		{
			endPos = points[point];

			bool bCheckIntersection = (point + 1) < points.size();

			normal = math::normalize(math::float2(startPos.y - endPos.y, endPos.x - startPos.x));

			up = normal * halfThickness;

			math::float2 intersectUpper = endPos + up;
			math::float2 intersectLower = endPos - up;

			if (bCheckIntersection)
			{
				math::float2 nextEndPos = points[point + 1];

				math::float2 nextNormal = math::normalize(math::float2(endPos.y - nextEndPos.y, nextEndPos.x - endPos.x));

				math::float2 nextUp = nextNormal * halfThickness;

				math::float2 intersectionPoint;
				if (lineIntersect(startPos + up, endPos + up, endPos + nextUp, nextEndPos + nextUp, intersectionPoint))
				{
					intersectUpper = intersectionPoint;
				}
				if (lineIntersect(startPos - up, endPos - up, endPos - nextUp, nextEndPos - nextUp, intersectionPoint))
				{
					intersectLower = intersectionPoint;
				}
			}

			batchData->m_vertices.emplace_back(math::float4(1.0f, 0.0f, 0.0f, 0.0f), fAbsolutePosition + intersectUpper, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			batchData->m_vertices.emplace_back(math::float4(0.0f, 0.0f, 0.0f, 0.0f), fAbsolutePosition + intersectLower, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

			batchData->m_indices.emplace_back(2 * point + 0);
			batchData->m_indices.emplace_back(2 * point - 2);
			batchData->m_indices.emplace_back(2 * point - 1);

			batchData->m_indices.emplace_back(2 * point + 1);
			batchData->m_indices.emplace_back(2 * point + 0);
			batchData->m_indices.emplace_back(2 * point - 1);

			startPos = endPos;
		}

		m_batches.push_back(batchData);
	}
	void ElementList::generateSplineBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<SplineElement> lineElement = std::static_pointer_cast<SplineElement>(element);
		math::float2 absolutePosition = lineElement->m_geometry.getAbsolutePosition(); //todo:add scale
		math::float2 fAbsolutePosition = math::float2(absolutePosition.x, absolutePosition.y);

		std::shared_ptr<BatchData> batchData = std::make_shared<BatchData>();
		math::float4 color = lineElement->m_color;//todo:fix this
		batchData->m_clippingState = getClippingState(element->m_clipIndex);
		batchData->m_layer = element->m_layer;
		batchData->shaderType = UIShaderType::Line;
		const float filterScale = 1.0f;
		float requestedThickness = lineElement->m_thickNess;
		static const float twoRootTwo = 2 * 1.4142135623730950488016887242097f;//todo:fix this
		const float lineThickness = twoRootTwo + requestedThickness;
		const float halfThickness = lineThickness * 0.5f + filterScale;

		batchData->m_shaderParams = ShaderParam(math::float4(requestedThickness, filterScale, 0.0f, 0.0f));

		LineBuilder lineBuilder(*batchData, lineElement->P0, halfThickness, lineElement->m_geometry, Color(color.x, color.y, color.z, color.w));

		lineBuilder.buildBezierGeometry(lineElement->P0, lineElement->P1, lineElement->P2, lineElement->P3);

		m_batches.push_back(batchData);
	}
	void ElementList::generateViewportBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<BatchData> boxBatch = std::make_shared<BatchData>();
		boxBatch->shaderType = UIShaderType::Default;
		boxBatch->m_layer = element->m_layer;
		boxBatch->m_clippingState = getClippingState(element->m_clipIndex);

		std::shared_ptr<ViewportElement> viewportElement = std::static_pointer_cast<ViewportElement>(element);
		//math::double2 absolutePosition = boxElement->m_geometry.getAbsolutePosition();
		math::affine2 transform = viewportElement->m_geometry.getAccumulateRenderTransform();
		math::float2 localSize = viewportElement->m_geometry.getLocalSize();
		//math::double2 localSize = math::double2(200.0f, 200.0f);
		math::float4 color = viewportElement->m_color;

		math::float2 topLeft = math::float2(0.0f, 0.0f);
		math::float2 bottomRight = math::float2(localSize.x, localSize.y);
		math::float2 topRight = math::float2(localSize.x, 0.0f);
		math::float2 bottomLeft = math::float2(0.0f, localSize.y);
		//math::float2 tile = tiling ? math::float2(4.0f, 4.0f) : math::float2(1.0f, 1.0f);
		boxBatch->m_vertices.emplace_back(math::float4(0.0f, 0.0f, 1.0f, 1.0f), transform.transformPoint(topLeft), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(1.0f, 1.0f, 1.0f, 1.0f), transform.transformPoint(bottomRight), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(0.0f, 1.0f, 1.0f, 1.0f), transform.transformPoint(bottomLeft), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(1.0f, 0.0f, 1.0f, 1.0f), transform.transformPoint(topRight), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_indices.emplace_back(0);
		boxBatch->m_indices.emplace_back(1);
		boxBatch->m_indices.emplace_back(2);
		boxBatch->m_indices.emplace_back(0);
		boxBatch->m_indices.emplace_back(3);
		boxBatch->m_indices.emplace_back(1);

		boxBatch->m_texture = viewportElement->m_renderTargetResouce;
		m_batches.push_back(boxBatch);
	}
	void ElementList::generateShapedTextBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<ShapedTextElement> textElement = std::static_pointer_cast<ShapedTextElement>(element);
		std::shared_ptr<ShapedGlyphSequence> shapedGlyphSequence = textElement->m_shapedTextSequence;
		math::float4 color = textElement->m_color;
		std::shared_ptr<FontCache> fontCache = FontCache::getFontCache();

		const int16_t textBaseLine = shapedGlyphSequence->m_textBaseLine;
		const uint16_t maxHeight = shapedGlyphSequence->m_maxTextHeight;

		ShapedTextBuildContext shapedTextBuildContext;
		shapedTextBuildContext.fontCache = fontCache.get();
		shapedTextBuildContext.maxHeight = maxHeight;
		shapedTextBuildContext.textBaseLine = textBaseLine;
		shapedTextBuildContext.shapedGlyphSequence = shapedGlyphSequence.get();
		shapedTextBuildContext.element = textElement.get();
		
		auto buildFontGeometry = [&](const math::float4& inTint, int32_t inLayer)
		{
			math::float2 topLeft(0, 0);

			const float posX = topLeft.x;
			float posY = topLeft.y;

			shapedTextBuildContext.startLineX = posX;
			shapedTextBuildContext.startLineY = posY;
			shapedTextBuildContext.layerId = inLayer;
			shapedTextBuildContext.fontTint = inTint;
			
			buildShapedTextSequence(shapedTextBuildContext);
		};

		buildFontGeometry(color, element->m_layer);
	}
	void ElementList::generateGradientBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<BatchData> boxBatch = std::make_shared<BatchData>();
		boxBatch->shaderType = UIShaderType::Default;
		boxBatch->m_layer = element->m_layer;
		boxBatch->m_clippingState = getClippingState(element->m_clipIndex);

		std::shared_ptr<GradientElement> gradientElement = std::static_pointer_cast<GradientElement>(element);
		//math::double2 absolutePosition = boxElement->m_geometry.getAbsolutePosition();
		math::affine2 transform = gradientElement->m_geometry.getAccumulateRenderTransform();
		math::float2 localSize = gradientElement->m_geometry.getLocalSize();
		//math::double2 localSize = math::double2(200.0f, 200.0f);

		math::bool4 haveCorner = gradientElement->m_cornerRadius != math::float4(0.0f, 0.0f, 0.0f, 0.0f);
		if (haveCorner.x || haveCorner.y || haveCorner.z || haveCorner.w)
		{
			ShaderParam shaderParam;
			shaderParam.pixelParams = math::float4(0.0f, 0.0f, localSize.x, localSize.y);
			shaderParam.pixelParams2 = gradientElement->m_cornerRadius;
			boxBatch->shaderType = UIShaderType::RoundedBox;
			boxBatch->m_shaderParams = shaderParam;
		}

		std::vector<GradientStop> gradientStops = gradientElement->m_gradientStops;

		const GradientStop& firstStop = gradientStops[0];
		const GradientStop& lastStop = gradientStops[gradientStops.size() - 1];

		if (gradientElement->m_orientation == Orientation::Vertical)
		{
			if (0.0f < firstStop.m_position.x)
			{
				auto beg = gradientStops.begin();
				gradientStops.insert(beg, GradientStop(math::float2(0.0f, 0.0f), firstStop.m_color));
			}
			
			if(localSize.x > lastStop.m_position.x)
			{
				gradientStops.push_back(GradientStop(localSize, lastStop.m_color));
			}
		}
		else
		{
			if (0.0f < firstStop.m_position.y)
			{
				auto beg = gradientStops.begin();
				gradientStops.insert(beg, GradientStop(math::float2(0.0f, 0.0f), firstStop.m_color));
			}

			if (localSize.y > lastStop.m_position.y)
			{
				gradientStops.push_back(GradientStop(localSize, lastStop.m_color));
			}
		}

		math::float2 topLeft = math::float2(0.0f, 0.0f);
		math::float2 topRight = math::float2(localSize.x, 0.0f);
		math::float2 bottomLeft = math::float2(0, localSize.y);
		math::float2 bottomRight = math::float2(localSize.x, localSize.y);

		for (int32_t stopIndex = 0; stopIndex < gradientStops.size(); ++stopIndex)
		{
			const uint32_t indexStart = boxBatch->m_vertices.size();

			const GradientStop& curStop = gradientStops[stopIndex];

			math::float2 startPt;
			math::float2 endPt;

			math::float2 startUV;
			math::float2 endUV;

			if (gradientElement->m_orientation == Orientation::Vertical)
			{
				startPt = topLeft;
				endPt = bottomLeft;

				startPt.x += curStop.m_position.x;
				endPt.x += curStop.m_position.x;

				startUV.x = startPt.x / topRight.x;
				startUV.y = 0;

				endUV.x = endPt.x / topRight.x;
				endUV.y = 1.0;
			}
			else
			{
				startPt = topLeft;
				endPt = topRight;

				startPt.y += curStop.m_position.y;
				endPt.y += curStop.m_position.y;

				startUV.x = 0;
				startUV.y = startPt.y / bottomLeft.y;

				endUV.x = 1;
				endUV.y = startPt.y / bottomLeft.y;
			}

			if (stopIndex == 0)
			{
				boxBatch->m_vertices.emplace_back(math::float4(startUV.x, startUV.y, 1.0f, 1.0f), transform.transformPoint(startPt), curStop.m_color.toFloat4(), math::float4(1.0f, 1.0f, 1.0f, 1.0f));
				boxBatch->m_vertices.emplace_back(math::float4(endUV.x, endUV.y, 1.0f, 1.0f), transform.transformPoint(endPt), curStop.m_color.toFloat4(), math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				boxBatch->m_vertices.emplace_back(math::float4(startUV.x, startUV.y, 1.0f, 1.0f), transform.transformPoint(startPt), curStop.m_color.toFloat4(), math::float4(1.0f, 1.0f, 1.0f, 1.0f));
				boxBatch->m_vertices.emplace_back(math::float4(endUV.x, endUV.y, 1.0f, 1.0f), transform.transformPoint(endPt), curStop.m_color.toFloat4(), math::float4(1.0f, 1.0f, 1.0f, 1.0f));
				//boxBatch->m_vertices.emplace_back(math::float4(0.0f, 1.0f, 1.0f, 1.0f), transform.transformPoint(bottomLeft), curStop.m_color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
				//boxBatch->m_vertices.emplace_back(math::float4(1.0f, 0.0f, 1.0f, 1.0f), transform.transformPoint(topRight), curStop.m_color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

				boxBatch->m_indices.emplace_back(indexStart + 0);
				boxBatch->m_indices.emplace_back(indexStart + 1);
				boxBatch->m_indices.emplace_back(indexStart - 1);
				boxBatch->m_indices.emplace_back(indexStart + 0);
				boxBatch->m_indices.emplace_back(indexStart - 1);
				boxBatch->m_indices.emplace_back(indexStart - 2);
			}
		}

		m_batches.push_back(boxBatch);
	}

	void ElementList::generateBorderBatch(std::shared_ptr<Element> element)
	{
		//border batch
		std::shared_ptr<BatchData> boxBatch = std::make_shared<BatchData>();
		boxBatch->shaderType = UIShaderType::Border;
		boxBatch->m_layer = element->m_layer;
		boxBatch->m_clippingState = getClippingState(element->m_clipIndex);

		std::shared_ptr<BoxElement> boxElement = std::static_pointer_cast<BoxElement>(element);
		math::affine2 transform = boxElement->m_geometry.getAccumulateRenderTransform();
		math::float2 localSize = boxElement->m_geometry.getLocalSize();

		math::affine2 layoutTransform = boxElement->m_geometry.getAccumulateLayoutTransform();
		math::affine2 inverseLayoutTransform = math::inverse(layoutTransform);

		//texture width, height
		uint32_t textureWidth = 1;
		uint32_t textureHeight = 1;
		if (boxElement->m_brush->m_texture)
		{
			textureWidth = boxElement->m_brush->m_actualSize.x;
			textureHeight = boxElement->m_brush->m_actualSize.y;
		}

		math::float2 textureSizeLocalSpace = inverseLayoutTransform.transformVector(math::float2(textureWidth, textureHeight));//这个只是将纹理缩放了scale倍

		const math::float2 startUV(0, 0);
		const math::float2 endUV(1.0f, 1.0f);

		Padding margin = boxElement->m_brush->m_margin;

		math::float2 topLeft(0, 0);
		math::float2 bottomRight(localSize);

		bool bIsFlippedX = topLeft.x > bottomRight.x;
		bool bIsFlippedY = topLeft.y > bottomRight.y;
		margin.left = bIsFlippedX ? -margin.left : margin.left;
		margin.top = bIsFlippedY ? -margin.top : margin.top;
		margin.right = bIsFlippedX ? -margin.right : margin.right;
		margin.bottom = bIsFlippedY ? -margin.bottom : margin.bottom;

		math::float2 topLeftMargin(textureSizeLocalSpace * math::float2(margin.left, margin.top));
		math::float2 bottomRightMargin(localSize - textureSizeLocalSpace * math::float2(margin.right, margin.bottom));

		float leftMarginX = topLeftMargin.x;
		float topMarginY = topLeftMargin.y;
		float rightMarginX = bottomRightMargin.x;
		float bottomMarginY = bottomRightMargin.y;

		//handle overlap
		if (std::abs(rightMarginX) < std::abs(leftMarginX))
		{
			leftMarginX = localSize.x / 2.0f;
			rightMarginX = leftMarginX;
		}

		if (std::abs(bottomMarginY) < std::abs(topMarginY))
		{
			topMarginY = localSize.y / 2.0f;
			bottomMarginY = topMarginY;
		}

		float leftMarginU = std::abs(margin.left);
		float topMarginV = std::abs(margin.top);
		float rightMarginU = 1.0f - std::abs(margin.right);
		float bottomMarginV = 1.0f - std::abs(margin.bottom);

		float topTiling = 1.0f;
		float leftTiling = 1.0f;
		float denom = textureSizeLocalSpace.x * (1.0f - margin.getTotalSpaceAlong<Orientation::Horizontal>());
		if (!math::isnear(denom, 0))
		{
			topTiling = (rightMarginX - leftMarginX) / denom;//中间拉伸的尺寸
		}
		denom = textureSizeLocalSpace.y * (1.0f - margin.getTotalSpaceAlong<Orientation::Vertical>());
		if (!math::isnear(denom, 0))
		{
			leftTiling = (bottomMarginY - topMarginY) / denom;
		}
		boxBatch->m_shaderParams = ShaderParam(math::float4(leftMarginU, rightMarginU, topMarginV, bottomMarginV));
		
		//保证平铺至少为1
		topTiling = topTiling >= 1.0f ? topTiling : 1.0f;
		leftTiling = leftTiling >= 1.0f ? leftTiling : 1.0f;
		float rightTiling = leftTiling;
		float bottomTiling = topTiling;

		math::float2 position = topLeft;
		math::float2 endPosition = bottomRight;

		math::float4 color = boxElement->m_brush->m_tintColor;

		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, startUV.y, 0.0f, 0.0f), transform.transformPoint(position), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, topMarginV, 0.0f, 0.0f), transform.transformPoint(math::float2(position.x, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, startUV.y, 0.0f, 0.0f), transform.transformPoint(math::float2(leftMarginX, position.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, topMarginV, 0.0f, 0.0f), transform.transformPoint(math::float2(leftMarginX, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, startUV.y, topTiling, 0.0f), transform.transformPoint(math::float2(leftMarginX, position.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, topMarginV, topTiling, 0.0f), transform.transformPoint(math::float2(leftMarginX, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, startUV.y, topTiling, 0.0f), transform.transformPoint(math::float2(rightMarginX, position.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, topMarginV, topTiling, 0.0f), transform.transformPoint(math::float2(rightMarginX, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, startUV.y, 0.0f, 0.0f), transform.transformPoint(math::float2(rightMarginX, position.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, topMarginV, 0.0f, 0.0f), transform.transformPoint(math::float2(rightMarginX, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, startUV.y, 0.0f, 0.0f), transform.transformPoint(math::float2(endPosition.x, position.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, topMarginV, 0.0f, 0.0f), transform.transformPoint(math::float2(endPosition.x, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, startUV.y, 0.0f, leftTiling), transform.transformPoint(math::float2(position.x, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, topMarginV, 0.0f, leftTiling), transform.transformPoint(math::float2(position.x, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, startUV.y, 0.0f, leftTiling), transform.transformPoint(math::float2(leftMarginX, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, topMarginV, leftTiling), transform.transformPoint(math::float2(leftMarginX, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, startUV.y, 0.0f, rightTiling), transform.transformPoint(math::float2(rightMarginX, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, endUV.y, 0.0f, rightTiling), transform.transformPoint(math::float2(rightMarginX, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, startUV.y, 0.0f, rightTiling), transform.transformPoint(math::float2(endPosition.x, topMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, endUV.y, 0.0f, rightTiling), transform.transformPoint(math::float2(endPosition.x, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, bottomMarginV, 0.0f, 0.0f), transform.transformPoint(math::float2(position.x, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, endUV.y, 0.0f, 0.0f), transform.transformPoint(math::float2(position.x, endPosition.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, bottomMarginV, 0.0f, 0.0f), transform.transformPoint(math::float2(leftMarginX, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(leftMarginU, endUV.y, 0.0f, 0.0f), transform.transformPoint(math::float2(leftMarginX, endPosition.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, bottomMarginV, bottomTiling, 0.0f), transform.transformPoint(math::float2(leftMarginX, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(startUV.x, endUV.y, bottomTiling, 0.0f), transform.transformPoint(math::float2(leftMarginX, endPosition.x)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, bottomMarginV, bottomTiling, 0.0f), transform.transformPoint(math::float2(rightMarginX, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, endUV.y, bottomTiling, 0.0f), transform.transformPoint(math::float2(rightMarginX, endPosition.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, bottomMarginV, 0.0f, 0.0f), transform.transformPoint(math::float2(rightMarginX, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(rightMarginU, endUV.y, 0.0f, 0.0f), transform.transformPoint(math::float2(rightMarginX, endPosition.x)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, bottomMarginV, 0.0f, 0.0f), transform.transformPoint(math::float2(endPosition.x, bottomMarginY)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(endUV.x, endUV.y, 0.0f, 0.0f), transform.transformPoint(math::float2(endPosition.x, endPosition.y)), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		//top
		boxBatch->m_indices.emplace_back(0);
		boxBatch->m_indices.emplace_back(2);
		boxBatch->m_indices.emplace_back(1);
		boxBatch->m_indices.emplace_back(2);
		boxBatch->m_indices.emplace_back(3);
		boxBatch->m_indices.emplace_back(1);

		boxBatch->m_indices.emplace_back(4);
		boxBatch->m_indices.emplace_back(6);
		boxBatch->m_indices.emplace_back(5);
		boxBatch->m_indices.emplace_back(6);
		boxBatch->m_indices.emplace_back(7);
		boxBatch->m_indices.emplace_back(5);

		boxBatch->m_indices.emplace_back(8);
		boxBatch->m_indices.emplace_back(10);
		boxBatch->m_indices.emplace_back(9);
		boxBatch->m_indices.emplace_back(10);
		boxBatch->m_indices.emplace_back(11);
		boxBatch->m_indices.emplace_back(9);

		//middle
		boxBatch->m_indices.emplace_back(12);
		boxBatch->m_indices.emplace_back(14);
		boxBatch->m_indices.emplace_back(13);
		boxBatch->m_indices.emplace_back(14);
		boxBatch->m_indices.emplace_back(15);
		boxBatch->m_indices.emplace_back(13);

		boxBatch->m_indices.emplace_back(16);
		boxBatch->m_indices.emplace_back(18);
		boxBatch->m_indices.emplace_back(17);
		boxBatch->m_indices.emplace_back(18);
		boxBatch->m_indices.emplace_back(19);
		boxBatch->m_indices.emplace_back(17);

		//bottom
		boxBatch->m_indices.emplace_back(20);
		boxBatch->m_indices.emplace_back(22);
		boxBatch->m_indices.emplace_back(21);
		boxBatch->m_indices.emplace_back(22);
		boxBatch->m_indices.emplace_back(23);
		boxBatch->m_indices.emplace_back(21);

		boxBatch->m_indices.emplace_back(24);
		boxBatch->m_indices.emplace_back(26);
		boxBatch->m_indices.emplace_back(25);
		boxBatch->m_indices.emplace_back(26);
		boxBatch->m_indices.emplace_back(27);
		boxBatch->m_indices.emplace_back(25);

		boxBatch->m_indices.emplace_back(28);
		boxBatch->m_indices.emplace_back(30);
		boxBatch->m_indices.emplace_back(29);
		boxBatch->m_indices.emplace_back(30);
		boxBatch->m_indices.emplace_back(31);
		boxBatch->m_indices.emplace_back(29);

		boxBatch->m_texture = boxElement->m_brush->m_texture;
		m_batches.push_back(boxBatch);
	}

	void ElementList::buildShapedTextSequence(const ShapedTextBuildContext& context)
	{
		const ShapedGlyphSequence* glyphSequenceToRender = context.shapedGlyphSequence;

		float invTextureSizeX = 1.0f / 1024.0f;
		float invTextureSizeY = 1.0f / 1024.0f;

		float lineX = context.startLineX;
		float lineY = context.startLineY;

		int32_t numGlyphs = glyphSequenceToRender->m_glyphsToRender.size();
		const std::vector<GlyphEntry>& glyphsToRender = glyphSequenceToRender->m_glyphsToRender;
		for (int32_t glyphIndex = 0; glyphIndex < numGlyphs; ++glyphIndex)
		{
			const GlyphEntry& glyphToRender = glyphsToRender[glyphIndex];

			float X = 0;
			float SizeX = 0;
			float Y = 0;
			float U = 0;
			float V = 0;
			float SizeY = 0;
			float SizeU = 0;
			float SizeV = 0;

			const GlyphFontAtlasData glyphAtlasData = context.fontCache->getShapedGlyphFontAtlasData(glyphToRender);
			X += lineX + glyphAtlasData.horizontalOffset + glyphToRender.xOffset;

			std::shared_ptr<BatchData> batch = std::make_shared<BatchData>();
			batch->shaderType = UIShaderType::Font;
			batch->m_layer = context.element->m_layer;
			batch->m_clippingState = getClippingState(context.element->m_clipIndex);

			Y = lineY - glyphAtlasData.verticalOffset + glyphToRender.yOffset + ((context.maxHeight + context.textBaseLine));
			U = glyphAtlasData.startU * invTextureSizeX;
			V = glyphAtlasData.startV * invTextureSizeY;
			SizeX = glyphAtlasData.uSize;
			SizeY = glyphAtlasData.vSize;
			SizeU = glyphAtlasData.uSize * invTextureSizeX;
			SizeV = glyphAtlasData.vSize * invTextureSizeY;

			math::affine2 transform = context.element->m_geometry.getAccumulateRenderTransform();
			transform.m_linear = math::float2x2::identity();
			math::float4 color = static_cast<ShapedTextElement*>(context.element)->m_color;
			math::float2 UpperLeft = transform.transformPoint(math::float2(X, Y));
			math::float2 UpperRight = transform.transformPoint(math::float2(X + SizeX, Y));
			math::float2 LowerLeft = transform.transformPoint(math::float2(X, Y + SizeY));
			math::float2 LowerRight = transform.transformPoint(math::float2(X + SizeX, Y + SizeY));

			batch->m_vertices.emplace_back(math::float4(U, V, 1.0f, 1.0f), math::float2(UpperLeft.x, UpperLeft.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			batch->m_vertices.emplace_back(math::float4(U + SizeU, V + SizeV, 1.0f, 1.0f), math::float2(LowerRight.x, LowerRight.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			batch->m_vertices.emplace_back(math::float4(U, V + SizeV, 1.0f, 1.0f), math::float2(LowerLeft.x, LowerLeft.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			batch->m_vertices.emplace_back(math::float4(U + SizeU, V, 1.0f, 1.0f), math::float2(UpperRight.x, UpperRight.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

			batch->m_indices.emplace_back(0);
			batch->m_indices.emplace_back(1);
			batch->m_indices.emplace_back(2);
			batch->m_indices.emplace_back(0);
			batch->m_indices.emplace_back(3);
			batch->m_indices.emplace_back(1);

			batch->m_texture = FontCache::getFontCache()->getFontAtlasTexture();

			lineX += glyphToRender.xAdvance;
			lineY += glyphToRender.yAdvance;

			m_batches.push_back(batch);
		}
	}
}