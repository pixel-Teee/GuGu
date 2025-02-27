#include <pch.h>

#include "SceneOutlinerStandaloneTypes.h"

namespace GuGu {

	namespace SceneOutlinerNameSpace
	{
		TreeItemID& TreeItemID::operator=(const TreeItemID& other)
		{
			this->m_type = other.m_type;
			this->m_gameObjectName = other.m_gameObjectName;
			this->m_folderName = other.m_folderName;
			return *this;
		}

		bool TreeItemID::operator<(const TreeItemID& rhs) const
		{
			switch (m_type)
			{
				case Type::Object: return m_gameObjectName < rhs.m_gameObjectName;
				case Type::Folder: return m_folderName < rhs.m_folderName;
			}
			return false;
		}
	}

}