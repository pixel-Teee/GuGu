#pragma once

#include <Core/FileSystem/FileSystem.h>

namespace GuGu {
	class AssetManager
	{
	public:
		AssetManager();

		virtual ~AssetManager();

		static AssetManager& getAssetManager();

	private:
	};
}