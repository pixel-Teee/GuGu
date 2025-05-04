#include <pch.h>

#include "ColorPicker.h"
#include "ColorWheel.h"
#include <Application/Application.h>
#include <Core/UI/Border.h>

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
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
        m_childWidget->m_childWidget = WIDGET_NEW(ColorWheel);
        m_childWidget->m_childWidget->setParentWidget(shared_from_this());
    }
   
    bool openColorPicker(const ColorPickerArgs& args)
    {
		destroyColorPicker();
		bool result = false;

        math::float2 cursorPos = Application::getApplication()->getCursorPos();
		math::box2 anchor;
		anchor.m_mins = math::float2(cursorPos.x, cursorPos.y);
		anchor.m_maxs = math::float2(cursorPos.x, cursorPos.y);
        math::float2 adjustedSummonLocation = Application::getApplication()->calculatePopupWindowPosition(anchor, ColorPicker::DEFAULT_WINDOW_SIZE, true, math::float2(0, 0), Orientation::Horizontal);

        std::shared_ptr<WindowWidget> window = nullptr;
        std::shared_ptr<Border> windowContent = WIDGET_NEW(Border);

        std::shared_ptr<ColorPicker> createColorPicker = WIDGET_NEW(ColorPicker);

        windowContent->setContent(createColorPicker);

		window = Application::getApplication()->pushMenu(
			args.m_parentWidget,
			WidgetPath(),
			windowContent,
			adjustedSummonLocation,
			true,//todo:这个要改为false
			math::float2(0.0f, 0.0f),
			PopupMethod::CreateNewWindow,
			false)->getOwnedWindow();

        return false;
    }
    void destroyColorPicker()
    {
       
    }
}