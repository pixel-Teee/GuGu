#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/GuGuUtf8Str.h>

#include <json.hpp>

namespace GuGu {
	class GStaticMesh;
	struct GMeshGeometry;
	class ModelImporter
	{
	public:
		ModelImporter();

		virtual ~ModelImporter();

		nlohmann::json loadModel(const GuGuUtf8Str& modelPhysicalFilePath, bool isSkeleton = false);

		nlohmann::json loadAnimation(const GuGuUtf8Str& animationFilePath);
	private:
		void processNode(aiNode* node, const aiScene* scene);

		void processMesh(aiMesh* mesh, const aiScene* scene, GMeshGeometry& gMeshGeometry);

		void linkMeshGeometryId(aiNode* node, int32_t parentIndex = -1);

		Assimp::Importer m_importer;

		std::shared_ptr<GStaticMesh> m_currentLoadStaticMesh;

		uint32_t m_currentStaticMeshVerticesNumber;
		uint32_t m_currentStaticMeshIndicesNumber;
		//int32_t m_currentMeshId = 0;

		bool m_isSkeleton = false;
		int32_t m_boneStartIndex = -1;
	};
}