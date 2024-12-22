#include <pch.h>

#include "ContentBrowser.h"

namespace GuGu {
	void ContentBrowser::init(const BuilderArguments& arguments)
	{
		//实际路径
		const GuGuUtf8Str defaultPath = "content";

		//将content实际路径转换为虚拟路径
		std::vector<GuGuUtf8Str> selectedPaths;
		selectedPaths.push_back(defaultPath);
		//m_pathView->setSelectedPaths()
	}
	ContentBrowser::~ContentBrowser()
	{
	}
}