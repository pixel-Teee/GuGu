#include <pch.h>

#include "EditorMainWindow.h"

#include <Core/UI/Button.h>
#include <Core/UI/NullWidget.h>

#include "StyleSet/EditorStyleSet.h"
#include <Core/UI/StyleSetCenter.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/ColorBlock.h>
#include <Core/UI/ComplexGradient.h>

namespace GuGu {
	EditorMainWindow::EditorMainWindow()
	{
	}
	EditorMainWindow::~EditorMainWindow()
	{
	}
	void EditorMainWindow::init(const BuilderArguments& arguments)
	{
		m_windowType = arguments.mType;
		m_childWidget = arguments.mContent;
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;

		m_screenPosition = arguments.mScreenPosition;
		//set size
		resize(arguments.mClientSize);

		m_sizingRule = arguments.msizingRule;
	}
	std::shared_ptr<EditorMainWindow> CreateEditorMainWindow()
	{
		//register style set
		std::shared_ptr<StyleSet> editorStyleSet = std::make_shared<EditorStyleSet>();
		StyleSetCenter::RegisterStyleSet("EditorStyleSet", editorStyleSet);

		//blue gradient background
		std::vector<math::float4> blueGradientBackground;
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel1"));
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel2"));

		//1.先创建 window widget
		return WIDGET_NEW(EditorMainWindow)
			.Content
			(
				WIDGET_NEW(Border)
				.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("grayColor"))
				.Content
				(
					WIDGET_NEW(VerticalBox) //菜单栏
					+ VerticalBox::Slot()
					.StretchHeight(0.06)
					.setPadding(Padding(21.0f, 17.0f, 21.0f, 50.0f))
					(
						WIDGET_NEW(Overlay)
						+ Overlay::Slot()
						(
							WIDGET_NEW(ComplexGradient)
							.GradientColors(blueGradientBackground)
							.Content
							(
								NullWidget::getNullWidget()
							)
						)
					)
					+ VerticalBox::Slot() //中间区域
					.StretchHeight(0.58)
					.setPadding(Padding(22.0f, 0.0f, 19.0f, 45.0f))
					(
						WIDGET_NEW(ComplexGradient)
						.GradientColors(blueGradientBackground)
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
					+ VerticalBox::Slot() //下面区域
					.StretchHeight(0.175)
					.setPadding(Padding(21.0f, 0.0f, 19.0f, 18.0f))
					(
						WIDGET_NEW(ComplexGradient)
						.GradientColors(blueGradientBackground)
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
				)
			)
			//.ClientSize(math::float2(m_deviceParams.backBufferWidth, m_deviceParams.backBufferHeight))
			.ClientSize(math::float2(1280.0f, 720.0f)) //todo:fix this
			.ScreenPosition(math::float2(0.0f, 0.0f));
	}
}