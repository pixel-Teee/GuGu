#pragma once

#include <Core/FileSystem/FileSystem.h>
#include <functional>

namespace GuGu {
	class RootFileSystem;
	class AssetManager
	{
	public:
		AssetManager();

		virtual ~AssetManager();

		static AssetManager& getAssetManager();

		void traverseDirectoryAndFile(std::function<void(GuGuUtf8Str, bool)> enumerateCallBack);

		GuGuUtf8Str getRootPath() const;
	private:
		//遍历目录和文件
		void traverseDirectoryAndFile_private(const GuGuUtf8Str& directory, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack);

		std::shared_ptr<RootFileSystem> m_rootFileSystem;

		std::shared_ptr<NativeFileSystem> m_nativeFileSystem;
	};
}