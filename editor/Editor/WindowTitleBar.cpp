#include <pch.h>

#include "WindowTitleBar.h"
#include <Core/UI/Overlay.h>
#include <Core/UI/ComplexGradient.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/Button.h>
#include <Core/UI/WindowWidget.h>
#include <Editor/StyleSet/EditorStyleSet.h>

#include <Application/Application.h>

namespace GuGu {
	WindowTitleBar::WindowTitleBar()
	{
	}
	WindowTitleBar::~WindowTitleBar()
	{
	}
	void WindowTitleBar::init(const BuilderArguments& arguments, const std::shared_ptr<WindowWidget> inParentWindow)
	{
		m_parentWindow = inParentWindow;

		//blue gradient background
		std::vector<math::float4> blueGradientBackground;
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel1"));
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel2"));


		std::shared_ptr<Button> closeButton;
		//std::shared_ptr<EditorMainWindow> editorMainWindow;
		std::shared_ptr<Button> minimizeButton;
		//std::shared_ptr<ViewportWidget> viewportWidget;
		std::shared_ptr<Button> fileButton;

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		std::shared_ptr<Overlay> overlay = WIDGET_NEW(Overlay)
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
		);

		m_childWidget->m_childWidget = overlay;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		closeButton->setOnClicked(OnClicked(std::bind(&WindowTitleBar::exitApplication, std::static_pointer_cast<WindowTitleBar>(shared_from_this()))));
		minimizeButton->setOnClicked(OnClicked(std::bind(&WindowTitleBar::miniMizeWindow, std::static_pointer_cast<WindowTitleBar>(shared_from_this()))));

		m_visibilityAttribute = Visibility::Visible;
	}

	Reply WindowTitleBar::exitApplication()
	{
		//Application::getApplication()->setExit(true);
		std::shared_ptr<WindowWidget> parentWindow = m_parentWindow.lock();
		parentWindow->requestDestroyWindow();
		return Reply::Handled();
	}
	Reply WindowTitleBar::miniMizeWindow()
	{
		Application::getApplication()->miniMizeWindow(std::static_pointer_cast<WindowWidget>(shared_from_this()));
		return Reply::Handled();
	}
	WindowZone::Type WindowTitleBar::getWindowZoneOverride() const
	{
		return WindowZone::Type::TitleBar;
	}
}