#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "WidgetDelegates.h"

#include "WindowWidget.h"//window widget

namespace GuGu {
	using OnColorPickerCancelled = std::function<void(math::float4)>;//linear color

	//枚举颜色拾取器模式
	enum class ColorPickerModes
	{
		Spectrum,
		Wheel
	};

	//结构体去持有 float 值的独立指针
	struct ColorChannels
	{
		ColorChannels()
		{
			red = green = blue = alpha = nullptr;
		}

		float* red;
		float* green;
		float* blue;
		float* alpha;
	};

	class Border;
	class ComboButton;

	//颜色拾取器
	class ColorPicker : public Widget
	{
	public:
		struct BuilderArguments : public Arguments<ColorPicker>
		{
			BuilderArguments()
				: mtargetColorAttribute(math::float4(1.0f, 1.0f, 1.0f, 1.0f))
				, mtargetFColors()
				, mtargetLinearColors()
				, mtargetColorChannels()
				, museAlpha(true)
				, monlyRefreshOnMouseUp(false)
				, monlyRefreshOk(false)
				, monColorCommitted()
				, mpreColorCommitted()
				, monColorPickerCancelled()
				, monColorPickerWindowClosed()
				, monInteractivePickBegin()
				, monInteractivePickEnd()
				, mparentWindow()
				, mdisplayGamma(2.2f)
				, msRGBOverride()
				, mdisplayInlineVersion(false)
				, moverrideColorPickerCreation(false)
				, mexpandAdvancedSection(false)
				, moptionalOwningDetailsView(nullptr)
			{}

			~BuilderArguments() = default;

			//被作为 attribute 的目标的颜色
			ARGUMENT_ATTRIBUTE(math::float4, targetColorAttribute)

			//颜色数组的指针，颜色拾取器的目标
			ARGUMENT_ATTRIBUTE(std::vector<math::float4*>, targetFColors) //FColors

			//线性颜色数组的指针，颜色拾取器的目标
			ARGUMENT_ATTRIBUTE(std::vector<math::float4*>, targetLinearColors)

			//这个颜色拾取器目标的颜色指针的数组
			ARGUMENT_ATTRIBUTE(std::vector<ColorChannels>, targetColorChannels)

			//是否 alpha 值被开启
			ARGUMENT_ATTRIBUTE(bool, useAlpha)

			//是否阻止刷新，对于性能原因
			ARGUMENT_ATTRIBUTE(bool, onlyRefreshOnMouseUp)

			//阻止多次刷新，当请求的时候
			ARGUMENT_ATTRIBUTE(bool, onlyRefreshOk)

			//当颜色被提交的时候，被调用的事件
			UI_EVENT(OnLinearColorValueChanged, onColorCommitted)

			//在颜色被提交之前，被调用的事件
			UI_EVENT(OnLinearColorValueChanged, preColorCommitted)

			//当取消按钮被按下的时候，调用的回调
			UI_EVENT(OnColorPickerCancelled, onColorPickerCancelled)

			//当颜色拾取器的窗口被关闭的时候调用的回调
			UI_EVENT(OnWindowClosed, onColorPickerWindowClosed)

			//当 slider 拖动，颜色色环拖动或者滴管拖动开始的时候
			UI_EVENT(SimpleDelegate, onInteractivePickBegin)

			//当 slider 拖动，颜色色环拖动或者滴管拖动结束的时候
			UI_EVENT(SimpleDelegate, onInteractivePickEnd)

			//指向父窗口的指针
			ARGUMENT_ATTRIBUTE(std::shared_ptr<WindowWidget>, parentWindow)

			//设置显示的 gamma 设置
			ARGUMENT_ATTRIBUTE(float, displayGamma)

			//重载 sRGB 选项的 check box 值
			ARGUMENT_ATTRIBUTE(std::optional<bool>, sRGBOverride)

			//如果真的话，这个颜色拾取器是一个简化版本的
			ARGUMENT_ATTRIBUTE(bool, displayInlineVersion)

			//如果真的话，这个颜色拾取器将有一个非标准的创建行为
			ARGUMENT_ATTRIBUTE(bool, overrideColorPickerCreation)

			//如果真的话，advanced section 将会被展开
			ARGUMENT_ATTRIBUTE(bool, expandAdvancedSection)

			//允许一个细节 view 去持有颜色拾取器，那么刷新另外的 details view 将不会关闭它
			ARGUMENT_ATTRIBUTE(std::shared_ptr<Widget>, optionalOwningDetailsView)
		};

		//一个默认的窗口大小，对于颜色拾取器，使其看起来不错
		static const math::float2 DEFAULT_WINDOW_SIZE;

		ColorPicker();

		virtual ~ColorPicker();

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;
	private:

		Attribute<math::float4> m_targetColorAttribute;//linear color

		//现在被拾取的颜色，在 hsv 中
		math::float4 m_currentColorHSV;

		//现在被拾取的颜色，在 rgb 中
		math::float4 m_currentColorRGB;

		//在 hsv 下修改的老的颜色
		math::float4 m_oldColor;

		//颜色终止动画的点
		math::float4 m_colorEnd;

		//颜色开始动画的点
		math::float4 m_colorBegin;

		//持有颜色拾取器的模式
		ColorPickerModes m_currentMode;

		//时间，用于颜色动画
		float m_currentTime;

		//最大允许的更新时间，在我们关闭自动更新之前
		static const double MAX_ALLOWED_UPDATE_TIME;

		//如果为真，那么性能太坏导致自动更新
		bool m_bPerfIsTooSlowToUpdate;

		//这个颜色拾取器目标的颜色指针的数组
		std::vector<math::float4*> m_targetFColors;//FColor

		std::vector<math::float4*> m_targetLinearColors;//linear colors

		std::vector<ColorChannels> m_targetColorChannels;

		std::vector<math::float4> m_oldTargetFColors;

		std::vector<math::float4> m_oldTargetLinearColors;

		std::vector<math::float4> m_oldTargetColorChannels;

		Attribute<bool> m_bUseAlpha;

		bool m_bOnlyRefreshOnMouseUp;

		bool m_bOnlyRefreshOnOK;

		bool m_bClosedViaOrCancel;

		std::weak_ptr<WindowWidget> m_parentWindowPtr;

		//todo:add theme color blocks bar

		//去展示颜色主题 viewer 的按钮，或者一个颜色垃圾桶
		std::shared_ptr<Border> m_colorThemeButtonOrSmallTrash;

		//显示颜色主题 viewer 的按钮
		std::shared_ptr<ComboButton> m_colorThemeComboButton;

		//放置在 combo button 上的小 color trash
		std::shared_ptr<Widget> m_smallTrash;

		Attribute<float> m_displayGamma;

		std::optional<bool> m_originalSRGBOption;

		//inline color picker
		bool m_bColorPickerIsInlineVersion;

		bool m_bColorPickerCreationIsOverridden;

		//是否用户移动一个值 spin box ，色环还有滴管
		bool m_bIsInteractive;

	private:
		OnLinearColorValueChanged m_onColorCommitted;

		OnLinearColorValueChanged m_preColorCommitted;

		OnColorPickerCancelled m_onColorPickerCancelled;

		SimpleDelegate m_onInteractivePickBegin;

		SimpleDelegate m_onInteractivePickEnd;

		OnWindowClosed m_onColorPickerWindowClosed;

		std::weak_ptr<Widget> m_optionalOwningDetailsView;

	private:
		//todo:add color themes viewer
	};

	struct ColorPickerArgs
	{
		//新的颜色拾取器是否是模态对话框
		bool m_bIsModal;

		std::shared_ptr<Widget> m_parentWidget;

		bool m_bUseAlpha;

		bool m_bOnlyRefreshOnMouseUp;

		bool m_bOnlyRefreshOnOk;

		bool m_bExpandAdvancedSection;

		bool m_bOpenAsMenu;

		Attribute<float> m_displayGamma;

		std::optional<bool> m_bSRGBOverride;

		const std::vector<math::float4*>* m_colorArray;//FColor

		const std::vector<math::float4*>* m_linearColorArray;//linear color

		const std::vector<ColorChannels>* m_colorChannelsArray;

		OnLinearColorValueChanged m_onColorCommitted;

		OnLinearColorValueChanged m_preColorCommitted;

		OnWindowClosed m_onColorPickerWindowClosed;

		OnColorPickerCancelled m_onColorPickerCancelled;

		SimpleDelegate m_onInteractivePickBegin;

		SimpleDelegate m_onInteractivePickEnd;

		math::float4 m_initialColorOverride;//linear color

		std::shared_ptr<Widget> m_optionalOwningDetailsView;

		ColorPickerArgs()
			: m_bIsModal(false)
			, m_parentWidget(nullptr)
			, m_bUseAlpha(false)
			, m_bOnlyRefreshOnMouseUp(false)
			, m_bOnlyRefreshOnOk(false)
			, m_bExpandAdvancedSection(true)
			, m_bOpenAsMenu(false)
			, m_displayGamma(2.2f)
			, m_bSRGBOverride()
			, m_colorArray(nullptr)
			, m_linearColorArray(nullptr)
			, m_colorChannelsArray(nullptr)
			, m_onColorCommitted()
			, m_preColorCommitted()
			, m_onColorPickerWindowClosed()
			, m_onColorPickerCancelled()
			, m_onInteractivePickBegin()
			, m_onInteractivePickEnd()
			, m_initialColorOverride()
			, m_optionalOwningDetailsView(nullptr)
		{}
	};
}