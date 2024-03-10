#pragma once

#include "ElementList.h"

namespace GuGu {
	struct LineBuilder
	{
		LineBuilder(BatchData& inRenderBatch, const math::float2 startPoint, float halfThickness, const WidgetGeometry& inTransform, const Color& inColor)
			: m_renderBatch(inRenderBatch)
			, m_transform(inTransform)
			, m_lastPointAdded()
			, m_lastNormal(math::float2(0.0f, 0.0f))
			, m_halfLineThickness(halfThickness)
			, m_numPointsAdded(1)
			, m_singleColor(inColor)
		{
			m_lastPointAdded[0] = m_lastPointAdded[1] = startPoint;
		}

		static float computeCurviness(const math::float2& p0, const math::float2& p1, const math::float2& p2, const math::float2& p3)
		{
			math::float2 twoP1Deviations = p0 + p2 - 2.0f * p1;
			math::float2 twoP2Deviations = p1 + p3 - 2.0f * p2;
			float twoDeviations = math::abs(twoP1Deviations.x) + math::abs(twoP1Deviations.y) + math::abs(twoP2Deviations.x) + math::abs(twoP2Deviations.y);
			return twoDeviations;
		}

		static void deCasteljauSplit(const math::float2& p0, const math::float2& p1, const math::float2& p2, const math::float2& p3, math::float2 outCurveParams[7])
		{
			math::float2 L1 = (p0 + p1) * 0.5f;
			math::float2 M = (p1 + p2) * 0.5f;
			math::float2 R2 = (p2 + p3) * 0.5f;

			math::float2 L2 = (L1 + M) * 0.5f;
			math::float2 R1 = (M + R2) * 0.5f;

			math::float2 L3R0 = (L2 + R1) * 0.5f;

			outCurveParams[0] = p0;
			outCurveParams[1] = L1;
			outCurveParams[2] = L2;
			outCurveParams[3] = L3R0;
			outCurveParams[4] = R1;
			outCurveParams[5] = R2;
			outCurveParams[6] = p3;
		}

		static void subdivide(const math::float2& p0, const math::float2& p1, const math::float2& p2, const math::float2& p3, LineBuilder& lineBuilder, float maxBiasTimesTwo = 2.0f)
		{
			const float curviness = computeCurviness(p0, p1, p2, p3);
			if (curviness > maxBiasTimesTwo)
			{
				//split the bezier into two curves
				math::float2 twoCurves[7];
				deCasteljauSplit(p0, p1, p2, p3, twoCurves);

				//subdivice left, then right
				subdivide(twoCurves[0], twoCurves[1], twoCurves[2], twoCurves[3], lineBuilder, maxBiasTimesTwo);
				subdivide(twoCurves[3], twoCurves[4], twoCurves[5], twoCurves[6], lineBuilder, maxBiasTimesTwo);
			}
			else
			{
				lineBuilder.appendPoint(p3, lineBuilder.m_singleColor);
			}
		}

		void buildBezierGeometry(const math::float2& p0, const math::float2& p1, const math::float2& p2, const math::float2& p3)
		{
			subdivide(p0, p1, p2, p3, *this, 1.0f);
			finish(p3, m_singleColor);
		}

		void finish(const math::float2& lastPoint, const Color& inColor)
		{
			math::float4 color = math::float4(inColor.r, inColor.g, inColor.b, inColor.a);

			if (m_numPointsAdded < 3)
			{
				appendPoint(lastPoint, inColor);
			}
			else
			{
				math::float2 lastUp = m_lastNormal * m_halfLineThickness;

				math::float2 absolutePosition = m_transform.getAbsolutePosition();
				math::float2 fabsolutePosition = math::float2(absolutePosition.x, absolutePosition.y);
				m_renderBatch.m_vertices.emplace_back(math::float4(1.0f, 0.0f, 0.0f, 0.0f), fabsolutePosition + m_lastPointAdded[1] + lastUp, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
				m_renderBatch.m_vertices.emplace_back(math::float4(0.0f, 0.0f, 0.0f, 0.0f), fabsolutePosition + m_lastPointAdded[1] - lastUp, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

				const int32_t numVerts = m_renderBatch.m_vertices.size();

				m_renderBatch.m_indices.emplace_back(numVerts - 2);
				m_renderBatch.m_indices.emplace_back(numVerts - 4);
				m_renderBatch.m_indices.emplace_back(numVerts - 3);

				m_renderBatch.m_indices.emplace_back(numVerts - 1);
				m_renderBatch.m_indices.emplace_back(numVerts - 2);
				m_renderBatch.m_indices.emplace_back(numVerts - 3);
			}
		}

	private:
		void appendPoint(const math::float2 newPoint, const Color& inColor)
		{
			const math::float2 newNormal = math::normalize(math::float2(m_lastPointAdded[0].y - newPoint.y, newPoint.x - m_lastPointAdded[0].x));

			math::float4 color = math::float4(inColor.r, inColor.g, inColor.b, inColor.a);

			if (m_numPointsAdded == 2)
			{
				//once we have two points, we have a normal, so we can generate the first bit of geometry
				math::float2 lastUp = m_lastNormal * m_halfLineThickness;

				math::float2 absolutePosition = m_transform.getAbsolutePosition();
				math::float2 fabsolutePosition = math::float2(absolutePosition.x, absolutePosition.y);
				m_renderBatch.m_vertices.emplace_back(math::float4(1.0f, 0.0f, 0.0f, 0.0f), fabsolutePosition + m_lastPointAdded[1] + lastUp, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
				m_renderBatch.m_vertices.emplace_back(math::float4(0.0f, 0.0f, 0.0f, 0.0f), fabsolutePosition + m_lastPointAdded[1] - lastUp, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			}

			if (m_numPointsAdded >= 2)
			{
				const math::float2 averagedUp = math::normalize((0.5f * (newNormal + m_lastNormal))) * m_halfLineThickness;

				math::float2 absolutePosition = m_transform.getAbsolutePosition();
				math::float2 fabsolutePosition = math::float2(absolutePosition.x, absolutePosition.y);
				m_renderBatch.m_vertices.emplace_back(math::float4(1.0f, 0.0f, 0.0f, 0.0f), fabsolutePosition + m_lastPointAdded[0] + averagedUp, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
				m_renderBatch.m_vertices.emplace_back(math::float4(0.0f, 0.0f, 0.0f, 0.0f), fabsolutePosition + m_lastPointAdded[0] - averagedUp, color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

				const int32_t numVerts = m_renderBatch.m_vertices.size();

				m_renderBatch.m_indices.emplace_back(numVerts - 2);
				m_renderBatch.m_indices.emplace_back(numVerts - 4);
				m_renderBatch.m_indices.emplace_back(numVerts - 3);

				m_renderBatch.m_indices.emplace_back(numVerts - 1);
				m_renderBatch.m_indices.emplace_back(numVerts - 2);
				m_renderBatch.m_indices.emplace_back(numVerts - 3);
			}

			m_lastPointAdded[1] = m_lastPointAdded[0];
			m_lastPointAdded[0] = newPoint;
			m_lastNormal = newNormal;

			++m_numPointsAdded;
		}
		BatchData& m_renderBatch;
		const WidgetGeometry& m_transform;
		math::float2 m_lastPointAdded[2];
		math::float2 m_lastNormal;
		float m_halfLineThickness;
		int32_t m_numPointsAdded;
		Color m_singleColor;
	};
}