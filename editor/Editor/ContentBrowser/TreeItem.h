#pragma once

#include <vector>
#include "ContentBrowserItem.h"

namespace GuGu {
	class GuGuUtf8Str;
	//展示在 asset tree 中的文件夹项
	class TreeItem : public std::enable_shared_from_this<TreeItem>
	{
	public:
		TreeItem() {}

		std::shared_ptr<TreeItem> getChild(const GuGuUtf8Str& inChildForlderName) const;
	
		//这个 tree item 的孩子
		std::vector<std::shared_ptr<TreeItem>> m_children;

		//这个 tree item 的父亲
		std::weak_ptr<TreeItem> m_parent;

		//获取底层的 content browser item
		const ContentBrowserItem& getItem() const;

	private:
		ContentBrowserItem m_item;
	};
}