#pragma once

namespace GuGu {
	class FileSystem;
	class GuGuUtf8Str;
	class SceneGraphNode;
	bool Load(const GuGuUtf8Str& filePath, std::shared_ptr<SceneGraphNode>& outRoot, std::shared_ptr<FileSystem> fileSystem);
}