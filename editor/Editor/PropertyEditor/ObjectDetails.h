#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class IDetailsView;
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
	private:
		std::shared_ptr<IDetailsView> m_detailsView;
	};
}