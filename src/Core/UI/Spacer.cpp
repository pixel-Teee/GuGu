#include <pch.h>

#include "Spacer.h"

namespace GuGu {
    Spacer::Spacer()
    {
    }
    Spacer::~Spacer()
    {
    }
    void Spacer::init(const BuilderArguments& arguments)
    {
        m_spacerSize = arguments.msize;
    }
    uint32_t Spacer::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
        return layer;
    }
    math::float2 Spacer::ComputeFixedSize(float inLayoutScaleMultiplier)
    {
        return m_spacerSize.Get();
    }
}