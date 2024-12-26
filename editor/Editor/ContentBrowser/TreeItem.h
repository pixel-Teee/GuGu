#pragma once

#include <vector>
//#include "ContentBrowserItem.h"

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class GuGuUtf8Str;
	//展示在 asset tree 中的文件夹项
	class TreeItem : public std::enable_shared_from_this<TreeItem>
	{
	public:
		TreeItem() {}

		GuGuUtf8Str m_folderName;
		
		GuGuUtf8Str m_folderPath;

		//是否处于更名中
		bool m_bNamingFolder;

		TreeItem(GuGuUtf8Str inFolderName, GuGuUtf8Str inFolderPath, std::shared_ptr<TreeItem> inParent, bool inNamingFolder = false)
			: m_folderName(inFolderName)
			, m_folderPath(inFolderPath)
			, m_bNamingFolder(inNamingFolder)
		{}

		std::shared_ptr<TreeItem> getChild(const GuGuUtf8Str& inChildForlderName) const;
	
		//这个 tree item 的孩子
		std::vector<std::shared_ptr<TreeItem>> m_children;

		//这个 tree item 的父亲
		std::weak_ptr<TreeItem> m_parent;
	private:
	};
}