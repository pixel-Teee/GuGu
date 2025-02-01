#pragma once

#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	struct AssetData : public meta::Object
	{
		GuGuUtf8Str m_filePath;
		GuGuUtf8Str m_fileName;
		meta::Type m_assetType;

		virtual void Update(float fElapsedTimeSeconds) override;

		virtual void PostLoad() override;

		virtual meta::Type GetType() const override;

		virtual Object* Clone(void) const override;

		AssetData() {}

		AssetData(const GuGuUtf8Str& filePath, const GuGuUtf8Str& fileName, meta::Type assetType);

		std::shared_ptr<meta::Object> m_loadedResource; //加载好后的资源
	};
}