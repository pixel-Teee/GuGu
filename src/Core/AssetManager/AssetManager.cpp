#include <pch.h>

#include "AssetManager.h"

#include <Application/Application.h>

namespace GuGu {
	AssetManager::AssetManager()
	{
		GuGuUtf8Str contentDirectory = Application::GetContentDirectoryWithExecutable();
	}
	AssetManager::~AssetManager()
	{
	}
	AssetManager& AssetManager::getAssetManager()
	{
		static AssetManager g_assetManager;
		return g_assetManager;
	}
}