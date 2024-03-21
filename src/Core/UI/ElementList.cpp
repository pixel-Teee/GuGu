#include <pch.h>

#include "ElementList.h"
#include "Element.h"
#include "BoxElement.h"
#include "LineElement.h"
#include "SplineElement.h"
#include "ViewportElement.h"
#include "TextElement.h"
#include "ShapedTextElement.h"
#include "Brush.h"
#include "FontCache.h"
#include "CharacterList.h"
#include "LineBuilder.h"

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
		if (brush->m_texture == nullptr)
			return;
		
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
			}
		}

		std::vector<std::shared_ptr<BatchData>> batches;
		uint32_t lastBatch = 0;
		//merge batches
		for (size_t i = 1; i < m_batches.size(); ++i)
		{
			if (m_batches[i]->shaderType == m_batches[lastBatch]->shaderType
				&& m_batches[i]->m_texture == m_batches[lastBatch]->m_texture
				&& m_batches[i]->m_layer == m_batches[lastBatch]->m_layer && m_batches[i]->m_clippingState == m_batches[lastBatch]->m_clippingState)
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
		//math::double2 absolutePosition = boxElement->m_geometry.getAbsolutePosition();
		math::affine2 transform = boxElement->m_geometry.getAccumulateTransform();
		math::float2 localSize = boxElement->m_geometry.getLocalSize();
		//math::double2 localSize = math::double2(200.0f, 200.0f);
		math::float4 color = boxElement->m_color;
		std::shared_ptr<Brush> brush = boxElement->m_brush;
		bool tiling = element->m_tiling;
		math::float2 tile = tiling ?  math::float2(localSize.x / brush->m_actualSize.x, localSize.y / brush->m_actualSize.x) : math::float2(1.0f, 1.0f);

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
	void ElementList::generateTextBatch(std::shared_ptr<Element> element)
	{
		std::shared_ptr<TextElement> textElement = std::static_pointer_cast<TextElement>(element);
		float scale = textElement->m_geometry.getAbsoluteScale();
		//math::float2 absolutePosition = textElement->m_geometry.getAbsolutePosition();
		math::affine2 transform = textElement->m_geometry.getAccumulateTransform();
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
		static const float twoRootTwo = 2 * 1.4142135623730950488016887242097f;//todo:fix this
		const float lineThickness = twoRootTwo + requestedThickness;
		const float halfThickness = lineThickness * 0.5f + filterScale;

		math::float2 startPos = points[0];
		math::float2 endPos = points[1];

		math::float2 normal = math::normalize(math::float2(startPos.y - endPos.y, endPos.x - startPos.x));
		math::float2 up = normal * halfThickness;

		batchData->m_vertices.emplace_back(math::float4(1.0f, 0.0f, 0.0f, 0.0f), fAbsolutePosition + startPos + up, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		batchData->m_vertices.emplace_back(math::float4(1.0f, 0.0f, 0.0f, 0.0f), fAbsolutePosition + startPos - up, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

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
		math::affine2 transform = viewportElement->m_geometry.getAccumulateTransform();
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

			math::affine2 transform = context.element->m_geometry.getAccumulateTransform();
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