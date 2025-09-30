#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/GuGuUtf8Str.h>

#include <json.hpp>

namespace GuGu {
	class GStaticMesh;
	class ModelImporter
	{
	public:
		ModelImporter();

		virtual ~ModelImporter();

		nlohmann::json loadModel(const GuGuUtf8Str& modelPhysicalFilePath, bool isSkeleton = false);

	private:
		void processNode(aiNode* node, const aiScene* scene);

		void processMesh(aiMesh* mesh, const aiScene* scene);

		Assimp::Importer m_importer;

		std::shared_ptr<GStaticMesh> m_currentLoadStaticMesh;

		uint32_t m_currentStaticMeshVerticesNumber;
		uint32_t m_currentStaticMeshIndicesNumber;

		bool m_isSkeleton = false;
		int32_t m_boneStartIndex = -1;
	};
}