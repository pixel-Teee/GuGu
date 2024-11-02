#include <pch.h>

#include "EditorMainWindow.h"

#include <Core/UI/Button.h>
#include <Core/UI/NullWidget.h>

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
		//std::shared_ptr<StyleSet> editorStyleSet = std::make_shared<editorStyleSet>();

		//1.先创建 window widget
		return WIDGET_NEW(EditorMainWindow)
			.Content
			(
				NullWidget::getNullWidget()

			)
			//.ClientSize(math::float2(m_deviceParams.backBufferWidth, m_deviceParams.backBufferHeight))
			.ClientSize(math::float2(1280.0f, 720.0f))
			.ScreenPosition(math::float2(0.0f, 0.0f));
	}
}