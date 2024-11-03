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

#include <Application/Application.h>//用于退出

namespace GuGu {
	EditorMainWindow::EditorMainWindow()
	{
	}
	EditorMainWindow::~EditorMainWindow()
	{
		EditorStyleSet::getStyleSet()->clear();
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
	Reply EditorMainWindow::exitApplication()
	{
		//Application::getApplication()->setExit(true);
		return Reply::Handled();
	}
	Reply EditorMainWindow::miniMizeWindow()
	{
		Application::getApplication()->miniMizeWindow(std::static_pointer_cast<WindowWidget>(shared_from_this()));
		return Reply::Handled();
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

		std::shared_ptr<Button> closeButton;
		std::shared_ptr<EditorMainWindow> editorMainWindow;
		std::shared_ptr<Button> minimizeButton;
		//1.先创建 window widget
		WIDGET_ASSIGN_NEW(EditorMainWindow, editorMainWindow)
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
						.cornerRadius(math::float4(10.0f, 10.0f, 10.0f, 10.0f))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)	
					+ Overlay::Slot()
					.setHorizontalAlignment(HorizontalAlignment::Right)
					.setVerticalAlignment(VerticalAlignment::Center)
					.setPadding(Padding(0.0f, 0.0f, 67.0f, 0.0f))
					(
						WIDGET_ASSIGN_NEW(Button, minimizeButton)
						.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"MinimizeButton"))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
					+ Overlay::Slot()
					.setHorizontalAlignment(HorizontalAlignment::Right)
					.setVerticalAlignment(VerticalAlignment::Center)
					.setPadding(Padding(0.0f, 0.0f, 7.0f, 0.0f))
					(
						WIDGET_ASSIGN_NEW(Button, closeButton)
						.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"CloseButton"))
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
					WIDGET_NEW(HorizontalBox)
					+ HorizontalBox::Slot()
					.StretchWidth(0.16f)
					.setPadding(Padding(0.0f, 0.0f, 24.0f, 0.0f))
					(
						WIDGET_NEW(ComplexGradient)
						.GradientColors(blueGradientBackground)
						.cornerRadius(math::float4(10.0f, 10.0f, 10.0f, 10.0f))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
					+ HorizontalBox::Slot()
					.StretchWidth(0.60f)
					.setPadding(Padding(0.0f, 0.0f, 27.0f, 0.0f))
					(
						WIDGET_NEW(ComplexGradient)
						.GradientColors(blueGradientBackground)
						.cornerRadius(math::float4(10.0f, 10.0f, 10.0f, 10.0f))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
					+ HorizontalBox::Slot()
					.StretchWidth(0.16f)
					(
						WIDGET_NEW(ComplexGradient)
						.GradientColors(blueGradientBackground)
						.cornerRadius(math::float4(10.0f, 10.0f, 10.0f, 10.0f))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
				)
				+ VerticalBox::Slot() //下面区域
				.StretchHeight(0.175)
				.setPadding(Padding(21.0f, 0.0f, 19.0f, 18.0f))
				(
					WIDGET_NEW(HorizontalBox)
					+ HorizontalBox::Slot()
					.StretchWidth(0.16f)
					.setPadding(Padding(0.0f, 0.0f, 25.0f, 0.0f))
					(
						WIDGET_NEW(ComplexGradient)
						.GradientColors(blueGradientBackground)
						.cornerRadius(math::float4(10.0f, 10.0f, 10.0f, 10.0f))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
					+ HorizontalBox::Slot()
					.StretchWidth(0.78f)
					.setPadding(Padding(0.0f, 0.0f, 0.0f, 0.0f))
					(
						WIDGET_NEW(ComplexGradient)
						.GradientColors(blueGradientBackground)
						.cornerRadius(math::float4(10.0f, 10.0f, 10.0f, 10.0f))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
				)
			)
		)
		//.ClientSize(math::float2(m_deviceParams.backBufferWidth, m_deviceParams.backBufferHeight))
		.ClientSize(math::float2(1280.0f, 720.0f)) //todo:fix this
		.ScreenPosition(math::float2(0.0f, 0.0f));

		closeButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::exitApplication, editorMainWindow)));
		minimizeButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::miniMizeWindow, editorMainWindow)));
		return editorMainWindow;
	}
}