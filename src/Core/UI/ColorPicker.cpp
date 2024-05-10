#include <pch.h>

#include "ColorPicker.h"

namespace GuGu {
    const math::float2 ColorPicker::DEFAULT_WINDOW_SIZE = math::float2(441, 537);//默认窗口大小

    const double ColorPicker::MAX_ALLOWED_UPDATE_TIME = 0.1f;

    ColorPicker::ColorPicker()
    {
    }
    ColorPicker::~ColorPicker()
    {
    }
    void ColorPicker::init(const BuilderArguments& arguments)
    {
    }
    uint32_t ColorPicker::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
        return 0;
    }
    math::float2 ColorPicker::ComputeFixedSize(float inLayoutScaleMultiplier)
    {
        return math::float2();
    }
    void ColorPicker::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
    {
    }
    SlotBase* ColorPicker::getSlot(uint32_t index) const
    {
        return nullptr;
    }
    uint32_t ColorPicker::getSlotsNumber() const
    {
        return 0;
    }
    bool openColorPicker(const ColorPickerArgs& args)
    {
		destroyColorPicker();
		bool result = false;
        return false;
    }
    void destroyColorPicker()
    {
       
    }
}