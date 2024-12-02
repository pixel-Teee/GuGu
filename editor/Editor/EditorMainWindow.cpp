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
#include <Core/UI/Viewport.h>
#include <Core/UI/MenuAnchor.h>
#include <Core/UI/Box.h>
#include <Core/UI/TileView.h>

#include <Application/Application.h>//用于退出

#ifdef WIN32
#include <Application/Platform/Windows/WindowsMisc.h>
#else
#ifdef ANDROID
#include <Application/Platform/Android/AndroidMisc.h>
#endif
#endif

namespace GuGu {
	EditorMainWindow::EditorMainWindow()
	{
	}
	EditorMainWindow::~EditorMainWindow()
	{
		//EditorStyleSet::getStyleSet()->clear();
	}
	void EditorMainWindow::init(const BuilderArguments& arguments)
	{	
		if (true) //todo:修复这个
		{
			//blue gradient background
			std::vector<math::float4> blueGradientBackground;
			blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel1"));
			blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel2"));

			std::shared_ptr<Button> closeButton;
			//std::shared_ptr<EditorMainWindow> editorMainWindow;
			std::shared_ptr<Button> minimizeButton;
			//std::shared_ptr<ViewportWidget> viewportWidget;
			std::shared_ptr<Button> fileButton;

			WindowWidget::init(
			WindowWidget::BuilderArguments()
			.Content
			(
				WIDGET_NEW(Border)
				.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("grayColor"))
				.Content
				(
					WIDGET_NEW(VerticalBox) //菜单栏1
					+ VerticalBox::Slot()
					.StretchHeight(0.06)
					.setPadding(Padding(21.0f, 17.0f, 21.0f, 14.0f))
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
					+ VerticalBox::Slot()
					.FixedHeight()
					.setPadding(Padding(21.0f, 0.0f, 21.0f, 11.0f))
					(
						WIDGET_NEW(HorizontalBox)
						+ HorizontalBox::Slot()
						.FixedWidth()
						(
							WIDGET_NEW(VerticalBox)
							+ VerticalBox::Slot()
							.FixedHeight()
							(
								WIDGET_NEW(BoxWidget)
								.HeightOverride(OptionalSize(27.0f))
								.WidthOverride(OptionalSize(64.0f))
								.Content
								(
									WIDGET_ASSIGN_NEW(Button, fileButton)
									.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
									.Content
									(
										WIDGET_NEW(TextBlockWidget)
										.text(u8"File")
										.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
									)
								)
							)
							+ VerticalBox::Slot()
							.FixedHeight()
							(
								WIDGET_ASSIGN_NEW(MenuAnchor, m_openFileMenuAnchor)
								.useApplicationMenuStack(true)
								.method(PopupMethod::CreateNewWindow)
								.Content
								(
									NullWidget::getNullWidget()
								)
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
								WIDGET_NEW(Border)
								.padding(Padding(20.0f, 52.0f, 20.0f, 20.0f))
								.BorderBackgroundColor(math::float4(0.0f, 0.0f, 0.0f, 0.0f))
								.Content
								(
									WIDGET_ASSIGN_NEW(ViewportWidget, m_viewportWidget)
									.Content
									(
										NullWidget::getNullWidget()
									)
								)										
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
			.ScreenPosition(arguments.mScreenPosition));

			closeButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::exitApplication, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
			minimizeButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::miniMizeWindow, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
			fileButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openFileMenu, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
		}
		else
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

		//std::shared_ptr<TileView<GuGuUtf8Str>> test = WIDGET_NEW(TileView<GuGuUtf8Str>);
	}
	Reply EditorMainWindow::exitApplication()
	{
		//Application::getApplication()->setExit(true);
		this->requestDestroyWindow();
		return Reply::Handled();
	}
	Reply EditorMainWindow::miniMizeWindow()
	{
		Application::getApplication()->miniMizeWindow(std::static_pointer_cast<WindowWidget>(shared_from_this()));
		return Reply::Handled();
	}
	Reply EditorMainWindow::openFileMenu()
	{
		//return Reply();
		//open menu achor
		m_openFileMenuAnchor->setMenuContent(
			WIDGET_NEW(Border)
			.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige4"))
			.Content
			(
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_ASSIGN_NEW(Button, m_saveLevelButton)
					.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
					.Content
					(
						WIDGET_NEW(TextBlockWidget)
						.text(u8"save level")
						.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
					)
				)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_NEW(Button)
					.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
					.Content
					(
						WIDGET_NEW(TextBlockWidget)
						.text(u8"open level")
						.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
					)
				)
				//NullWidget::getNullWidget()
			)
		);
		m_saveLevelButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::saveLevel, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
		m_openFileMenuAnchor->setIsOpen(true);
		return Reply::Handled();
	}
	Reply EditorMainWindow::saveLevel()
	{
		GuGuUtf8Str fileName;
		GuGuUtf8Str filePath;
		PlatformMisc::getSaveFilePathAndFileName(std::static_pointer_cast<WindowWidget>(shared_from_this()), filePath, fileName);

		GuGuUtf8Str executableFilePath = Application::GetExecutableFilePath();
		return Reply();
	}
	void EditorMainWindow::setRenderTarget(nvrhi::TextureHandle renderTarget)
	{
		m_viewportWidget->setRenderTarget(renderTarget);
	}
	std::shared_ptr<EditorMainWindow> CreateEditorMainWindow()
	{
		//register style set
		std::shared_ptr<StyleSet> editorStyleSet = std::make_shared<EditorStyleSet>();
		StyleSetCenter::RegisterStyleSet("EditorStyleSet", editorStyleSet);

		std::shared_ptr<EditorMainWindow> editorMainWindow = std::make_shared<EditorMainWindow>();

		//1.先创建 window widget
		WIDGET_ASSIGN_NEW(EditorMainWindow, editorMainWindow).ScreenPosition(math::float2(200.0f, 200.0f));

		return editorMainWindow;
	}
}