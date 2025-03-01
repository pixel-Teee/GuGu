#include <pch.h>

#include "SceneOutlinerStandaloneTypes.h"

namespace GuGu {

	namespace SceneOutlinerNameSpace
	{
		TreeItemID& TreeItemID::operator=(const TreeItemID& other)
		{
			this->m_type = other.m_type;
			this->m_gameObjectIndex = other.m_gameObjectIndex;
			this->m_folderName = other.m_folderName;
			return *this;
		}

		bool TreeItemID::operator<(const TreeItemID& rhs) const
		{
			switch (m_type)
			{
				case Type::Object: return m_gameObjectIndex < rhs.m_gameObjectIndex;
				case Type::Folder: return m_folderName < rhs.m_folderName;
			}
			return false;
		}
	}

}