#pragma once

#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	class ISceneOutliner;
	struct SceneOutlinerCommonLabelData
	{
		std::weak_ptr<ISceneOutliner> m_weakSceneOutliner;
	};

	namespace SceneOutlinerNameSpace
	{
		struct TreeItemID
		{
		public:
			enum class Type : uint8_t
			{
				Object,
				Folder,
				Bone,
				Null
			};

			TreeItemID() : m_type(Type::Null) {}

			TreeItemID(const std::shared_ptr<GameObject> inGameObject)
			: m_type(Type::Object)
			, m_gameObjectIndex(inGameObject->getId()) //todo:fix this
			{}

			TreeItemID(const GuGuUtf8Str& inFolder) 
			: m_type(Type::Folder)
			{}

			TreeItemID(const TreeItemID& rhs)
			{
				*this = rhs;
			}

			//copy
			TreeItemID& operator=(const TreeItemID& other);

			bool operator<(const TreeItemID& rhs) const;

		private:

			Type m_type;

			uint32_t m_gameObjectIndex;
			GuGuUtf8Str m_folderName;
		};
	}
}