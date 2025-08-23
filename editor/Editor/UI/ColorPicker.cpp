#include <pch.h>

#include "ColorPicker.h"
#include <Core/UI/ColorWheel.h>
#include <Application/Application.h>
#include <Core/UI/Border.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/ComplexGradient.h>
#include <Core/UI/Slider.h>
#include <Core/UI/SimpleGradient.h>
#include <Core/UI/Button.h>
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/Spacer.h>

#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {
    const math::float2 ColorPicker::DEFAULT_WINDOW_SIZE = math::float2(441, 537);//默认窗口大小

	Color ColorPicker::handleColorSliderEndColor(ColorPickerChannels channel) const
	{
        switch (channel)
        {
        case GuGu::ColorPickerChannels::Red: 
            return Color(0.0f, 0.0f, 0.0f, 1.0f);
        case GuGu::ColorPickerChannels::Green:
            return Color(0.0f, 0.0f, 0.0f, 1.0f);
        case GuGu::ColorPickerChannels::Blue:
            return Color(0.0f, 0.0f, 0.0f, 1.0f);
        case GuGu::ColorPickerChannels::Alpha:
            return Color(m_currentColorRGB.r, m_currentColorRGB.g, m_currentColorRGB.b, 0.0f);
        case GuGu::ColorPickerChannels::Saturation://饱和度
            return Color(m_currentColorHSV.r, 0.0, 1.0, 1.0).HSVToLinearRGB();
        case GuGu::ColorPickerChannels::Value://明度
            return Color(m_currentColorHSV.r, m_currentColorHSV.g, 0.0, 1.0).HSVToLinearRGB();
        default:
            return Color();
        }
	}

	Color ColorPicker::handleColorSliderStartColor(ColorPickerChannels channel) const
	{
		switch (channel)
		{
		case GuGu::ColorPickerChannels::Red:
            return Color(1.0f, 0.0f, 0.0f, 1.0f);
		case GuGu::ColorPickerChannels::Green:
            return Color(0.0f, 1.0f, 0.0f, 1.0f);
		case GuGu::ColorPickerChannels::Blue:
            return Color(0.0f, 0.0f, 1.0f, 1.0f);
		case GuGu::ColorPickerChannels::Alpha:
            return Color(m_currentColorRGB.r, m_currentColorRGB.g, m_currentColorRGB.b, 1.0f);
		case GuGu::ColorPickerChannels::Saturation:
            return Color(m_currentColorHSV.r, 1.0, 1.0, 1.0).HSVToLinearRGB();
		case GuGu::ColorPickerChannels::Value:
            return Color(m_currentColorHSV.r, m_currentColorHSV.g, 1.0, 1.0).HSVToLinearRGB();
		default:
			break;
		}
	}

	void ColorPicker::handleColorSpinBoxValueChanged(float newValue, ColorPickerChannels channel)
	{
        int32_t componentIndex;
        bool isHsv = false;

		switch (channel)
		{
		case GuGu::ColorPickerChannels::Red:
            componentIndex = 0;
            break;
		case GuGu::ColorPickerChannels::Green:
			componentIndex = 1;
			break;
		case GuGu::ColorPickerChannels::Blue:
			componentIndex = 2;
			break;
		case GuGu::ColorPickerChannels::Alpha:
			componentIndex = 3;
			break;
		case GuGu::ColorPickerChannels::Hue:
			componentIndex = 0;
            isHsv = true;
			break;
		case GuGu::ColorPickerChannels::Saturation:
			componentIndex = 1;
            isHsv = true;
			break;
		case GuGu::ColorPickerChannels::Value:
			componentIndex = 2;
            isHsv = true;
			break;
		default:
            return;
		}

        Color& newColor = isHsv ? m_currentColorHSV : m_currentColorRGB;

        if (math::isnear(newValue, newColor.component(componentIndex)))
        {
            return;
        }

        newColor.component(componentIndex) = newValue;

        if (isHsv)
        {
            setNewTargetColorHsv(newColor, !m_bIsInteractive);
        }
        else
        {
            setNewTargetColorRgb(newColor, !m_bIsInteractive);
        }
	}

	float ColorPicker::handleColorSpinBoxValue(ColorPickerChannels channel) const
	{
        switch (channel)
        {
        case GuGu::ColorPickerChannels::Red:
            return m_currentColorRGB.r;
		case GuGu::ColorPickerChannels::Green:
            return m_currentColorRGB.g;
		case GuGu::ColorPickerChannels::Blue:
            return m_currentColorRGB.b;
		case GuGu::ColorPickerChannels::Alpha:
            return m_currentColorRGB.a;
		case GuGu::ColorPickerChannels::Hue:
            return m_currentColorHSV.r;
		case GuGu::ColorPickerChannels::Saturation:
            return m_currentColorHSV.g;
		case GuGu::ColorPickerChannels::Value:
            return m_currentColorHSV.b;
		default:
            return 0.0f;
        }
	}

	bool ColorPicker::setNewTargetColorRgb(const Color& newValue, bool bForceUpdate /*= false*/)
	{
		m_currentColorRGB = newValue;
		m_currentColorHSV = newValue.LinearRGBToHSV();

        return applyNewTargetColor(bForceUpdate);
	}

	bool ColorPicker::setNewTargetColorHsv(const Color& newValue, bool bForceUpdate /*= false*/)
	{
		m_currentColorHSV = newValue;
		m_currentColorRGB = newValue.HSVToLinearRGB();

        return applyNewTargetColor(bForceUpdate);
	}

	bool ColorPicker::applyNewTargetColor(bool bForceUpdate /*= false*/)
	{
        bool bUpdated = false;
        if (bForceUpdate)
        {
            updateColorPickMouseUp();
            bUpdated = true;
        }

        return bUpdated;
	}

	void ColorPicker::updateColorPickMouseUp()
	{
        if (m_bColorPickerIsInlineVersion)
        {
            updateColorPick();
        }
	}

	void ColorPicker::updateColorPick()
	{
        Color outColor = m_currentColorRGB;

        if (m_preColorCommitted)
            m_preColorCommitted(outColor);

        setColors(outColor);

        if (m_onColorCommitted)
            m_onColorCommitted(outColor);
	}

	void ColorPicker::handleColorSpectrumValueChanged(Color newValue)
	{
        setNewTargetColorHsv(newValue);
	}

	Reply ColorPicker::handleOkButtonClicked()
	{
        updateColorPick();

        m_parentWindowPtr.lock()->requestDestroyWindow();

        return Reply::Handled();
	}

	const double ColorPicker::MAX_ALLOWED_UPDATE_TIME = 0.1f;

    ColorPicker::ColorPicker()
    {
    }
    ColorPicker::~ColorPicker()
    {
    }
    void ColorPicker::init(const BuilderArguments& arguments)
    {
        //color picker 可以根据参数提供的颜色指针，直接修改值
        //或者提供的回调去修改值
        m_bColorPickerIsInlineVersion = arguments.mdisplayInlineVersion.Get();//是否使用内联版本，相比于另外一个版本，没有主题管理器和直方图
        m_bUseAlpha = arguments.museAlpha;//透明度滑条
        m_targetColorAttribute = arguments.mtargetColorAttribute;
        m_currentColorHSV = m_oldColor = m_targetColorAttribute.Get().LinearRGBToHSV();
        m_currentColorRGB = m_targetColorAttribute.Get();
        m_targetFColors = arguments.mtargetFColors.Get();
        m_targetLinearColors = arguments.mtargetLinearColors.Get();
        m_onColorCommitted = arguments.monColorCommitted;
        m_preColorCommitted = arguments.mpreColorCommitted;
        m_parentWindowPtr = arguments.mparentWindow.Get();

        if (m_bColorPickerIsInlineVersion)
        {
            generateInlineColorPickerContent();
        }
        else
        {
            generateDefaultColorPickerContent();
        }
    }
   
	void ColorPicker::generateInlineColorPickerContent()
	{
        std::shared_ptr<Widget> alphaSlider = NullWidget::getNullWidget();
        if (m_bUseAlpha.Get())
        {
            alphaSlider = makeColorSlider(ColorPickerChannels::Alpha);
        }

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = WIDGET_NEW(HorizontalBox)
        + HorizontalBox::Slot()
        .StretchWidth(1.0f)
        .setHorizontalAlignment(HorizontalAlignment::Center)
        (
            WIDGET_NEW(ColorWheel)
            .SelectedColor(this, &ColorPicker::getCurrentColor)
            .OnValueChanged(this, &ColorPicker::handleColorSpectrumValueChanged)
        )
        + HorizontalBox::Slot()
        .FixedWidth()
        .setPadding(Padding(4.0f, 0.0f))
         (
             makeColorSlider(ColorPickerChannels::Saturation) //饱和度
         )
         + HorizontalBox::Slot()
         .FixedWidth()
         .setPadding(Padding(4.0f, 0.0f))
         (
             makeColorSlider(ColorPickerChannels::Value) //明度
         )
         + HorizontalBox::Slot()
         .FixedWidth()
         .setPadding(Padding(4.0f, 0.0f))
         (
             alphaSlider //透明度
         );

		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	void ColorPicker::generateDefaultColorPickerContent()
	{
        std::shared_ptr<Widget> alphaSlider = NullWidget::getNullWidget();
        if (m_bUseAlpha.Get())
        {
            alphaSlider = makeColorSlider(ColorPickerChannels::Alpha);
        }

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = 
		WIDGET_NEW(Border)
		.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("grayColor"))
		.Content
		(
            WIDGET_NEW(VerticalBox)
            + VerticalBox::Slot()
            .FixedHeight()
		    (
			    WIDGET_NEW(HorizontalBox)
			    + HorizontalBox::Slot()
			    .StretchWidth(1.0f)
			    .setHorizontalAlignment(HorizontalAlignment::Center)
			    (
				    WIDGET_NEW(ColorWheel)
				    .SelectedColor(this, &ColorPicker::getCurrentColor)
				    .OnValueChanged(this, &ColorPicker::handleColorSpectrumValueChanged)
			    )
			    + HorizontalBox::Slot()
			    .FixedWidth()
			    .setPadding(Padding(4.0f, 0.0f))
			    (
				    makeColorSlider(ColorPickerChannels::Saturation) //饱和度
			    )
			    + HorizontalBox::Slot()
			    .FixedWidth()
			    .setPadding(Padding(4.0f, 0.0f))
			    (
				    makeColorSlider(ColorPickerChannels::Value) //明度
			    )
				+ HorizontalBox::Slot()
				.FixedWidth()
				.setPadding(Padding(4.0f, 0.0f))
				(
				    alphaSlider //透明度
				)
		    )
            + VerticalBox::Slot()
            .FixedHeight()
            (
			    WIDGET_NEW(HorizontalBox)
			    + HorizontalBox::Slot()
                .StretchWidth(1.0f)
                (
                    WIDGET_NEW(Spacer)
                )
                + HorizontalBox::Slot()
                .FixedWidth()
                (
				    WIDGET_NEW(Button)
				    .buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
                    .Clicked(this, &ColorPicker::handleOkButtonClicked)
				    .Content
				    (
					    WIDGET_NEW(TextBlockWidget)
					    .text(u8"OK")
					    .textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
				    )
                )
            )
        );

		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	std::shared_ptr<Widget> ColorPicker::makeColorSlider(ColorPickerChannels channel) const
	{
        return WIDGET_NEW(Overlay)
               + Overlay::Slot()
               .setPadding(Padding(4.0f, 0.0f))
               (
				   WIDGET_NEW(SimpleGradient)
				   .endColor(this, &ColorPicker::handleColorSliderEndColor, channel)
                   .startColor(this, &ColorPicker::handleColorSliderStartColor, channel)
				   .orientation(Orientation::Horizontal)
               )
               + Overlay::Slot()
               (
                   WIDGET_NEW(Slider)
                   .sliderBarColor(Color(0.0f, 0.0f, 0.0f, 0.0f)) //transparent
                   .orientation(Orientation::Vertical)
                   .visibility(Visibility::Visible)
                   .Value(this, &ColorPicker::handleColorSpinBoxValue, channel)
                   .OnValueChanged(const_cast<ColorPicker*>(this), &ColorPicker::handleColorSpinBoxValueChanged, channel)
               );
	}

	void ColorPicker::setColors(const Color& inColor)
	{
        for (int32_t i = 0; i < m_targetFColors.size(); ++i)
            *m_targetFColors[i] = inColor;

		for (int32_t i = 0; i < m_targetLinearColors.size(); ++i)
			*m_targetLinearColors[i] = inColor;

        for (int32_t i = 0; i < m_targetColorChannels.size(); ++i)
        {
            ColorChannels& channels = m_targetColorChannels[i];

            if (channels.red)
            {
                *channels.red = inColor.r;
            }
			if (channels.green)
			{
                *channels.green = inColor.g;
			}
			if (channels.blue)
			{
				*channels.blue = inColor.b;
			}
			if (channels.alpha)
			{
                *channels.alpha = inColor.a;
			}
        }
	}

	Color ColorPicker::getCurrentColor() const
	{
        return m_currentColorHSV;
	}

	bool openColorPicker(const ColorPickerArgs& args)
    {
		destroyColorPicker();
#ifdef WIN32
		bool result = false;

        Color oldColor = args.m_initialColorOverride;

        math::float2 cursorPos = Application::getApplication()->getCursorPos();
		math::box2 anchor;
		anchor.m_mins = math::float2(cursorPos.x, cursorPos.y);
		anchor.m_maxs = math::float2(cursorPos.x, cursorPos.y);
        math::float2 adjustedSummonLocation = Application::getApplication()->calculatePopupWindowPosition(anchor, ColorPicker::DEFAULT_WINDOW_SIZE, true, math::float2(0, 0), Orientation::Horizontal);

        std::shared_ptr<WindowWidget> window = nullptr;
        std::shared_ptr<Border> windowContent = WIDGET_NEW(Border);

        if (args.m_bOpenAsMenu)
        {
			window = Application::getApplication()->pushMenu(
				args.m_parentWidget,
				WidgetPath(),
				windowContent,
				adjustedSummonLocation,
				true,//todo:这个要改为false
				math::float2(0.0f, 0.0f),
				PopupMethod::CreateNewWindow,
				false)->getOwnedWindow();
        }
        else
        {
            window = WIDGET_NEW(WindowWidget)
                     .ScreenPosition(adjustedSummonLocation)
                     .sizingRule(SizingRule::AutoSized)
                     .Content
                     (
                         windowContent
                     );
        }


		std::shared_ptr<ColorPicker> createColorPicker = WIDGET_NEW(ColorPicker)
			.targetColorAttribute(oldColor)
			.targetFColors(args.m_colorArray ? *args.m_colorArray : std::vector<Color*>())
			.targetColorChannels(args.m_colorChannelsArray ? *args.m_colorChannelsArray : std::vector<ColorChannels*>())
			.useAlpha(args.m_bUseAlpha)
			.onColorCommitted(args.m_onColorCommitted)
			.preColorCommitted(args.m_preColorCommitted)
			.parentWindow(window);
		    //.displayInlineVersion(true);

		windowContent->setContent(createColorPicker);

		if (args.m_parentWidget)
		{
			WidgetPath widgetPath;
            Application::getApplication()->generatePathToWidgetUnchecked(args.m_parentWidget, widgetPath);
            window = Application::getApplication()->addWindowAsNativeChild(window, widgetPath.getWindow());
		}
        else
        {
			Application::getApplication()->makeWindow(window);
            Application::getApplication()->showWindow(window);
        }
		
#endif
        return false;
    }
    void destroyColorPicker()
    {
       
    }
}