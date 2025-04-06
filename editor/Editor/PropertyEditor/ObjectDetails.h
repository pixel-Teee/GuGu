#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class IDetailsView;
	class GameObject;
	class ObjectDetails : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<ObjectDetails>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~ObjectDetails();

		void setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh = false);
	private:
		std::shared_ptr<IDetailsView> m_detailsView;
	};
}