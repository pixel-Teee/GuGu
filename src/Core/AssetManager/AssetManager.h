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

		struct SerializeDeserializeContext
		{
			uint32_t index = 0;//用于序列化的时候，给指针类型赋予一个索引
			std::unordered_map<uint32_t, meta::Object*> m_indexToObject;
		};

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

		nlohmann::json serializeJson(std::shared_ptr<meta::Object> object);

		void CollisionObjects(meta::Variant& object, SerializeDeserializeContext& context);

		nlohmann::json serializeJson(meta::Type type, const meta::Variant& instance, SerializeDeserializeContext& context);

		meta::Variant deserializeJson(meta::Type type, nlohmann::json value, const meta::Constructor& ctor, SerializeDeserializeContext& context);

		void deserializeJson(nlohmann::json value, SerializeDeserializeContext& context);

		template<typename ClassType>
		ClassType* deserializeJson(const nlohmann::json& value)
		{
			SerializeDeserializeContext context;
			context.m_indexToObject.clear();
			context.index = 0;
			deserializeJson(value, context);
		
			for (auto& item : context.m_indexToObject)
			{
				if (item.second->GetType() == typeof(ClassType))
					return static_cast<ClassType*>(item.second);
			}

			context.m_indexToObject.clear();
			context.index = 0;

			return nullptr;
		}

		void link(nlohmann::json value, meta::Object* object, SerializeDeserializeContext& context);

		GGuid getGuid(std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<AssetData> loadAsset(GGuid guid);
	private:
		//遍历目录和文件
		void traverseDirectoryAndFile_private(const GuGuUtf8Str& directory, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack);

		std::shared_ptr<RootFileSystem> m_rootFileSystem;

		std::shared_ptr<NativeFileSystem> m_nativeFileSystem;

		std::unordered_map<GGuid, std::shared_ptr<AssetData>> m_guidToAssetMap;

		nlohmann::json m_assetRegistryJson;
	};
}