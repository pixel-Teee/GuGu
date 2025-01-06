#include <pch.h>

#include "ContentBrowser.h"
#include "PathView.h"
#include "AssetView.h"

#include <Core/UI/ComplexGradient.h>
#include <Core/UI/Splitter.h>
#include <Core/UI/Button.h>
#include <Core/UI/ImageWidget.h>

#include <Core/AssetManager/AssetManager.h>

#include <Editor/StyleSet/EditorStyleSet.h>

#include <ModelImporter/ModelImporter.h>

#ifdef WIN32
#include <Application/Platform/Windows/WindowsMisc.h>
#else
#ifdef ANDROID
#include <Application/Platform/Android/AndroidMisc.h>
#endif
#endif

namespace GuGu {
	void ContentBrowser::init(const BuilderArguments& arguments, std::shared_ptr<WindowWidget> inParentWindow)
	{
		std::vector<math::float4> blueGradientBackground;
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel1"));
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel2"));

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		m_childWidget->m_childWidget = WIDGET_NEW(Splitter)
		+ Splitter::Slot()
		.value(0.13f)
		.sizeRule(Splitter::SizeRule::FractionOfParent)
		(
			WIDGET_NEW(ComplexGradient)
			.GradientColors(blueGradientBackground)
			.cornerRadius(math::float4(5.0f, 5.0f, 5.0f, 5.0f))
			.Content
			(
				WIDGET_ASSIGN_NEW(PathView, m_pathView)
				.onPathSelected(this, &ContentBrowser::pathSelected)
			)
		)
		+ Splitter::Slot()
		.value(0.84f)
		.sizeRule(Splitter::SizeRule::FractionOfParent)
		(
			WIDGET_NEW(ComplexGradient)
			.GradientColors(blueGradientBackground)
			.cornerRadius(math::float4(5.0f, 5.0f, 5.0f, 5.0f))
			.Content
			(
				WIDGET_ASSIGN_NEW(AssetView, m_assetView)
				.onGetAssetContextMenu(this, &ContentBrowser::onGetAssetContextMenu)
				.onPathSelected(this, &ContentBrowser::pathSelected)
			)
		);

		//实际路径
		const GuGuUtf8Str defaultPath = "content";

		//将content实际路径转换为虚拟路径
		std::vector<GuGuUtf8Str> selectedPaths;
		selectedPaths.push_back(defaultPath);
		m_pathView->setSelectedPaths(selectedPaths);
		m_assetView->setSourcesData(defaultPath);

		m_parentWindow = inParentWindow;
	}
	ContentBrowser::~ContentBrowser()
	{
	}
	void ContentBrowser::pathSelected(const GuGuUtf8Str& folderPath)
	{
		m_assetView->setSourcesData(folderPath);
	}
	std::shared_ptr<Widget> ContentBrowser::onGetAssetContextMenu()
	{
		GuGuUtf8Str sourcesData = m_assetView->getSourcesData();//当前所处于的文件夹

		std::shared_ptr<Button> importModelButton;
		std::shared_ptr<Widget> menuContent;
		menuContent = 
		WIDGET_NEW(Border)
		.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige7"))
		.padding(Padding(2.0f, 2.0f, 2.0f, 2.0f))
		.Content
		(
			WIDGET_NEW(Border)
			.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige4"))
			.Content
			(
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_ASSIGN_NEW(Button, importModelButton)
					.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
					.Content
					(
						WIDGET_NEW(HorizontalBox)
						+ HorizontalBox::Slot()
						.FixedWidth()
						(
							WIDGET_NEW(ImageWidget)
							.brush(EditorStyleSet::getStyleSet()->getBrush("ImportModel_Icon"))
						)
						+ HorizontalBox::Slot()
						.setPadding(Padding(5.0f, 0.0f, 5.0f, 0.0f))
						.FixedWidth()
						(
							WIDGET_NEW(TextBlockWidget)
							.text(u8"Import Model")
							.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
						)	
					)
				)
			)
		);

		importModelButton->setOnClicked(
			OnClicked([=]() {
				GuGuUtf8Str initDir = sourcesData + "/";
				std::vector<GuGuUtf8Str> filterArray;
				filterArray.push_back("FBX(*.fbx)\0");
				filterArray.push_back("*.fbx\0");
				filterArray.push_back("OBJ(*.obj)\0");
				filterArray.push_back("*.obj\0");
				initDir = sourcesData.substr(initDir.findFirstOf("/"));
				initDir = AssetManager::getAssetManager().getActualPhysicalPath(initDir);
				GuGuUtf8Str fileName;
				GuGuUtf8Str filePath;
				PlatformMisc::getOpenFilePathAndFileName(m_parentWindow, initDir, filePath, fileName, filterArray);

				if (fileName != "")
				{
					//import model
					ModelImporter modelImporter;
					GuGuUtf8Str modelJson = modelImporter.loadModel(filePath);

					GuGuUtf8Str noFileExtensionsFileName = fileName;
					int32_t dotPos = noFileExtensionsFileName.findLastOf(".");
					if (dotPos != -1)
					{
						noFileExtensionsFileName = noFileExtensionsFileName.substr(0, dotPos);
					}

					GuGuUtf8Str outputFilePath = sourcesData + "/" + noFileExtensionsFileName + ".json";
					//输出到目录
					AssetManager::getAssetManager().getRootFileSystem()->OpenFile(outputFilePath, GuGuFile::FileMode::OnlyWrite);
					AssetManager::getAssetManager().getRootFileSystem()->WriteFile((void*)modelJson.getStr(), modelJson.getTotalByteCount());
					AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
				}		
				return Reply::Handled();
			}));

		return menuContent;
	}
}