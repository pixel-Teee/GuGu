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

#include <Core/GamePlay/Prefab.h>

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

#include <Editor/StyleSet/ThemePanel.h> //theme panel
#include <Editor/Debug/ShowTexturePanel.h>
#include <Editor/Terrain/TerrainEditorPanel.h>

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
			std::vector<Attribute<math::float4>> blueGradientBackground;
			blueGradientBackground.push_back(Attribute<math::float4>::Create([=]() {
				return EditorStyleSet::getStyleSet()->getColor("LightColorLevel1");
			}));
			blueGradientBackground.push_back(Attribute<math::float4>::Create([=]() {
				return EditorStyleSet::getStyleSet()->getColor("LightColorLevel2");
			}));

			std::shared_ptr<Button> closeButton;
			//std::shared_ptr<EditorMainWindow> editorMainWindow;
			std::shared_ptr<Button> minimizeButton;
			//std::shared_ptr<ViewportWidget> viewportWidget;
			std::shared_ptr<Button> fileButton;

			std::shared_ptr<Button> themeButton;

			std::shared_ptr<Button> terrainButton;

			std::shared_ptr<Button> debugView;//debug view

			std::shared_ptr<Button> undoButton;
			std::shared_ptr<Button> redoButton;

			std::shared_ptr<Button> applyPrefab;
			std::shared_ptr<Button> revertToPrefab;

			WindowWidget::init(
			WindowWidget::BuilderArguments()
			.Content
			(
				WIDGET_NEW(Border)
				.BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("GrayColor");
				}))
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
						.FixedWidth()
						.setPadding(Padding(10.0f, 0.0f, 0.0f, 0.0f))
						(
							WIDGET_NEW(BoxWidget)
							.HeightOverride(OptionalSize(38.0f))
							.WidthOverride(OptionalSize(60.0f))
							.Content
							(
								WIDGET_ASSIGN_NEW(Button, themeButton)
								.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
								.Content
								(
									WIDGET_NEW(TextBlockWidget)
									.text(u8"Theme")
									.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
								)
							)
						)
						+ HorizontalBox::Slot()
						.FixedWidth()
						.setPadding(Padding(10.0f, 0.0f, 0.0f, 0.0f))
						(
							WIDGET_NEW(BoxWidget)
							.HeightOverride(OptionalSize(38.0f))
							.WidthOverride(OptionalSize(60.0f))
							.Content
							(
								WIDGET_ASSIGN_NEW(Button, terrainButton)
								.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
								.Content
								(
									WIDGET_NEW(TextBlockWidget)
									.text(u8"Terrain")
									.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
								)
							)
						)
						+ HorizontalBox::Slot()
						.FixedWidth()
						.setPadding(Padding(10.0f, 0.0f, 0.0f, 0.0f))
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
									WIDGET_ASSIGN_NEW(Button, debugView)
									.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
									.Content
									(
										WIDGET_NEW(TextBlockWidget)
										.text(u8"Debug")
										.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
									)
								)
							)
							+ VerticalBox::Slot()
							.FixedHeight()
							(
								WIDGET_ASSIGN_NEW(MenuAnchor, m_debugViewMenuAnchor)
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
									WIDGET_NEW(VerticalBox)
									+ VerticalBox::Slot()
									.FixedHeight()
									(
										//prefab edit
										WIDGET_NEW(HorizontalBox)
										.Clip(WidgetClipping::ClipToBounds)
										.visibility(Attribute<Visibility>::CreateSP(this, &EditorMainWindow::getGameObjectPrefabEditVisiblity))
										+ HorizontalBox::Slot()
										.FixedWidth()
										(
											//apply prefab
											WIDGET_ASSIGN_NEW(Button, applyPrefab)
											.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
											.Content
											(
												WIDGET_NEW(TextBlockWidget)
												.text(u8"applyPrefab")
												.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
											)
										)
										+ HorizontalBox::Slot()
										.setPadding(Padding(20.0f, 0.0f, 0.0f, 0.0f)) //left padding
										.FixedWidth()
										(
											//revert prefab
											WIDGET_ASSIGN_NEW(Button, revertToPrefab)
											.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
											.Content
											(
												WIDGET_NEW(TextBlockWidget)
												.text(u8"revertToPrefab")
												.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
											)
										)
									)
									+ VerticalBox::Slot()
									.StretchHeight(1.0f)
									(
										WIDGET_ASSIGN_NEW(ObjectDetails, m_objectDetails)
									)
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

			debugView->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openDebugViewMenu, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
			//m_switchEditorAndRuntime->setOnClicked(OnClicked(std::bind(&EditorMainWindow::switchEditorAndRuntime, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));

			undoButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::undo, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
			redoButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::redo, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));

			themeButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openTheme, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));

			terrainButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openTerrainEditorPanel, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));

			applyPrefab->setOnClicked(OnClicked(std::bind(&EditorMainWindow::applyPrefab, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
			revertToPrefab->setOnClicked(OnClicked(std::bind(&EditorMainWindow::revertToPrefab, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
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
		m_gameObjectSelectionChangedEventDelegateHandle = viewportClient->addGameObjectSelectionChangedEvent(gameObjectSelectionChangedEvent);

		//PropertyEditorManager::getPropertyEditorManager()->addDetailsView(m_objectDetails);

	}

	Reply EditorMainWindow::openFileMenu()
	{
		//return Reply();
		//open menu achor
		m_openFileMenuAnchor->setMenuContent(
			WIDGET_NEW(Border)
			.BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
				return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel4");
			}))
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

		//m_saveLevelButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openLevel, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
		return Reply::Handled();
	}

	GuGu::Reply EditorMainWindow::openDebugViewMenu()
	{
		m_debugViewMenuAnchor->setMenuContent(
			WIDGET_NEW(Border)
			.BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
				return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel4");
			}))
			.Content
			(
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_ASSIGN_NEW(Button, m_openFontAtlasButton)
					.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
					.Content
					(
						WIDGET_NEW(TextBlockWidget)
						.text(u8"Show Font Atlas")
						.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
					)
				)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_NEW(HorizontalBox)
					+ HorizontalBox::Slot()
					.FixedWidth()
					(
						WIDGET_ASSIGN_NEW(CheckBox, m_terrainUseWireFrameCheckBox)
						.checkBoxStyle(EditorStyleSet::getStyleSet()->getStyle<CheckBoxStyle>("EditorNormalCheckBox"))
						.Content
						(
							WIDGET_NEW(NullWidget)
						)
						.onCheckStateChanged(this, &EditorMainWindow::switchTerrainWireFrame)
					)
					+ HorizontalBox::Slot()
					.FixedWidth()
					(
						WIDGET_NEW(TextBlockWidget)
						.text(u8"Terrain WireFrame")
						.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
					)
				)
			)
		);

		m_openFontAtlasButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openFontAtlas, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));

		m_debugViewMenuAnchor->setIsOpen(true);

		std::shared_ptr<ViewportClient> viewportClient = m_viewportWidget->getViewportClient();
		m_terrainUseWireFrameCheckBox->setIsChecked(viewportClient->getTerrainUseWireFrame());

		//m_saveLevelButton->setOnClicked(OnClicked(std::bind(&EditorMainWindow::openLevel, std::static_pointer_cast<EditorMainWindow>(shared_from_this()))));
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

	Reply EditorMainWindow::openTheme()
	{
		math::float2 cursorPos = Application::getApplication()->getCursorPos();
		std::shared_ptr<WindowWidget> window = WIDGET_NEW(WindowWidget)
			.ScreenPosition(cursorPos)
			.sizingRule(SizingRule::AutoSized);
			//.Content
			//(
			//	WIDGET_NEW(VerticalBox)
			//	+ VerticalBox::Slot()
			//	.FixedHeight()
			//	(
			//		WIDGET_NEW(WindowTitleBar)
			//		)
			//	+ VerticalBox::Slot()
			//	.FixedHeight()
			//	(
			//		WIDGET_NEW(ThemePanel)
			//		)
			//);

		window->setContent(
			WIDGET_NEW(VerticalBox)
			+ VerticalBox::Slot()
			.FixedHeight()
			(
				WIDGET_NEW(WindowTitleBar, window)
			)
			+ VerticalBox::Slot()
			.FixedHeight()
			(
				WIDGET_NEW(ThemePanel)
			)
		);

		WidgetPath widgetPath;
		Application::getApplication()->generatePathToWidgetUnchecked(shared_from_this(), widgetPath);
		Application::getApplication()->addWindowAsNativeChild(window, widgetPath.getWindow());

		return Reply::Handled();
	}

	GuGu::Reply EditorMainWindow::openTerrainEditorPanel()
	{
		math::float2 cursorPos = Application::getApplication()->getCursorPos();
		std::shared_ptr<WindowWidget> window = WIDGET_NEW(WindowWidget)
			.ScreenPosition(cursorPos)
			.sizingRule(SizingRule::AutoSized);
		//.Content
		//(
		//	WIDGET_NEW(VerticalBox)
		//	+ VerticalBox::Slot()
		//	.FixedHeight()
		//	(
		//		WIDGET_NEW(WindowTitleBar)
		//		)
		//	+ VerticalBox::Slot()
		//	.FixedHeight()
		//	(
		//		WIDGET_NEW(ThemePanel)
		//		)
		//);

		window->setContent(
			WIDGET_NEW(VerticalBox)
			+ VerticalBox::Slot()
			.FixedHeight()
			(
				WIDGET_NEW(WindowTitleBar, window)
			)
			+ VerticalBox::Slot()
			.FixedHeight()
			(
				WIDGET_NEW(TerrainEditorPanel)
			)
		);

		WidgetPath widgetPath;
		Application::getApplication()->generatePathToWidgetUnchecked(shared_from_this(), widgetPath);
		Application::getApplication()->addWindowAsNativeChild(window, widgetPath.getWindow());

		return Reply::Handled();
	}

	Reply EditorMainWindow::openFontAtlas()
	{
		//open editor font atlas
		math::float2 cursorPos = Application::getApplication()->getCursorPos();
		std::shared_ptr<WindowWidget> window = WIDGET_NEW(WindowWidget)
			.ScreenPosition(cursorPos)
			.sizingRule(SizingRule::AutoSized);
		//.Content
		//(
		//	WIDGET_NEW(VerticalBox)
		//	+ VerticalBox::Slot()
		//	.FixedHeight()
		//	(
		//		WIDGET_NEW(WindowTitleBar)
		//		)
		//	+ VerticalBox::Slot()
		//	.FixedHeight()
		//	(
		//		WIDGET_NEW(ThemePanel)
		//		)
		//);

		window->setContent(
			WIDGET_NEW(VerticalBox)
			+ VerticalBox::Slot()
			.FixedHeight()
			(
				WIDGET_NEW(WindowTitleBar, window)
			)
			+ VerticalBox::Slot()
			.FixedHeight()
			(
				WIDGET_NEW(ShowTexturePanel)//show texture panel
			)
		);

		WidgetPath widgetPath;
		Application::getApplication()->generatePathToWidgetUnchecked(shared_from_this(), widgetPath);
		Application::getApplication()->addWindowAsNativeChild(window, widgetPath.getWindow());

		return Reply::Handled();
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
		m_sceneOutliner->fullRefresh();
		//return Reply::Handled();
	}

	void EditorMainWindow::switchTerrainWireFrame(CheckBoxState inCheckBoxState)
	{
		std::shared_ptr<ViewportClient> viewportClient = m_viewportWidget->getViewportClient();
		ViewportClient::ViewportState state = viewportClient->getViewportState();
		if (inCheckBoxState == CheckBoxState::Unchecked)
		{
			viewportClient->setTerrainUseWireFrame(false);
		}
		else
		{
			viewportClient->setTerrainUseWireFrame(true);
		}
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

	Visibility EditorMainWindow::getGameObjectPrefabEditVisiblity() const
	{
		std::shared_ptr<ViewportClient> viewportClient = m_viewportWidget->getViewportClient();
		std::shared_ptr<GameObject> selectItem = viewportClient->getSelectedItems();
		if (selectItem)
		{
			if(selectItem->isPrefabChildren())
				return Visibility::Visible;
		}
		return Visibility::Collapsed;
	}

	Reply EditorMainWindow::applyPrefab()
	{
		//apply modify to prefab asset
		std::shared_ptr<ViewportClient> viewportClient = m_viewportWidget->getViewportClient();
		std::shared_ptr<GameObject> selectItem = viewportClient->getSelectedItems();
		if (selectItem)
		{
			if (selectItem->isPrefabChildren())
			{
				//get prefab asset
				std::shared_ptr<GameObject> prefabRoot = selectItem->getPrefabRootGameObject();
				if (prefabRoot)
				{
					GuGuUtf8Str prefabAssetGuidStr = prefabRoot->getPrefab();
					const AssetData& assetData = AssetManager::getAssetManager().getAssetData(prefabAssetGuidStr);
					if (prefabAssetGuidStr != "")
					{
						nlohmann::json gameObjectTreeJson = AssetManager::getAssetManager().serializeJson(prefabRoot);
						gameObjectTreeJson["Version"] = std::to_string(GuGu_Version);

						GuGuUtf8Str fileContent = gameObjectTreeJson.dump();

						GuGuUtf8Str noFileExtensionsFileName = assetData.m_fileName;
						int32_t dotPos = noFileExtensionsFileName.findLastOf(".");

						GuGuUtf8Str outputFilePath = assetData.m_filePath;

						prefabAssetGuidStr = AssetManager::getAssetManager().registerAsset(prefabAssetGuidStr, outputFilePath, noFileExtensionsFileName + ".json", meta::Type(meta::TypeIDs<Prefab>().ID));
						gameObjectTreeJson["GUID"] = prefabAssetGuidStr.getStr();
						//输出到目录
						AssetManager::getAssetManager().getRootFileSystem()->OpenFile(outputFilePath, GuGuFile::FileMode::OnlyWrite);
						AssetManager::getAssetManager().getRootFileSystem()->WriteFile((void*)fileContent.getStr(), fileContent.getTotalByteCount());
						AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
					}
				}	

				return Reply::Handled();
			}
			return Reply::Unhandled();
		}
		return Reply::Unhandled();
	}

	Reply EditorMainWindow::revertToPrefab()
	{
		std::shared_ptr<ViewportClient> viewportClient = m_viewportWidget->getViewportClient();
		std::shared_ptr<GameObject> selectItem = viewportClient->getSelectedItems();
		if (selectItem)
		{
			if (selectItem->isPrefabChildren())
			{
				
				return Reply::Handled();
			}
			return Reply::Unhandled();
		}
		return Reply::Unhandled();
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