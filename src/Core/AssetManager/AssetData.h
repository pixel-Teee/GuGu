#pragma once

#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	struct AssetData
	{
		GuGuUtf8Str m_filePath;
		GuGuUtf8Str m_fileName;
		meta::Type m_assetType;
	};
}