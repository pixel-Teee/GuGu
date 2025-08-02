#include <pch.h>

#include "EditorMainWindow.h"
#include "LevelViewport.h"

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
#include <Core/UI/Splitter.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/CheckBox.h>

#include <Application/Application.h>//用于退出

#ifdef WIN32
#include <Application/Platform/Windows/WindowsMisc.h>
#else
#ifdef ANDROID
#include <Application/Platform/Android/AndroidMisc.h>
#endif
#endif

#include "ContentBrowser/ContentBrowser.h"
#include "ContentBrowser/PathView.h"
#include "WindowTitleBar.h"
#include "SceneOutliner/SceneOutliner.h"
#include "PropertyEditor/ObjectDetails.h"
#include <Editor/PropertyEditor/PropertyEditorManager.h>
#include <Editor/Transaction/TransactionManager.h>

#include <Core/GamePlay/World.h>

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

			std::shared_ptr<Button> undoButton;
			std::shared_ptr<Button> redoButton;

			WindowWidget::init(
			WindowWidget::BuilderArguments()
			.Content
			(
				WIDGET_NEW(Border)
				.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("grayColor"))
				.Content
				(
					WIDGET_NEW(VerticalBox) //标题栏
					+ VerticalBox::Slot()
					.StretchHeight(0.08)
					.setPadding(Padding(10.0f, 8.0f, 10.0f, 5.0f))
					(
						WIDGET_NEW(WindowTitleBar, std::static_pointer_cast<WindowWidget>(shared_from_this()))
					)
					+ VerticalBox::Slot()
					//.setHorizontalAlignment(HorizontalAlignment::Stretch)
					.FixedHeight()
					.setPadding(Padding(10.0f, 0.0f, 10.0f, 5.0f))
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
								.HeightOverride(OptionalSize(38.0f))
								.WidthOverride(OptionalSize(40.0f))
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
						+ HorizontalBox::Slot()
						.StretchWidth(1.0)
						(
							NullWidget::getNullWidget()
						)
						+ HorizontalBox::Slot()
						.FixedWidth()
						.setPadding(Padding(5.0f, 0.0f, 0.0f, 0.0f))
						.setHorizontalAlignment(HorizontalAlignment::Center)
						(
							WIDGET_NEW(VerticalBox)
							+ VerticalBox::Slot()
							.FixedHeight()
							(
								WIDGET_NEW(BoxWidget)
								.HeightOverride(OptionalSize(38.0f))
								.WidthOverride(OptionalSize(60.0f))
								.Content
								(
									//WIDGET_ASSIGN_NEW(Border, m_undoBorder)
									//.BorderBackgroundColor(Attribute<math::float4>(this, &EditorMainWindow::getUndoColor))
									//.Content
									//(
									WIDGET_ASSIGN_NEW(Button, undoButton)
									.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"undoButton"))
									.Content
									(
										NullWidget::getNullWidget()
									)
									//)	
								)
							)
						)
						+ HorizontalBox::Slot()
						.FixedWidth()
						.setPadding(Padding(5.0f, 0.0f, 0.0f, 0.0f))
						.setHorizontalAlignment(HorizontalAlignment::Right)
						(
							WIDGET_NEW(VerticalBox)
							+ VerticalBox::Slot()
							.FixedHeight()
							(
								WIDGET_NEW(BoxWidget)
								.HeightOverride(OptionalSize(38.0f))
								.WidthOverride(OptionalSize(60.0f))
								.Content
								(
									//WIDGET_ASSIGN_NEW(Border, m_undoBorder)
									//.BorderBackgroundColor(Attribute<math::float4>(this, &EditorMainWindow::getRedoColor))
									//.Content
									//(
									WIDGET_ASSIGN_NEW(Button, redoButton)
									.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"redoButton"))
									.Content
									(
										NullWidget::getNullWidget()
									)
									//)							
								)
							)
						)
					)
					+ VerticalBox::Slot() //中间区域和下面区域
					.StretchHeight(0.83)
					.setPadding(Padding(10.0f, 0.0f, 10.0f, 10.0f))
					(
						WIDGET_NEW(Splitter)
						.orientation(Orientation::Vertical)
						+ Splitter::Slot()
						.value(0.74)
						.sizeRule(Splitter::SizeRule::FractionOfParent)
						(
							WIDGET_NEW(Splitter)
							+ Splitter::Slot()
							.value(0.13f)
							.sizeRule(Splitter::SizeRule::FractionOfParent)
							.setPadding(Padding(0.0f, 0.0f, 0.0f, 0.0f))
							(
								WIDGET_NEW(ComplexGradient)
								.GradientColors(blueGradientBackground)
								.cornerRadius(math::float4(5.0f, 5.0f, 5.0f, 5.0f))
								.Content
								(
// 									WIDGET_ASSIGN_NEW(Button, m_switchEditorAndRuntime)
// 									.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
// 									.Content
// 									(
// 										WIDGET_NEW(TextBlockWidget)
// 										.text(u8"Test")
// 										.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
// 									)
									WIDGET_ASSIGN_NEW(SceneOutlinerNameSpace::SceneOutliner, m_sceneOutliner, shared_from_this())
								)
							)
							+ Splitter::Slot()
							.value(0.70f)
							.sizeRule(Splitter::SizeRule::FractionOfParent)
							.setPadding(Padding(0.0f, 0.0f, 27.0f, 0.0f))
							(
								WIDGET_NEW(ComplexGradient)
								.GradientColors(blueGradientBackground)
								.cornerRadius(math::float4(5.0f, 5.0f, 5.0f, 5.0f))
								.Content
								(
									WIDGET_NEW(Border)
									.padding(Padding(10.0f, 10.0f, 10.0f, 10.0f))
									.BorderBackgroundColor(math::float4(0.0f, 0.0f, 0.0f, 0.0f))
									.Content
									(
										WIDGET_NEW(VerticalBox)
										+ VerticalBox::Slot()
										.setHorizontalAlignment(HorizontalAlignment::Center)
										.setVerticalAlignment(VerticalAlignment::Center)
										.setPadding(Padding(0, 0, 0, 0))
										.FixedHeight()
										(
											WIDGET_ASSIGN_NEW(CheckBox, m_switchEditorAndRuntime)
											.onCheckStateChanged(this, &EditorMainWindow::switchEditorAndRuntime)
											.checkBoxStyle(EditorStyleSet::getStyleSet()->getStyle<CheckBoxStyle>(u8"StartAndStop"))
											.visibility(Visibility::Visible)
											.Content
											(
												NullWidget::getNullWidget()
											)
										)
										+ VerticalBox::Slot()
										.StretchHeight(1.0f)
										(
											WIDGET_ASSIGN_NEW(LevelViewport, m_viewportWidget)
											.Content
											(
												NullWidget::getNullWidget()
											)
										)
									)										
								)
							)
							+ Splitter::Slot()
							.sizeRule(Splitter::SizeRule::FractionOfParent)
							.value(0.14f)
							(
								WIDGET_NEW(ComplexGradient)
								.GradientColors(blueGradientBackground)
								.cornerRadius(math::float4(5.0f, 5.0f, 5.0f, 5.0f))
								.Content
								(
									WIDGET_ASSIGN_NEW(ObjectDetails, m_objectDetails)
								)
							)
						)		
						+ Splitter::Slot()
						.value(0.24f)
						.sizeRule(Splitter::SizeRule::FractionOfParent)
						//.setPadding(Padding(0.0f, 0.0f, 0.0f, 0.0f))
						(
							WIDGET_NEW(ContentBrowser, std::static_pointer_cast<WindowWidget>(shared_from_this()))
						)
					)
				)
			)
			//.ClientSize(math::float2(m_deviceParams.backBufferWidth, m_deviceParams.backBufferHeight))
			.ClientSize(math::float2(1280.0f, 720.0f)) //todo:fix this
			.ScreenPosition(arguments.mScreenPosition));

			fileButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openFileMenu, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
			//m_switchEditorAndRuntime->setOnClicked(OnClicked(std::bind(&EditorMainWindow::switchEditorAndRuntime, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));

			undoButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::undo, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
			redoButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::redo, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
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
		std::function<void(const std::vector<GameObject*>&, bool)> gameObjectSelectionChangedEvent = std::bind(&EditorMainWindow::onItemSelect, this, std::placeholders::_1, std::placeholders::_2);
		std::shared_ptr<ViewportClient> viewportClient = m_viewportWidget->getViewportClient();
		viewportClient->setGameObjectSelectionChangedEvent(gameObjectSelectionChangedEvent);

		//PropertyEditorManager::getPropertyEditorManager()->addDetailsView(m_objectDetails);

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
		m_saveLevelButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openLevel, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));

		m_openFileMenuAnchor->setIsOpen(true);

		m_saveLevelButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openLevel, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
		return Reply::Handled();
	}
	GuGu::Reply EditorMainWindow::openLevel()
{
		GuGuUtf8Str fileName;
		GuGuUtf8Str filePath;
		GuGuUtf8Str initDir;
		std::vector<GuGuUtf8Str> filterStr;
		PlatformMisc::getOpenFilePathAndFileName(std::static_pointer_cast<WindowWidget>(shared_from_this()), initDir, filePath, fileName, filterStr);

		GuGuUtf8Str executableFilePath = Application::GetExecutableFilePath();
		return Reply();
	}

	Reply EditorMainWindow::undo()
	{
		TransactionManager& transactionManager = TransactionManager::getTransactionManager();
		if (transactionManager.canUndo())
		{
			transactionManager.undo();
			return Reply::Handled();
		}
		return Reply::Unhandled();
	}

	Reply EditorMainWindow::redo()
	{
		TransactionManager& transactionManager = TransactionManager::getTransactionManager();
		if (transactionManager.canRedo())
		{
			transactionManager.redo();
			return Reply::Handled();
		}
		return Reply::Unhandled();
	}

	void EditorMainWindow::switchEditorAndRuntime(CheckBoxState inCheckBoxState)
	{
		std::shared_ptr<ViewportClient> viewportClient = m_viewportWidget->getViewportClient();
		ViewportClient::ViewportState state = viewportClient->getViewportState();
		if (inCheckBoxState == CheckBoxState::Unchecked)
		{
			viewportClient->setViewportState(ViewportClient::ViewportState::Editor);
		}
		else
		{
			viewportClient->setViewportState(ViewportClient::ViewportState::Runtime);
		}
		//return Reply::Handled();
	}

	void EditorMainWindow::setRenderTarget(nvrhi::TextureHandle renderTarget)
	{
		m_viewportWidget->setRenderTarget(renderTarget);
	}

	void EditorMainWindow::onItemSelect(const std::vector<GameObject*>& inObjects, bool bForceRefresh)
	{
		m_objectDetails->setObjects(inObjects, bForceRefresh);

		//select item
		std::shared_ptr<ViewportClient> viewportClient = m_viewportWidget->getViewportClient();
		if (inObjects.size() > 0)
		{
			std::shared_ptr<GameObject> gameObject = std::static_pointer_cast<GameObject>(inObjects[0]->shared_from_this());
			viewportClient->setSelectItem(gameObject);
		}
	}

	math::float4 EditorMainWindow::getUndoColor() const
	{
		TransactionManager& transactionManager = TransactionManager::getTransactionManager();
		if (transactionManager.canUndo())
		{
			return math::float4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return math::float4(0.3f, 0.3f, 0.3f, 1.0f);
	}

	math::float4 EditorMainWindow::getRedoColor() const
	{
		TransactionManager& transactionManager = TransactionManager::getTransactionManager();
		if (transactionManager.canRedo())
		{
			return math::float4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return math::float4(0.3f, 0.3f, 0.3f, 1.0f);
	}

	std::shared_ptr<EditorMainWindow> CreateEditorMainWindow()
	{
		//register style set
		std::shared_ptr<StyleSet> editorStyleSet = std::make_shared<EditorStyleSet>();
		StyleSetCenter::RegisterStyleSet("EditorStyleSet", editorStyleSet);

		std::shared_ptr<EditorMainWindow> editorMainWindow;

		//1.先创建 window widget
		WIDGET_ASSIGN_NEW(EditorMainWindow, editorMainWindow).ScreenPosition(math::float2(200.0f, 200.0f));

		return editorMainWindow;
	}
}