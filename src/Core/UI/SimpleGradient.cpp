#include <pch.h>

#include "SimpleGradient.h"
#include <Core/UI/Element.h>
#include <Core/UI/ElementList.h>

namespace GuGu {

	void SimpleGradient::init(const BuilderArguments& arguments)
	{
		m_startColor = arguments.mstartColor;
		m_endColor = arguments.mendColor;
		m_bHasAlphaBackground = arguments.mhasAlphaBackground.Get();
		m_orientation = arguments.morientation.Get();
	}

	uint32_t SimpleGradient::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		//todo:add alpha background

		std::vector<GradientStop> gradientStop;
		gradientStop.push_back(GradientStop(math::float2(0, 0), m_startColor.Get()));
		gradientStop.push_back(GradientStop(allocatedGeometry.getLocalSize(), m_endColor.Get()));

		
		ElementList::addGradientElement(
			elementList,
			allocatedGeometry,
			gradientStop,
			m_orientation,
			math::float4(0, 0, 0, 0), layer + 1);

		return layer + 1;
	}

}