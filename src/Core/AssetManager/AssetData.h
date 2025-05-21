#pragma once

#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	struct AssetData : public meta::Object
	{
		GuGuUtf8Str m_filePath;
		GuGuUtf8Str m_fileName;
		GGuid m_assetTypeGuid;

		virtual void Update(float fElapsedTimeSeconds) override;

		virtual void PostLoad() override;

		virtual meta::Type GetType() const override;

		virtual Object* Clone(void) const override;

		AssetData() {}

		AssetData(const GuGuUtf8Str& filePath, const GuGuUtf8Str& fileName, const GuGuUtf8Str& assetTypeGuid);

		GuGuUtf8Str toString() const;

		std::shared_ptr<meta::Object> m_loadedResource; //加载好后的资源
	};
}