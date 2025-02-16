#pragma once

#include "ISceneOutliner.h"
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class SceneOutliner : public ISceneOutliner
	{
	public:
		SceneOutliner() {}

		virtual ~SceneOutliner() {}

		struct BuilderArguments : public Arguments<SceneOutliner>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

	private:

	};
}