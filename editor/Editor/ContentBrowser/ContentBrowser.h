#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class AssetView;
	class PathView;
	class ContentBrowser : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<ContentBrowser>
		{
			BuilderArguments(){}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~ContentBrowser();

		void pathSelected(const GuGuUtf8Str& folderPath);
	private:

		std::shared_ptr<AssetView> m_assetView;

		std::shared_ptr<PathView> m_pathView;
	};
}