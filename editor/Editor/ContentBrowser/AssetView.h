#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	//显示过滤的资产的 widget
	class AssetView : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<AssetView>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~AssetView();
	private:

	};
}