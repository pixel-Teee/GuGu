#include <pch.h>

#include "AssetManager.h"

#include <Application/Application.h>
#include <Core/FileSystem/FilePath.h>

#ifdef WIN32 
#include <windows.h>
#endif

#include <Core/Reflection/TypeCreator.h>
#include <Core/Reflection/Enum.h>
#include <Core/Reflection/ReflectionDatabase.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/Model/StaticMesh.h>
#include <Core/Texture/GTexture.h>
#include <Core/GamePlay/GameUI/GFont.h>
#include <Core/Model/GeometryHelper.h>
#include <Core/Animation/GAnimation.h>

#include "Base64.h"
#include <Core/Reflection/MetaProperty/CustomDeserializeField.h>
#include <Core/Reflection/MetaProperty/CustomSerializeField.h>

namespace GuGu {
	AssetManager::AssetManager()
	{
		GuGuUtf8Str contentDirectory = Application::GetContentDirectoryWithExecutable();

		m_nativeFileSystem = std::make_shared<NativeFileSystem>(contentDirectory);
		m_rootFileSystem = std::make_shared<RootFileSystem>();
		m_rootFileSystem->mount("content", m_nativeFileSystem);

		if (!m_nativeFileSystem->fileExists(m_nativeFileSystem->getNativeFilePath() + "/AssetRgistry.json"))
		{
			//create this file
			m_rootFileSystem->OpenFile("content/AssetRgistry.json", GuGuFile::FileMode::OnlyWrite);
			nlohmann::json newAssetRegistry = nlohmann::json::object();
			newAssetRegistry["AssetRegistry"] = nlohmann::json::array();
			GuGuUtf8Str data = newAssetRegistry.dump();
			m_rootFileSystem->WriteFile((void*)data.getStr(), data.getTotalByteCount());
			m_rootFileSystem->CloseFile();
		}
		//加载这个文件，这个文件存储了文件路径和其对应的GUID
		m_rootFileSystem->OpenFile("content/AssetRgistry.json", GuGuFile::FileMode::OnlyRead);
		int32_t fileSize = m_rootFileSystem->getFileSize();
		char* data = new char[fileSize + 1];
		data[fileSize] = '\0';
		int32_t numberOfBytesHaveReaded = 0;
		m_rootFileSystem->ReadFile(data, fileSize, numberOfBytesHaveReaded);
		m_rootFileSystem->CloseFile();
		//fileContent to GuGuUtfStr
		GuGuUtf8Str fileContent(data);
		delete[] data;
		m_assetRegistryJson = nlohmann::json::parse(fileContent.getStr());
		nlohmann::json root = m_assetRegistryJson["AssetRegistry"];
		for (const auto& item : root)
		{
			GGuid key = GuGuUtf8Str(item.at("GUID").get<std::string>());
			GuGuUtf8Str filePath = item.at("FilePath").get<std::string>();
			GuGuUtf8Str fileName = item.at("FileName").get<std::string>();
			GuGuUtf8Str assetTypeGuid = item.at("AssetType").get<std::string>();
			m_guidToAssetMap.insert({ key, std::make_shared<AssetData>(filePath, fileName, assetTypeGuid)});
		}

		//查看资产记录表里面有没有白色贴图，如果没有，生成一张默认的白色贴图
#if WIN32
		{
			GuGuUtf8Str noFileExtensionsFileName = "white";
			GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
			if (isInAssetRegistry(outputFilePath) == false)
			{
				std::shared_ptr<GTexture> whiteTexture = std::make_shared<GTexture>();
				whiteTexture->m_width = 1;
				whiteTexture->m_height = 1;
				whiteTexture->m_mipLevels = 1;
				whiteTexture->m_bytesPerPixel = 4 * 8;
				whiteTexture->m_dimension = static_cast<uint32_t>(nvrhi::TextureDimension::Texture2D);
				whiteTexture->m_data.resize(1 * 1 * 4);
				whiteTexture->m_data[0] = 255;
				whiteTexture->m_data[1] = 255;
				whiteTexture->m_data[2] = 255;
				whiteTexture->m_data[3] = 255; //(255, 255, 255, 255)
				whiteTexture->m_format = static_cast<uint32_t>(nvrhi::Format::RGBA8_UNORM);

				nlohmann::json whiteTextureJson = serializeJson(whiteTexture);
				GuGuUtf8Str guidStr = GGuid::generateGuid().getGuid();
				//whiteTextureJson["GUID"] = guidStr.getStr();
				whiteTextureJson["Version"] = std::to_string(GuGu_Version);
				GuGuUtf8Str fileContent = whiteTextureJson.dump();

				guidStr = registerAsset(guidStr, outputFilePath, noFileExtensionsFileName + ".json", meta::Type(meta::TypeIDs<GTexture>().ID));
				whiteTextureJson["GUID"] = guidStr.getStr();
				getRootFileSystem()->OpenFile(outputFilePath, GuGuFile::FileMode::OnlyWrite);
				getRootFileSystem()->WriteFile((void*)fileContent.getStr(), fileContent.getTotalByteCount());
				getRootFileSystem()->CloseFile();
			}
		}
		{
			GuGuUtf8Str noFileExtensionsFileName = "defaultCube";
			GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
			if (isInAssetRegistry(outputFilePath) == false)
			{
				GStaticMesh tempCube = GeometryHelper::createBox(1, 1, 1, 1);
				std::shared_ptr<GStaticMesh> cube = std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(tempCube.Clone()));

				nlohmann::json cubeModelJson = serializeJson(cube);
				GuGuUtf8Str guidStr = GGuid::generateGuid().getGuid();
				//cubeModelJson["GUID"] = guidStr.getStr();
				cubeModelJson["Version"] = std::to_string(GuGu_Version);
				GuGuUtf8Str fileContent = cubeModelJson.dump();

				guidStr = registerAsset(guidStr, outputFilePath, noFileExtensionsFileName + ".json", meta::Type(meta::TypeIDs<GStaticMesh>().ID));
				cubeModelJson["GUID"] = guidStr.getStr();
				getRootFileSystem()->OpenFile(outputFilePath, GuGuFile::FileMode::OnlyWrite);
				getRootFileSystem()->WriteFile((void*)fileContent.getStr(), fileContent.getTotalByteCount());
				getRootFileSystem()->CloseFile();
			}
		}
#endif
	}
	AssetManager::~AssetManager()
	{
	}
	AssetManager& AssetManager::getAssetManager()
	{
		static AssetManager g_assetManager;
		return g_assetManager;
	}
	void AssetManager::getSubPaths(const GuGuUtf8Str& relativePath, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
		//traverseDirectoryAndFile_private(m_nativeFileSystem->getNativeFilePath() + "/" + relativePath, enumerateCallBack);
		GuGuUtf8Str directory;
		if (relativePath != "")
		{
			directory = m_nativeFileSystem->getNativeFilePath() + "/" + relativePath;
		}
		else
		{
			directory = m_nativeFileSystem->getNativeFilePath();
		}
		
#ifdef WIN32 
		std::wstring searchDirectory = (directory + "/*").getUtf16String();

		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile(searchDirectory.c_str(), &findData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			GuGu_LOGE("%s", "error finding first file in directory");
			return;
		}

		do {
			const std::wstring& fileName = findData.cFileName;
			if (fileName == L"." || fileName == L"..")
				continue;
			std::wstring fullPath = directory.getUtf16String() + L"/" + fileName;
			GuGuUtf8Str utf8FullPath = GuGuUtf8Str::fromUtf16ToUtf8(fullPath.c_str());
			GuGuUtf8Str utf8RelativePath = FilePath::getRelativePath(utf8FullPath + "/", m_nativeFileSystem->getNativeFilePath() + "/").getStr();

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				GuGu_LOGD("directory %s\n", utf8RelativePath.getStr());
				if (enumerateCallBack)
				{
					bool isDirectory = true;
					//将绝对路径转换为相对路径
					enumerateCallBack("content/" + utf8RelativePath, isDirectory);
				}
			}
			else
			{
				bool isDirectory = false; //isFile
				enumerateCallBack("content/" + utf8RelativePath, isDirectory);
				GuGu_LOGD("File: %s\n", utf8RelativePath.getStr());
			}
		} while (FindNextFile(hFind, &findData) != 0);

		FindClose(hFind);
#endif
	}
	void AssetManager::traverseDirectoryAndFile(std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
		traverseDirectoryAndFile_private(m_nativeFileSystem->getNativeFilePath(), enumerateCallBack);
	}

	void AssetManager::traverseDirectoryAndFile(const GuGuUtf8Str& relativePath, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
		GuGuUtf8Str searchPath;
		if (relativePath == "")
			searchPath = m_nativeFileSystem->getNativeFilePath();
		else
			searchPath = m_nativeFileSystem->getNativeFilePath() + "/" + relativePath;
		traverseDirectoryAndFile_private(searchPath, enumerateCallBack);
	}

	GuGuUtf8Str AssetManager::getRootPath() const
	{
		//return m_nativeFileSystem->getNativeFilePath();
		return m_rootFileSystem->findMountPoint(m_nativeFileSystem);
	}

	GuGuUtf8Str AssetManager::getActualPhysicalPath(const GuGuUtf8Str& relativePath)
	{
		if (relativePath != "")
		{
			return m_nativeFileSystem->getNativeFilePath() + "/" + relativePath;
		}
		else
		{
			return m_nativeFileSystem->getNativeFilePath();
		}
	}

	std::shared_ptr<RootFileSystem> AssetManager::getRootFileSystem() const
	{
		return m_rootFileSystem;
	}

	GuGuUtf8Str AssetManager::registerAsset(const GuGuUtf8Str& guid, const GuGuUtf8Str& filePath, const GuGuUtf8Str& fileName, meta::Type assetType)
	{
		GuGuUtf8Str fileGuid = guid;
		bool bFound = false;
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.second->m_filePath == filePath)
			{
				bFound = true;
				fileGuid = item.first.getGuid();//get guid
				break;
			}
		}
		if (bFound)
		{
			return fileGuid;
		}
		else
		{
			//GuGuUtf8Str relativePath = "content/" + FilePath::getRelativePathForAsset(filePath, m_nativeFileSystem->getNativeFilePath() + "/").getStr();
			GuGuUtf8Str relativePath = filePath;
			m_guidToAssetMap.insert({ guid, std::make_shared<AssetData>(relativePath, fileName, assetType.getGuid().getGuid()) });
			m_rootFileSystem->OpenFile("content/AssetRgistry.json", GuGuFile::FileMode::OnlyWrite);

			nlohmann::json newItem = nlohmann::json::object();
			newItem["GUID"] = guid.getStr();
			newItem["FilePath"] = relativePath.getStr();
			newItem["FileName"] = fileName.getStr();
			GuGuUtf8Str assetTypeGuid = assetType.getGuid().getGuid();
			newItem["AssetType"] = assetTypeGuid.getStr();//string
			m_assetRegistryJson["AssetRegistry"].push_back(newItem);
			GuGuUtf8Str jsonFileContent = m_assetRegistryJson.dump();
			m_rootFileSystem->WriteFile((void*)jsonFileContent.getStr(), jsonFileContent.getTotalByteCount());
			m_rootFileSystem->CloseFile();

			return guid;
		}	
	}

	bool AssetManager::isInAssetRegistry(const GGuid& fileGuid) const
	{
		if (m_guidToAssetMap.find(fileGuid) == m_guidToAssetMap.end())
			return false;
		return true;
	}

	bool AssetManager::isInAssetRegistry(const GuGuUtf8Str& filePath) const
	{
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.second->m_filePath == filePath)
				return true;
		}
		return false;
	}

	const AssetData& AssetManager::getAssetData(const GuGuUtf8Str& filePath) const
	{
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.second->m_filePath == filePath)
				return *item.second;
		}
		return AssetData();
	}

	nlohmann::json AssetManager::serializeJson(std::shared_ptr<meta::Object> object, std::function<void(std::unordered_map<uint32_t, std::shared_ptr<meta::Object>>)> callback)
	{
		SerializeDeserializeContext context;
		context.m_indexToObject.clear();
		context.m_indexToSharedPtrObject.clear();
		context.index = 0;
		meta::Variant rootObject = ObjectVariant(object.get());
		CollisionObjects(rootObject, context);//收集所有object

		nlohmann::json output = nlohmann::json::object();

		nlohmann::json objectArrays = nlohmann::json::array();
		for (const auto& item : context.m_indexToObject)
		{
			meta::Variant object = ObjectVariant(item.second);
			std::string objectIndex = std::to_string(item.first);
			nlohmann::json jsonObject = serializeJson(object.GetType(), object, context);
			jsonObject["type"] = object.GetType().getGuid().getGuid().getStr();
			nlohmann::json indexAndObject = nlohmann::json::object();
			indexAndObject[objectIndex.c_str()] = jsonObject;
			objectArrays.push_back(indexAndObject);
		}
		output["Objects"] = objectArrays;
		GuGu_LOGD("%s\n", output.dump().c_str());

		if (callback)
		{
			callback(context.m_indexToSharedPtrObject);
		}

		context.m_indexToSharedPtrObject.clear();
		context.m_indexToObject.clear();
		context.index = 0;

		return output;
	}

	void AssetManager::CollisionObjects(meta::Variant& object, SerializeDeserializeContext& context)
	{
		if (object.GetType().IsSharedPtr())
		{
			std::shared_ptr<meta::Object> metaObject = *static_cast<std::shared_ptr<meta::Object>*>(object.getBase()->GetPtr());
			object = ObjectVariant(metaObject.get());
		}
		else
		{
			bool isMetaObject = false;
			auto& baseClassesType = object.GetType().GetBaseClasses();
			for (const auto& type : baseClassesType)
			{
				if (type == typeof(meta::Object))
					isMetaObject = true;
			}
			if (!isMetaObject)
				return;
		}

		//先找出所有的 std::shared_ptr<meta::object> 或者 std::weak_ptr<meta::object>
		auto type = object.GetType();

		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		bool haveAdded = false;
		for (const auto& item : context.m_indexToObject)
		{
			if (item.second == object.getBase()->GetPtr())
			{
				haveAdded = true;
				break;
			}
		}
		if (!haveAdded)
		{
			++context.index;
			meta::Object* insertObject = static_cast<meta::Object*>(object.getBase()->GetPtr());
			context.m_indexToObject.insert({ context.index, insertObject });
		}

		meta::Object* metaObject = static_cast<meta::Object*>(object.getBase()->GetPtr());
		meta::Variant instance = ObjectVariant(metaObject);
		for (auto& field : fields)
		{
			auto type = field.GetType();
			if (type != meta::Type::Invalid())
			{
				//if (type.IsSharedPtr() && type.IsArray() == false)
				//{
				//	meta::Variant value = field.GetValue(instance);
				//	void* ptr = value.getBase()->GetPtr();
				//	std::shared_ptr<meta::Object> object = *reinterpret_cast<std::shared_ptr<meta::Object>*>(&ptr);
				//	CollisionObjects(object);
				//}
				if (type.IsArray())
				{
					meta::Variant value = field.GetValue(instance);
					auto wrapper = value.GetArray();//从 variant 获取 array, array wrapper
					auto size = wrapper.Size();
					for (size_t i = 0; i < size; ++i)
					{
						meta::Variant item = wrapper.GetValue(i);//variant
						//void* ptr = item.getBase()->GetPtr();
						//std::shared_ptr<meta::Object> object = *reinterpret_cast<std::shared_ptr<meta::Object>*>(&ptr);
						CollisionObjects(item, context);
					}
				}
				else if(type != typeof(std::shared_ptr<AssetData>))
				{
					meta::Variant value = field.GetValue(instance);
					CollisionObjects(value, context);
				}
			}
		}
	}

	nlohmann::json AssetManager::serializeJson(meta::Type type, const meta::Variant& instance, SerializeDeserializeContext& context)
	{
		if (type == typeof(Array<uint8_t>))
		{
			//nlohmann::json binaryBlob = nlohmann::json::binary(instance.GetValue<Array<uint8_t>>());
			//return binaryBlob;
			GuGuUtf8Str base64Str = Base64Encode(instance.GetValue<Array<uint8_t>>());
			nlohmann::json base64 = base64Str.getStr();
			return base64;
		}

		if (type.IsArray()) //数组
		{
			nlohmann::json array = nlohmann::json::array();
			auto wrapper = instance.GetArray();//从 variant 获取 array, array wrapper
			auto size = wrapper.Size();
			for (size_t i = 0; i < size; ++i)
			{
				auto value = wrapper.GetValue(i);//variant

				array.emplace_back(
					serializeJson(value.GetType(), value, context)
				);
			}
			return array;
		}

		if (type == typeof(bool))
		{
			return instance.ToBool();
		}
	
		//number, or non-associative enum
		if (type.IsPrimitive()) //todo:add get property serialize as number
		{
			if (type.IsFloatingPoint())
			{
				return instance.ToDouble();
			}

			return instance.ToInt();
		}

		//associative enum value
		auto isEnum = type.IsEnum();
		if (isEnum)
		{
			return type.GetEnum().GetKey(instance).getStr();//str to json
		}

		if (type == typeof(GuGuUtf8Str))
		{
			return instance.ToString().getStr();
		}


		if (type == typeof(std::shared_ptr<AssetData>))
		{
			//序列化GUID
			std::shared_ptr<AssetData> assetData = *reinterpret_cast<std::shared_ptr<AssetData>*>(instance.getBase()->GetPtr());

			GGuid guid = AssetManager::getAssetManager().getGuid(assetData);

			return guid.getGuid().getStr();
		}
		else
		{
			//遇到指针，序列化成索引
			if (type.IsSharedPtr())
			{
				std::shared_ptr<meta::Object> ptr = *reinterpret_cast<std::shared_ptr<meta::Object>*>(instance.getBase()->GetPtr());
				for (auto& item : context.m_indexToObject)
				{
					if (item.second == ptr.get())
						return item.first;//index
				}
				return -1;
			}

			if (type.IsWeakPtr())
			{
				void* ptr = instance.getBase()->GetPtr();
				std::weak_ptr<meta::Object> object = *reinterpret_cast<std::weak_ptr<meta::Object>*>(ptr);
				for (auto& item : context.m_indexToObject)
				{
					if (item.second == object.lock().get())
						return item.first;//index
				}
				return -1;
			}
		}	

		nlohmann::json object = nlohmann::json::object();

		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		for (auto& field : fields)
		{
			auto value = field.GetValue(instance);//variant

			if (field.GetMeta().GetProperty<meta::CustomSerializeField>() != nullptr)
			{
				const meta::CustomSerializeField* customSerializeField = field.GetMeta().GetProperty<meta::CustomSerializeField>();
				nlohmann::json result = customSerializeField->invokeCallback(field.GetName().getStr(), value);
				if (result.contains(field.GetName().getStr()))
				{
					object[field.GetName().getStr()] = result[field.GetName().getStr()];
				}
			}
			else
			{
				//auto json = value.SerializeJson();
				auto json = serializeJson(field.GetType(), value, context);

				//value.getBase()->OnSerialize(const_cast<nlohmann::json&>(json));

				object[field.GetName().getStr()] = json;
			}			
		}

		return object;
	}

	meta::Variant AssetManager::deserializeJson(meta::Type type, nlohmann::json value, const meta::Constructor& ctor, SerializeDeserializeContext& context)
	{
		if (type == typeof(Array<uint8_t>))
		{
			//if (value.contains("bytes"))
			//{
			//	std::vector<uint8_t> binaryDataCopy = value["bytes"].get<std::vector<uint8_t>>();
			//	return Array<uint8_t>(binaryDataCopy);
			//}
			 //version 2(use base 64)
			{
				std::vector<uint8_t> base64Data = Base64Decode(value.get<std::string>());
				return Array<uint8_t>(base64Data);
			}
		}

		//数组类型需要特殊的情况
		if (type.IsArray())
		{
			auto nonArrayType = type.GetArrayType();
			auto arrayCtor = type.GetArrayConstructor();

			auto instance = arrayCtor.Invoke();
			auto wrapper = instance.GetArray();

			if (nonArrayType.IsSharedPtr() || nonArrayType.IsWeakPtr()) //指针数组
			{
				return nullptr;
			}
			else
			{
				size_t i = 0;
				for (auto& item : value) //遍历json数组
				{
					//nonArrayType.DeserializeJson(item, ctor)
					meta::Variant insertItem = deserializeJson(nonArrayType, item, ctor, context);

					wrapper.Insert(i++, insertItem);
				}
			}
			
			return instance;
		}
		//处理所有原子类型
		else if (type.IsPrimitive())
		{
			if (type == typeof(int))
				return { value.get<int>() };
			else if (type == typeof(unsigned int))
				return { static_cast<unsigned int>(value.get<unsigned int>()) };
			else if (type == typeof(bool))
				return { value.get<bool>() };
			else if (type == typeof(float))
				return { static_cast<float>(value.get<float>()) };
			else if (type == typeof(double))
				return { value.get<double>() };
			else if (type == typeof(uint8_t))
				return { value.get<uint8_t>() };
			else if (type == typeof(int16_t))
				return { value.get<int16_t>() };
			else if (type == typeof(uint16_t))
				return { value.get<uint16_t>() };
		}
		else if (type.IsEnum())
		{
			// number literal
			if (value.is_number())
				return { value.get<int>() };

			// associative value
			auto enumValue = type.GetEnum().GetValue(value.get<std::string>());

			// make sure we can find the key
			if (enumValue.IsValid())
				return enumValue;

			// use the default value as we couldn't find the key
			return meta::TypeCreator::Create(type);
		}
		else if (type == typeof(GuGuUtf8Str))
		{
			return GuGuUtf8Str(value.get<std::string>());
		}
		if (type == typeof(std::shared_ptr<AssetData>))
		{
			//特殊处理，引用了另外一种资源

			GGuid guid(value.get<std::string>());

			//去加载(有缓存)
			return AssetManager::getAssetManager().loadAsset(guid);
		}
		if (type.IsSharedPtr())
		{
			return nullptr;//后续 link
		}
		if (type.IsWeakPtr())
		{
			return nullptr;//后续 link
		}

		auto instance = ctor.Invoke();

		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		for (auto& field : fields)
		{
			auto fieldType = field.GetType();

			//if (field.GetName() == "m_jointData")
			//{
			//	int32_t qAq = 3 + 4;
			//}

			//				assert(fieldType.IsValid(),
			//					"Unknown type for field '%s' in base type '%s'. Is this type reflected?",
			//					fieldType.GetName().c_str(),
			//					GetName().c_str()
			//				);
			if(fieldType != typeof(std::shared_ptr<AssetData>))
			{
				if(fieldType.IsWeakPtr() || fieldType.IsSharedPtr()) //后续link
					continue;
			}

			auto& fieldData = value[field.GetName().getStr()];

			if (!fieldData.is_null() && fieldType.IsValid())
			{
				auto& ctor = fieldType.GetConstructor();

				if (field.GetMeta().GetProperty<meta::CustomDeserializeField>() != nullptr)
				{
					const meta::CustomDeserializeField* customDeserializeField = field.GetMeta().GetProperty<meta::CustomDeserializeField>();
					meta::Variant fieldValue = customDeserializeField->invokeCallback(field.GetName().getStr(), fieldType, value);
					field.SetValue(instance, fieldValue);
				}
				else
				{
					//auto fieldValue = fieldType.DeserializeJson(fieldData);
					auto fieldValue = deserializeJson(fieldType, fieldData, ctor, context);

					field.SetValue(instance, fieldValue);
				}		
			}
		}

		return instance;
	}

	void AssetManager::deserializeJson(nlohmann::json value, SerializeDeserializeContext& context)
	{
		auto& jsonObjects = value["Objects"];

		for (auto& element : jsonObjects)
		{
			for (auto& it : element.items())
			{
				GGuid typeGuid = GuGuUtf8Str(it.value()["type"]);
				meta::Type type = meta::Type::getType(typeGuid);
				//meta::Type type = meta::Type(typeId);
				meta::Constructor constructor = type.GetDynamicConstructor();
				meta::Variant instance = deserializeJson(type, it.value(), constructor, context);//GStaticMesh
				meta::Object* object = static_cast<meta::Object*>(instance.getBase()->GetPtr());
				object->PostLoad();
				//meta::Object& object = deserializeJson(type, it.value(), constructor).GetValue<meta::Object>();
				context.m_indexToObject.insert({ std::stoi(it.key()), object });
			}
		}
		
		//链接
		for (auto& item : context.m_indexToObject)
		{
			bool needLink = false;
			nlohmann::json object;
			for (auto& element : jsonObjects)
			{
				for (auto& it : element.items())
				{
					if (std::stoi(std::string(it.key())) == item.first)
					{
						needLink = true;
						object = it.value();
					}
					break;
				}
			}
			if (needLink)
			{
				uint32_t objectIndex = item.first;
				if (objectIndex != -1)
				{
					if (context.m_indexToSharedPtrObject.find(objectIndex) == context.m_indexToSharedPtrObject.end())
					{
						//auto& linkedObject = context.m_indexToObject.find(objectIndex)->second;
						std::shared_ptr<meta::Object> linkedObject(context.m_indexToObject.find(objectIndex)->second);
						context.m_indexToSharedPtrObject.insert({ objectIndex, linkedObject });
					}
				}
				linkSharedPtr(object, item.second, context);
			}			
		}	

		for (const auto& item : context.m_indexToObject)
		{
			if (context.m_indexToSharedPtrObject.find(item.first) == context.m_indexToSharedPtrObject.end())
			{
				context.m_indexToSharedPtrObject.insert({item.first, std::shared_ptr<meta::Object>(item.second)});
			}
		}

		//链接
		for (auto& item : context.m_indexToObject)
		{
			bool needLink = false;
			nlohmann::json object;
			for (auto& element : jsonObjects)
			{
				for (auto& it : element.items())
				{
					if (std::stoi(std::string(it.key())) == item.first)
					{
						needLink = true;
						object = it.value();
					}
					break;
				}
			}
			if (needLink)
			{
				linkWeakPtr(object, item.second, context);
			}
		}
	}

	std::shared_ptr<meta::Object> AssetManager::deserializeJsonNormalObject(const nlohmann::json& value, meta::Type inType)
	{
		SerializeDeserializeContext context;
		context.m_indexToObject.clear();
		context.m_indexToSharedPtrObject.clear();
		context.index = 0;
		deserializeJson(value, context);

		context.m_indexToObject.clear();
		context.index = 0;
		for (auto& item : context.m_indexToSharedPtrObject)
		{
			if (item.second->GetType() == inType) //check type
			{
				std::shared_ptr<meta::Object> rootObject = std::static_pointer_cast<meta::Object>(item.second);
				context.m_indexToSharedPtrObject.clear();
				return rootObject;
			}
		}
		//context.m_indexToSharedPtrObject.clear();
		return nullptr;
	}

	void AssetManager::linkSharedPtr(nlohmann::json jsonObject, meta::Object* object, SerializeDeserializeContext& context)
	{
		auto type = object->GetType();
		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		meta::Variant variantObject = ObjectVariant(object);
		
		for (auto& field : fields)
		{
			auto fieldType = field.GetType();

			if (fieldType == typeof(std::shared_ptr<AssetData>))
			{
				GGuid guid = GGuid(jsonObject[field.GetName().getStr()].get<std::string>());
				//field.SetValue(variantObject, linkedObject);
			}
			else if (fieldType.IsWeakPtr() && !fieldType.IsArray())
			{
				int32_t objectIndex = jsonObject[field.GetName().getStr()].get<int32_t>();
				if (objectIndex != -1)
				{
					//weak ptr 后续处理

					//auto& linkedObject = context.m_indexToObject.find(objectIndex)->second;
					//std::shared_ptr<meta::Object> linkedObject = std::shared_ptr<meta::Object>(context.m_indexToObject.find(objectIndex)->second);
					//std::weak_ptr<meta::Object> weakObject = linkedObject;
					//field.SetValue(variantObject, weakObject);
				}
			}
			else if (fieldType.IsSharedPtr() && !fieldType.IsArray())
			{
				if (jsonObject.contains(field.GetName().getStr()))
				{
					int32_t objectIndex = jsonObject[field.GetName().getStr()].get<int32_t>();
					if (objectIndex != -1)
					{
						if (context.m_indexToSharedPtrObject.find(objectIndex) != context.m_indexToSharedPtrObject.end())
						{
                            std::shared_ptr<meta::Object> linkedObject =  context.m_indexToSharedPtrObject.find(objectIndex)->second;

							field.SetValue(variantObject, linkedObject);
						}
						else
						{
							//auto& linkedObject = context.m_indexToObject.find(objectIndex)->second;
							std::shared_ptr<meta::Object> linkedObject(context.m_indexToObject.find(objectIndex)->second);
							context.m_indexToSharedPtrObject.insert({ objectIndex, linkedObject });
							field.SetValue(variantObject, linkedObject);
						}
					}
				}		
			}
			else if (fieldType.IsArray())
			{
				auto nonArrayType = fieldType.GetArrayType();
				if (nonArrayType.IsSharedPtr() == false)
					continue;
				auto arrayCtor = fieldType.GetArrayConstructor();

				auto instance = arrayCtor.Invoke();
				auto wrapper = instance.GetArray();

				size_t i = 0;
				for (auto& item : jsonObject[field.GetName().getStr()]) //遍历json数组
				{
					uint32_t objectIndex = item.get<int32_t>();
					if (context.m_indexToSharedPtrObject.find(objectIndex) != context.m_indexToSharedPtrObject.end())
					{
						std::shared_ptr<meta::Object> linkedObject = context.m_indexToSharedPtrObject.find(objectIndex)->second;
						wrapper.Insert(i++, linkedObject);
					}
					else
					{
                        if(context.m_indexToObject.find(objectIndex) != context.m_indexToObject.end())
                        {
                            std::shared_ptr<meta::Object> linkedObject(context.m_indexToObject.find(objectIndex)->second);
                            context.m_indexToSharedPtrObject.insert({ objectIndex, linkedObject });
                            wrapper.Insert(i++, linkedObject);
                        }
					}
					//auto& linkedObject = context.m_indexToObject.find(item.get<int32_t>())->second;
					//std::shared_ptr<meta::Object> linkedObject = std::shared_ptr<meta::Object>(context.m_indexToObject.find(objectIndex)->second);		
				}
				field.SetValue(variantObject, instance);
			}
		}
	}

	void AssetManager::linkWeakPtr(nlohmann::json jsonObject, meta::Object* object, SerializeDeserializeContext& context)
	{
		auto type = object->GetType();
		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		meta::Variant variantObject = ObjectVariant(object);
		for (auto& field : fields)
		{
			auto fieldType = field.GetType();

			if (fieldType == typeof(std::shared_ptr<AssetData>))
			{
				GGuid guid = GGuid(jsonObject[field.GetName().getStr()].get<std::string>());
				//field.SetValue(variantObject, linkedObject);
			}
			else if (fieldType.IsWeakPtr() && !fieldType.IsArray())
			{
				int32_t objectIndex = jsonObject[field.GetName().getStr()].get<int32_t>();
				if (objectIndex != -1)
				{
					//weak ptr
					std::shared_ptr<meta::Object> sharedPtrObject = context.m_indexToSharedPtrObject.find(objectIndex)->second;
					std::weak_ptr<meta::Object> weakObject = sharedPtrObject;
					field.SetValue(variantObject, weakObject);
				}
			}
			else if (fieldType.IsSharedPtr() && !fieldType.IsArray())
			{
// 				int32_t objectIndex = jsonObject[field.GetName().getStr()].get<int32_t>();
// 				if (objectIndex != -1)
// 				{
// 					//auto& linkedObject = context.m_indexToObject.find(objectIndex)->second;
// 					std::shared_ptr<meta::Object> linkedObject = std::shared_ptr<meta::Object>(context.m_indexToObject.find(objectIndex)->second);
// 					context.m_indexToSharedPtrObject.insert({ objectIndex, linkedObject });
// 					field.SetValue(variantObject, linkedObject);
// 				}
				//不做任何处理
			}
			else if (fieldType.IsArray()) //weak ptr 数组
			{
				auto nonArrayType = fieldType.GetArrayType();
				if (nonArrayType.IsWeakPtr() == false)
					continue;
				auto arrayCtor = fieldType.GetArrayConstructor();

				auto instance = arrayCtor.Invoke();
				auto wrapper = instance.GetArray();

				size_t i = 0;
				for (auto& item : jsonObject[field.GetName().getStr()]) //遍历json数组
				{
					uint32_t objectIndex = item.get<int32_t>();
					//auto& linkedObject = context.m_indexToObject.find(item.get<int32_t>())->second;
					std::shared_ptr<meta::Object> sharedPtrObject = context.m_indexToSharedPtrObject.find(objectIndex)->second;
					std::weak_ptr<meta::Object> weakObject = sharedPtrObject;
					wrapper.Insert(i++, weakObject);
				}
				field.SetValue(variantObject, instance);
			}
		}
	}

	GGuid AssetManager::getGuid(std::shared_ptr<AssetData> inAssetData)
	{
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.second->m_filePath == inAssetData->m_filePath)
				return item.first;
		}
		return GGuid();//nothing
	}

	GGuid AssetManager::getGuid(const GuGuUtf8Str& filePath, meta::Type assetType)
	{
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.second->m_filePath == filePath && meta::Type::getType(item.second->m_assetTypeGuid) == assetType)
				return item.first;
		}
		return GGuid();//nothing
	}

	std::shared_ptr<GuGu::AssetData> AssetManager::loadAssetData(const GuGuUtf8Str& filePath)
	{
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.second->m_filePath == filePath)
			{
				if (item.second->m_loadedResource == nullptr)
				{
					//load
					AssetManager::getAssetManager().getRootFileSystem()->OpenFile(item.second->m_filePath, GuGuFile::FileMode::OnlyRead);
					uint32_t fileSize = AssetManager::getAssetManager().getRootFileSystem()->getFileSize();
					char* fileContent = new char[fileSize + 1];
					fileContent[fileSize] = '\0';
					int32_t numberBytesHavedReaded = 0;
					AssetManager::getAssetManager().getRootFileSystem()->ReadFile((void*)fileContent, fileSize, numberBytesHavedReaded);
					AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
					GuGuUtf8Str json(fileContent);
					if (meta::Type::getType(item.second->m_assetTypeGuid) == typeof(GStaticMesh))
					{
						//load asset
						std::shared_ptr<meta::Object> loadedObject = AssetManager::getAssetManager().deserializeJson<GStaticMesh>(nlohmann::json::parse(json.getStr()));
						item.second->m_loadedResource = loadedObject;
					}
					else if (meta::Type::getType(item.second->m_assetTypeGuid) == typeof(GTexture))
					{
						//load asset
						std::shared_ptr<meta::Object> loadedObject = AssetManager::getAssetManager().deserializeJson<GTexture>(nlohmann::json::parse(json.getStr()));
						item.second->m_loadedResource = loadedObject;
					}
					else if (meta::Type::getType(item.second->m_assetTypeGuid) == typeof(GFont))
					{
						//load asset
						std::shared_ptr<meta::Object> loadedObject = AssetManager::getAssetManager().deserializeJson<GFont>(nlohmann::json::parse(json.getStr()));
						item.second->m_loadedResource = loadedObject;
					}
					else if (meta::Type::getType(item.second->m_assetTypeGuid) == typeof(GAnimation))
					{
						//load asset
						std::shared_ptr<meta::Object> loadedObject = AssetManager::getAssetManager().deserializeJson<GAnimation>(nlohmann::json::parse(json.getStr()));
						item.second->m_loadedResource = loadedObject;
					}
					//AssetManager::getAssetManager().deserializeJson(nlohmann::json::parse(modelJson.getStr()))

					delete[] fileContent;
					return item.second;
				}
				else
				{
					return item.second;
				}
			}
		}
		return nullptr;
	}

	std::shared_ptr<AssetData> AssetManager::loadAsset(GGuid guid)
	{
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.first == guid)
			{
				if (item.second->m_loadedResource == nullptr)
				{
					//load
					AssetManager::getAssetManager().getRootFileSystem()->OpenFile(item.second->m_filePath, GuGuFile::FileMode::OnlyRead);
					uint32_t fileSize = AssetManager::getAssetManager().getRootFileSystem()->getFileSize();
					char* fileContent = new char[fileSize + 1];
					fileContent[fileSize] = '\0';
					int32_t numberBytesHavedReaded = 0;
					AssetManager::getAssetManager().getRootFileSystem()->ReadFile((void*)fileContent, fileSize, numberBytesHavedReaded);
					AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
					GuGuUtf8Str json(fileContent);
					if (meta::Type::getType(item.second->m_assetTypeGuid) == typeof(GStaticMesh))
					{
						//load asset
						std::shared_ptr<meta::Object> loadedObject = AssetManager::getAssetManager().deserializeJson<GStaticMesh>(nlohmann::json::parse(json.getStr()));
						item.second->m_loadedResource = loadedObject;
					}
					else if(meta::Type::getType(item.second->m_assetTypeGuid) == typeof(GTexture))
					{
						//load asset
						std::shared_ptr<meta::Object> loadedObject = AssetManager::getAssetManager().deserializeJson<GTexture>(nlohmann::json::parse(json.getStr()));
						item.second->m_loadedResource = loadedObject;
					}
					else if (meta::Type::getType(item.second->m_assetTypeGuid) == typeof(GFont))
					{
						//load asset
						std::shared_ptr<meta::Object> loadedObject = AssetManager::getAssetManager().deserializeJson<GFont>(nlohmann::json::parse(json.getStr()));
						item.second->m_loadedResource = loadedObject;
					}
					//AssetManager::getAssetManager().deserializeJson(nlohmann::json::parse(modelJson.getStr()))

					delete[] fileContent;
					return item.second;
				}
				else
				{
					return item.second;
				}
			}
		}
		return nullptr;
	}

	nlohmann::json AssetManager::getDiffJson(nlohmann::json lhs, nlohmann::json rhs)
	{
		nlohmann::json result = nlohmann::json::object();

		//类型不同，完全差异(一般不会出现)
		if (lhs.type() != rhs.type())
		{
			result["old"] = lhs;
			result["new"] = rhs;
			return result;
		}

		//基本类型
		if (!lhs.is_structured())
		{
			if (lhs != rhs)
			{
				result["old"] = lhs;
				result["new"] = rhs;
			}
			return nullptr;//无差异返回空对象
		}

		//处理对象类型
		if (lhs.is_object())
		{
			std::vector<std::string> keys;
			for (auto& item : lhs.items()) keys.push_back(item.key());
			for (auto& item : rhs.items())
			{
				if (lhs.find(item.key()) == lhs.end())
				{
					keys.push_back(item.key());//合并键列表
				}
			}

			for (const auto& key : keys)
			{
				if (lhs.contains(key) && !rhs.contains(key))
				{
					result[key] = { {"removed", lhs[key]} }; //键被删除
				}
				else if (!lhs.contains(key) && rhs.contains(key))
				{
					result[key] = { {"added", rhs[key]} };
				}
				else
				{
					auto diff = getDiffJson(lhs[key], rhs[key]);
					if (!diff.empty())
						result[key] = diff;//字段差异
				}
			}
			return result;
		}

		//处理数组类型
		if (lhs.is_array())
		{
			for (size_t i = 0; i < std::min(lhs.size(), rhs.size()); ++i)
			{
				auto diff = getDiffJson(lhs[i], rhs[i]);
				if (!diff.empty())
					result[std::to_string(i)] = diff;//记录索引位置差异
			}

			//处理数组长度差异
			if (lhs.size() > rhs.size())
			{
				result["_size"] = { {"old", lhs.size()}, {"new", rhs.size()} };
				for (size_t i = rhs.size(); i < lhs.size(); ++i)
					result[std::to_string(i)] = { {"removed", lhs[i]} };
			}
			else if (rhs.size() > lhs.size())
			{
				result["_size"] = { {"old", lhs.size()}, {"new", rhs.size()} };
				for (size_t i = lhs.size(); i < rhs.size(); ++i)
					result[std::to_string(i)] = { {"added", rhs[i]} };
			}
			return result;
		}

		return result;//默认返回空对象
	}

	std::shared_ptr<meta::Object> AssetManager::cloneObject(std::shared_ptr<meta::Object> inClonedObject)
	{
		//临时方案(先序列化，再序列化)
		nlohmann::json resultJson = AssetManager::getAssetManager().serializeJson(inClonedObject);

		return deserializeJsonNormalObject(resultJson, inClonedObject->GetType());
	}

	//遍历目录
	void AssetManager::traverseDirectoryAndFile_private(const GuGuUtf8Str& directory, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
#ifdef WIN32 
		std::wstring searchDirectory = (directory + "/*").getUtf16String();

		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile(searchDirectory.c_str(), &findData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			GuGu_LOGE("%s", "error finding first file in directory");
			return;
		}

		do {
			const std::wstring& fileName = findData.cFileName;
			if(fileName == L"." || fileName == L"..")
				continue;
			std::wstring fullPath = directory.getUtf16String() + L"/" + fileName;
			GuGuUtf8Str utf8FullPath = GuGuUtf8Str::fromUtf16ToUtf8(fullPath.c_str());
			GuGuUtf8Str utf8RelativePath = FilePath::getRelativePath(utf8FullPath + "/", m_nativeFileSystem->getNativeFilePath() + "/").getStr();

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{	
				GuGu_LOGD("directory %s\n", utf8RelativePath.getStr());
				if (enumerateCallBack)
				{
					bool isDirectory = true;
					//将绝对路径转换为相对路径
					enumerateCallBack("content/" + utf8RelativePath, isDirectory);
				}
				traverseDirectoryAndFile_private(utf8FullPath, enumerateCallBack);
			}
			else
			{
				bool isDirectory = false; //isFile
				enumerateCallBack("content/" + utf8RelativePath, isDirectory);
				GuGu_LOGD("File: %s\n", utf8RelativePath.getStr());
			}
		} while (FindNextFile(hFind, &findData) != 0);

		FindClose(hFind);
#endif
	}

	
}