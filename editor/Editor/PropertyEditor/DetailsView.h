#pragma once

#include "DetailsViewBase.h"
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class DetailsViewObjectFilter;
	struct DetailsViewObjectRoot;
	class DetailsView : public DetailsViewBase
	{
	public:
		struct BuilderArguments : public Arguments<DetailsView>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~DetailsView();

		virtual RootPropertyNodeList& getRootNodes() override;

		//------IDetailsView------
		virtual void setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh = false, bool bOverrideLock = false) override;
		virtual void setObjects(const std::vector<std::weak_ptr<GameObject>>& inObjects, bool bForceRefresh = false, bool bOverrideLock = false) override;
		//------IDetailsView------
	private:
		void setObjectArrayPrivate(const std::vector<GameObject*>& inObjects);

		void preSetObject(int32_t inNewNumObjects);
		
		void postSetObjects(const std::vector<DetailsViewObjectRoot>& roots);

		std::shared_ptr<DetailTree> constructTreeView();

		//未应用过滤器之前，选中的 objects
		std::vector<std::weak_ptr<GameObject>> m_unfilteredSelectedObjects;

		std::shared_ptr<DetailsViewObjectFilter> m_objectFilter;

		//一组 game object 的确切的属性树的根属性节点
		std::vector<std::shared_ptr<ComplexPropertyNode>> m_rootPropertyNodes;
	};
}