#pragma once

#include <Core/GuGuUtf8Str.h>
#include <json.hpp>

namespace GuGu {
	class GTexture;
	class TextureImporter
	{
	public:
		TextureImporter();

		virtual ~TextureImporter();

		nlohmann::json loadTexture(const GuGuUtf8Str& texturePhysicalFilePath);

		std::shared_ptr<GTexture> m_currentLoadTexture;
	};
}