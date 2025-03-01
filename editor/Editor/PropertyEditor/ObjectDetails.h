#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class IDetailsView;
	class ActorDetails : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<ActorDetails>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~ActorDetails();
	private:
		std::shared_ptr<IDetailsView> m_detailsView;
	};
}