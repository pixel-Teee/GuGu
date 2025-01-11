#pragma once

#include <Core/FileSystem/FileSystem.h>
#include <functional>
#include <Core/Guid.h>
#include <Core/Reflection/Type.h>

#include <json.hpp>

#include "AssetData.h"

namespace GuGu {

	class RootFileSystem;
	class AssetManager
	{
	public:
		AssetManager();

		virtual ~AssetManager();

		static AssetManager& getAssetManager();

		void getSubPaths(const GuGuUtf8Str& relativePath, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack);

		void traverseDirectoryAndFile(std::function<void(GuGuUtf8Str, bool)> enumerateCallBack);

		GuGuUtf8Str getRootPath() const;

		GuGuUtf8Str getActualPhysicalPath(const GuGuUtf8Str& relativePath);

		std::shared_ptr<RootFileSystem> getRootFileSystem() const;

		void registerAsset(const GuGuUtf8Str& guid, const GuGuUtf8Str& filePath, const GuGuUtf8Str& fileName, meta::Type assetType);

		bool isInAssetRegistry(const GGuid& fileGuid) const;

		bool isInAssetRegistry(const GuGuUtf8Str& filePath) const;

		const AssetData& getAssetData(const GuGuUtf8Str& filePath) const;
	private:
		//遍历目录和文件
		void traverseDirectoryAndFile_private(const GuGuUtf8Str& directory, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack);

		std::shared_ptr<RootFileSystem> m_rootFileSystem;

		std::shared_ptr<NativeFileSystem> m_nativeFileSystem;

		std::unordered_map<GGuid, AssetData> m_guidToAssetMap;

		nlohmann::json m_assetRegistryJson;
	};
}