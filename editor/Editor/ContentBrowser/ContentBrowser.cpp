#include <pch.h>

#include "ContentBrowser.h"
#include "PathView.h"
#include "AssetView.h"

#include <Core/UI/ComplexGradient.h>
#include <Core/UI/Splitter.h>

#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {
	void ContentBrowser::init(const BuilderArguments& arguments)
	{
		std::vector<math::float4> blueGradientBackground;
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel1"));
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel2"));

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		m_childWidget->m_childWidget = WIDGET_NEW(Splitter)
		+ Splitter::Slot()
		.value(0.16f)
		.sizeRule(Splitter::SizeRule::FractionOfParent)
		.setPadding(Padding(0.0f, 0.0f, 25.0f, 0.0f))
		(
			WIDGET_NEW(ComplexGradient)
			.GradientColors(blueGradientBackground)
			.cornerRadius(math::float4(10.0f, 10.0f, 10.0f, 10.0f))
			.Content
			(
				WIDGET_ASSIGN_NEW(PathView, m_pathView)
				.onPathSelected(this, &ContentBrowser::pathSelected)
			)
		)
		+ Splitter::Slot()
		.value(0.78f)
		.sizeRule(Splitter::SizeRule::FractionOfParent)
		.setPadding(Padding(0.0f, 0.0f, 25.0f, 0.0f))
		(
			WIDGET_NEW(ComplexGradient)
			.GradientColors(blueGradientBackground)
			.cornerRadius(math::float4(10.0f, 10.0f, 10.0f, 10.0f))
			.Content
			(
				WIDGET_ASSIGN_NEW(AssetView, m_assetView)
			)
		);

		//实际路径
		const GuGuUtf8Str defaultPath = "content";

		//将content实际路径转换为虚拟路径
		std::vector<GuGuUtf8Str> selectedPaths;
		selectedPaths.push_back(defaultPath);
		m_pathView->setSelectedPaths(selectedPaths);
		m_assetView->setSourcesData(defaultPath);
	}
	ContentBrowser::~ContentBrowser()
	{
	}
	void ContentBrowser::pathSelected(const GuGuUtf8Str& folderPath)
	{
		m_assetView->setSourcesData(folderPath);
	}
}