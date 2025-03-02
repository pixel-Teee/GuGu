#pragma once

#include "DetailsViewBase.h"
#include <Core/UI/UIMacros.h>

namespace GuGu {
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

		//------IDetailsView------
		virtual void setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh = false, bool bOverrideLock = false) override;
		virtual void setObjects(const std::vector<std::weak_ptr<GameObject>>& inObjects, bool bForceRefresh = false, bool bOverrideLock = false) override;
		//------IDetailsView------
	private:
		void setObjectArrayPrivate(const std::vector<GameObject*>& inObjects);

		std::shared_ptr<DetailTree> constructTreeView();

		//tree view
		std::shared_ptr<DetailTree> m_detailTree;

		//未应用过滤器之前，选中的 objects
		std::vector<std::weak_ptr<GameObject>> m_unfilteredSelectedObjects;

		std::shared_ptr<DetailsViewObjectFilter> m_objectFilter;
	};
}