#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class GStaticMesh;
	class ModelImporter
	{
	public:
		ModelImporter();

		virtual ~ModelImporter();

		GuGuUtf8Str loadModel(const GuGuUtf8Str& modelPhysicalFilePath);

	private:
		void processNode(aiNode* node, const aiScene* scene);

		void processMesh(aiMesh* mesh, const aiScene* scene);

		Assimp::Importer m_importer;

		std::shared_ptr<GStaticMesh> m_currentLoadStaticMesh;

		uint32_t m_currentStaticMeshVerticesNumber;
		uint32_t m_currentStaticMeshIndicesNumber;
	};
}