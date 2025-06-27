#include <pch.h>

#include "TransactionManager.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/Level.h>

namespace GuGu {

	TransactionManager::TransactionManager()
	{

	}

	TransactionManager::~TransactionManager()
	{

	}

	TransactionManager& TransactionManager::getTransactionManager()
	{
		static TransactionManager transactionManager;
		return transactionManager;
	}

	void TransactionManager::beginTransaction()
	{
		if (m_bInTransaction)
		{
			GuGu_LOGE("transaction already in progress");
		}
		m_bInTransaction = true;
	}

	void TransactionManager::commit()
	{
		if (!m_bInTransaction)
		{
			GuGu_LOGE("no transaction to commit");
		}

		for (size_t i = 0; i < m_currentTransaction.m_currentObjects.size(); ++i)
		{
			for (const auto& item : m_currentTransaction.m_currentObjects[i])
			{
				if(item.first.isRoot)
					serializeJson(item.first.m_object, false, i);
			}
		}

		m_undoStack.push(std::move(m_currentTransaction));

		while (!m_redoStack.empty())
		{
			m_redoStack.pop();
		}

		m_bInTransaction = false;
		m_currentTransaction = Transaction();

//		for (size_t i = 0; i < m_currentTransaction.m_currentObjects.size(); ++i)
//		{
//			//get after state
//			if (m_currentTransaction.m_currentObjects[i].lock())
//			{
//				nlohmann::json objectJson = AssetManager::getAssetManager().serializeJson(m_currentTransaction.m_currentObjects[i].lock());
//				std::string objectStr = objectJson.dump();//deep copy
//				m_currentTransaction.m_afterState.push_back(GuGuUtf8Str(objectStr).getBinary());
//			}
//			else
//			{
//				GuGu_LOGD("missing object");
//			}		
//		}
//
//		m_undoStack.push(std::move(m_currentTransaction));
//
//		while (!m_redoStack.empty())
//		{
//			m_redoStack.pop();
//		}
//
//		m_bInTransaction = false;
//		m_currentTransaction.m_beforeState.clear();
//		m_currentTransaction.m_afterState.clear();
//		m_currentTransaction.m_currentObjects.clear();
	}

	void TransactionManager::undo()
	{
		if (m_bInTransaction)
		{
			GuGu_LOGE("cannot undo during a transaction");
		}

		if (m_undoStack.empty())
		{
			GuGu_LOGD("nothing to undo");
		}

		auto transaction = m_undoStack.top();

		m_undoStack.pop();

		for (size_t i = 0; i < transaction.m_currentObjects.size(); ++i)
		{
			//find root object
			bool isFindRootObjet = false;
			std::map<int32_t, meta::Object*> indexToObjects;
			for (const auto& item : transaction.m_currentObjects[i])
			{
				if (item.first.isRoot)
				{
					isFindRootObjet = true;
					indexToObjects = item.second.m_indexToObjects;
					break;
				}
			}
			if (isFindRootObjet)
			{
				for (const auto& item : transaction.m_currentObjects[i])
				{
					//modify every object
					const TrackObject& trackObject = item.first;
					std::shared_ptr<meta::Object> object = trackObject.m_object;
					//get json to diff
					const nlohmann::json& beforeState = item.second.m_beforeState;
					const nlohmann::json& afterState = item.second.m_afterState;
					if (beforeState.empty() || afterState.empty())
						continue;
					GuGu_LOGD("before state:%s", beforeState.dump().c_str());
					GuGu_LOGD("after state:%s", afterState.dump().c_str());
					//from after to before
					nlohmann::json diffJson = getDiffJson(afterState, beforeState);		
					nlohmann::json diffJson2;
					for (const auto& item : diffJson.items())
					{
						diffJson2 = item.value();
						break;
					}
					if (!diffJson2.empty())
					{
						GuGu_LOGD("diff:%s", diffJson2.dump().c_str());
						//modify object
						meta::Variant objectVariant = ObjectVariant(object.get());
						updateObject(objectVariant, objectVariant.GetType(), diffJson2, indexToObjects);
					}
				}
			}
		}
		//refresh
		World::getWorld()->getCurrentLevel()->refreshLevel();

		m_redoStack.push(transaction);
	}

	void TransactionManager::redo()
	{
		if (m_bInTransaction)
		{
			GuGu_LOGE("cannot undo during a transaction");
		}

		if (m_redoStack.empty())
		{
			GuGu_LOGD("nothing to redo");
		}

		auto transaction = m_redoStack.top();

		m_redoStack.pop();

		for (size_t i = 0; i < transaction.m_currentObjects.size(); ++i)
		{
			//find root object
			bool isFindRootObjet = false;
			std::map<int32_t, meta::Object*> indexToObjects;
			for (const auto& item : transaction.m_currentObjects[i])
			{
				if (item.first.isRoot)
				{
					isFindRootObjet = true;
					indexToObjects = item.second.m_indexToObjects;
					break;
				}
			}
			if (isFindRootObjet)
			{
				for (const auto& item : transaction.m_currentObjects[i])
				{
					//modify every object
					const TrackObject& trackObject = item.first;
					std::shared_ptr<meta::Object> object = trackObject.m_object;
					//get json to diff
					const nlohmann::json& beforeState = item.second.m_beforeState;
					const nlohmann::json& afterState = item.second.m_afterState;
					if (beforeState.empty() || afterState.empty())
						continue;
					GuGu_LOGD("before state:%s", beforeState.dump().c_str());
					GuGu_LOGD("after state:%s", afterState.dump().c_str());
					//from before to after
					nlohmann::json diffJson = getDiffJson(beforeState, afterState);
					nlohmann::json diffJson2;
					for (const auto& item : diffJson.items())
					{
						diffJson2 = item.value();
						break;
					}
					if (!diffJson2.empty())
					{
						GuGu_LOGD("diff:%s", diffJson2.dump().c_str());
						//modify object
						meta::Variant objectVariant = ObjectVariant(object.get());
						updateObject(objectVariant, objectVariant.GetType(), diffJson2, indexToObjects);
					}
				}
			}
		}
		//refresh
		World::getWorld()->getCurrentLevel()->refreshLevel();

		m_undoStack.push(transaction);
	}

	Transaction& TransactionManager::getCurrentTransaction()
	{
		return m_currentTransaction;
	}

	void TransactionManager::modifyObject(std::shared_ptr<meta::Object> inObject)
	{
		serializeJson(inObject, true); //每个对象对应一个序列化块，存储在m_currentTransaction.m_currentObjects里面
	}

	void TransactionManager::rollback()
	{
		if (!m_bInTransaction)
		{
			GuGu_LOGE("no transaction to rollback");
		}

		//for (size_t i = 0; i < m_currentTransaction.m_beforeState.size(); ++i)
		//{
		//	std::string beforeStateStr(reinterpret_cast<const char*>(m_currentTransaction.m_beforeState[i].data()), m_currentTransaction.m_beforeState[i].size());
		//	nlohmann::json beforeStateJson = nlohmann::json::parse(beforeStateStr.c_str());
		//	if (m_currentTransaction.m_currentObjects[i].lock())
		//	{
		//		meta::Type objectType = m_currentTransaction.m_currentObjects[i].lock()->GetType();
		//		std::shared_ptr<meta::Object> beforeObject = AssetManager::getAssetManager().deserializeJsonNormalObject(beforeStateJson, objectType);
		//		//implement diff and copy
		//	}
		//	else
		//	{
		//		GuGu_LOGE("missing object");
		//	}
		//}
		//m_bInTransaction = false;
		//m_currentTransaction.m_beforeState.clear();
		//m_currentTransaction.m_afterState.clear();
		//m_currentTransaction.m_currentObjects.clear();
	}

	void TransactionManager::preDiff(std::shared_ptr<meta::Object> inObject, const nlohmann::json& diff)
	{
		if (!diff.is_object()) return;

		if (diff.contains("Objects"))
		{
			nlohmann::json objects = diff["Objects"];
			for (const auto& [objectId, objectValue] : objects.items()) 
			{
				//object id
				if (objectValue.contains("removed"))
				{

				}
				else if (objectValue.contains("added"))
				{

				}
			}
		}
	}

	void TransactionManager::applyDiff(std::shared_ptr<meta::Object> inObject, const nlohmann::json& diff)
	{
		if (!diff.is_object()) return;

		for (const auto& [key, value] : diff.items())
		{
			if (key == "value")
			{
				//特殊处理整个对象替换
				return;
			}

			//查找匹配的字段
			bool bFound = false;
			auto& fields = meta::ReflectionDatabase::Instance().types[inObject->GetType().GetID()].fields;
			for (auto& field : fields)
			{
				if (field.GetName() == key)
				{
					applyFieldDiff(inObject, field, value);
					bFound = true;
					break;
				}
			}

			//处理嵌套的对象
			if (!bFound)
			{
				applyDiff(inObject, value);
			}
		}
	}

	void TransactionManager::applyFieldDiff(std::shared_ptr<meta::Object> inObject, meta::Field& field, const nlohmann::json& diff)
	{
		if (diff.is_object())
		{
			//处理字段级别的更新类型
			if (diff.contains("value"))
			{
				meta::Type fieldType = field.GetType();
				meta::Variant fieldValue = convertJsonValueToVariantValue(fieldType, diff["value"]);
				meta::Variant instance = ObjectVariant(inObject.get());

				//直接值更新
				field.SetValue(instance, fieldValue);
			}
			else if (diff.contains("added"))
			{
				meta::Type fieldType = field.GetType();
				meta::Variant fieldValue = convertJsonValueToVariantValue(fieldType, diff["added"]);
				meta::Variant instance = ObjectVariant(inObject.get());

				//直接值更新
				field.SetValue(instance, fieldValue);
			}
			else if (diff.contains("removed"))
			{
				meta::Type fieldType = field.GetType();
				meta::Variant fieldValue = convertJsonValueToVariantValue(fieldType, nlohmann::json());
				meta::Variant instance = ObjectVariant(inObject.get());

				//直接值更新
				field.SetValue(instance, fieldValue);
			}
			else
			{
				//递归处理嵌套
			}
		}
		else
		{
			meta::Type fieldType = field.GetType();
			meta::Variant fieldValue = convertJsonValueToVariantValue(fieldType, diff);
			meta::Variant instance = ObjectVariant(inObject.get());
			
			//直接值更新
			field.SetValue(instance, fieldValue);
		}
	}

	void TransactionManager::serializeJson(std::shared_ptr<meta::Object> inObject, bool isBeforeState, int32_t modifyMapIndex)
	{
		if (isBeforeState)
		{
			bool findMap = false;
			int32_t findIndex = -1;
			for (int32_t index = 0; index < m_currentTransaction.m_currentObjects.size(); ++index)
			{
				const auto& singleMap = m_currentTransaction.m_currentObjects[index];
				for (const auto& item : singleMap)
				{
					if (item.first.m_object == inObject)
					{
						findMap = true;
						findIndex = index;
						break;
					}
				}
				if (findMap)
					break;
			}
			if (!findMap)
			{
				m_currentTransaction.m_currentObjects.push_back(std::map<TrackObject, ModifyState>());
				findIndex = m_currentTransaction.m_currentObjects.size() - 1;
			}
			modifyMapIndex = findIndex;
		}
		else
		{
			if(modifyMapIndex == -1)
				return;
		}
		//SerializeDeserializeContext context;
		//context.m_indexToObject.clear();
		//context.m_indexToSharedPtrObject.clear();
		//context.index = 0;

		TrackObject rootTrackObject;
		rootTrackObject.m_object = inObject;
		rootTrackObject.isRoot = true;

		int32_t maxIndex = 0;
		for (const auto& item : m_currentTransaction.m_currentObjects[modifyMapIndex][rootTrackObject].m_indexToObjects)
		{
			maxIndex = std::max(maxIndex, item.first);
		}

		meta::Variant rootObject = ObjectVariant(inObject.get());
		collisionObjects(rootObject, m_currentTransaction.m_currentObjects[modifyMapIndex][rootTrackObject].m_indexToObjects, maxIndex);

		for (const auto& item : m_currentTransaction.m_currentObjects[modifyMapIndex][rootTrackObject].m_indexToObjects)
		{
			meta::Variant object = ObjectVariant(item.second);
			std::string objectIndex = std::to_string(item.first);
			nlohmann::json jsonObject = serializeJson(object.GetType(), object, m_currentTransaction.m_currentObjects[modifyMapIndex][rootTrackObject].m_indexToObjects);
			nlohmann::json indexAndObject = nlohmann::json::object();
			indexAndObject[objectIndex.c_str()] = jsonObject;

			TrackObject trackObject;
			trackObject.m_object = item.second->shared_from_this();
			trackObject.isRoot = (inObject == item.second->shared_from_this());

			if (isBeforeState)
			{
				m_currentTransaction.m_currentObjects[modifyMapIndex][trackObject].m_beforeState = indexAndObject;
				GuGu_LOGD("before state:%s", indexAndObject.dump().c_str());
			}
			else
			{
				m_currentTransaction.m_currentObjects[modifyMapIndex][trackObject].m_afterState = indexAndObject;
				GuGu_LOGD("after state:%s", indexAndObject.dump().c_str());
			}
		}
	}

	nlohmann::json TransactionManager::serializeJson(meta::Type type, const meta::Variant& instance, std::map<int32_t, meta::Object*>& indexToObject)
	{
		if (type.IsArray()) //数组
		{
			nlohmann::json array = nlohmann::json::array();
			auto wrapper = instance.GetArray();//从 variant 获取 array, array wrapper
			auto size = wrapper.Size();
			for (size_t i = 0; i < size; ++i)
			{
				auto value = wrapper.GetValue(i);//variant

				array.emplace_back(
					serializeJson(value.GetType(), value, indexToObject)
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
				for (auto& item : indexToObject)
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
				for (auto& item : indexToObject)
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

			//auto json = value.SerializeJson();
			auto json = serializeJson(field.GetType(), value, indexToObject);

			//value.getBase()->OnSerialize(const_cast<nlohmann::json&>(json));

			object[field.GetName().getStr()] = json;
		}

		return object;
	}

	nlohmann::json TransactionManager::getDiffJson(const nlohmann::json& lhs, const nlohmann::json& rhs)
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
				return result;
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
				result["diffSize"] = { {"old", lhs.size()}, {"new", rhs.size()} };
				for (size_t i = rhs.size(); i < lhs.size(); ++i)
					result[std::to_string(i)] = { {"removed", lhs[i]} };
			}
			else if (rhs.size() > lhs.size())
			{
				result["diffSize"] = { {"old", lhs.size()}, {"new", rhs.size()} };
				for (size_t i = lhs.size(); i < rhs.size(); ++i)
					result[std::to_string(i)] = { {"added", rhs[i]} };
			}
			return result;
		}

		return result;//默认返回空对象
	}

	void TransactionManager::updateObject(meta::Variant& instance, meta::Type inType, const nlohmann::json& diffJson, const std::map<int32_t, meta::Object*>& indexToObjects)
	{
		if (diffJson.empty())
			return;

		//数组
		if (inType.IsArray())
		{
			auto nonArrayType = inType.GetArrayType();
			auto wrapper = instance.GetArray();
			auto arrayCtor = inType.GetArrayConstructor();
			if (!diffJson["diffSize"].empty()) //数组长度发生变化
			{
				int32_t oldSize = diffJson["diffSize"]["old"].get<int32_t>();
				int32_t newSize = diffJson["diffSize"]["new"].get<int32_t>();
				if (oldSize > newSize)
					wrapper.Resize(newSize);
				
				//增加新的 item
				if (oldSize < newSize)
				{
					for (int32_t index = oldSize; index < newSize; ++index)
					{
						if (diffJson[std::to_string(index)].contains("added"))
						{
							if (nonArrayType.IsSharedPtr() || nonArrayType.IsWeakPtr())
							{
								int32_t itemValue = diffJson[std::to_string(index)]["added"].get<int32_t>();
								if (indexToObjects.find(itemValue) != indexToObjects.end())
								{
									meta::Object* pointerToObject = indexToObjects.find(itemValue)->second;
									std::shared_ptr<meta::Object> objectValue = pointerToObject->shared_from_this();
									wrapper.Insert(index, objectValue);
								}
							}
							else
							{
								//check
								if (!diffJson[std::to_string(index)].contains("added"))
								{
									meta::Variant value = arrayCtor.Invoke();
									wrapper.Insert(index, value);
									updateObject(value, nonArrayType, diffJson[std::to_string(index)]["added"], indexToObjects);
								}
							}
						}
					}
				}
			}
			//更新修改
			for (const auto& item : diffJson.items())
			{
				if(item.key() == "diffSize")
					continue;
				if (!diffJson[item.key()].empty())
				{
					if (nonArrayType.IsSharedPtr() || nonArrayType.IsWeakPtr())
					{
						nlohmann::json value = diffJson[item.key()];
						if(value.contains("removed") || value.contains("added"))
							continue;
						if (indexToObjects.find(item.value().get<int32_t>()) != indexToObjects.end())
						{
							meta::Object* pointerToObject = indexToObjects.find(item.value().get<int32_t>())->second;
							std::shared_ptr<meta::Object> objectValue = pointerToObject->shared_from_this();
							wrapper.SetValue(std::stoi(item.key()), objectValue);
						}
					}
					else
					{
						//check
						if (!diffJson[item.key()].contains("added") && !!diffJson[item.key()].contains("removed"))
							updateObject(wrapper.GetValue(std::stoi(item.key())), nonArrayType, diffJson[std::to_string(std::stoi(item.key()))], indexToObjects);
					}
				}
			}
		}
		
		//object
		meta::Type instanceType = instance.GetType();
		auto& fields = meta::ReflectionDatabase::Instance().types[instanceType.GetID()].fields;

		for (auto& field : fields)
		{
			if (!diffJson.contains(field.GetName().getStr()))
				continue;

			auto& fieldValue = field.GetValue(instance);//variant
			meta::Type fieldType = field.GetType();
			if (fieldType.IsPrimitive())
			{
				field.SetValue(instance, convertJsonToPrimitiveValue(fieldType, diffJson[field.GetName().getStr()]["new"]));
			}
			else if (fieldType.IsEnum())
			{
				if(diffJson[field.GetName().getStr()].is_number())
					field.SetValue(instance, diffJson[field.GetName().getStr()].get<int32_t>());
				else
					field.SetValue(instance, fieldType.GetEnum().GetValue(diffJson[field.GetName().getStr()]["new"].get<std::string>()));
			}
			else if (fieldType == typeof(std::shared_ptr<AssetData>))
			{
				GGuid guid(diffJson[field.GetName().getStr()]["new"].get<std::string>());
				field.SetValue(instance, AssetManager::getAssetManager().loadAsset(guid));
			}
			else if (fieldType.IsArray() == false && (fieldType.IsWeakPtr() || fieldType.IsSharedPtr()))
			{
				//field.SetValue
				if (indexToObjects.find(diffJson[field.GetName().getStr()]["new"].get<int32_t>()) != indexToObjects.end())
				{
					meta::Object* pointerToObject = indexToObjects.find(diffJson[field.GetName().getStr()]["new"].get<int32_t>())->second;
					std::shared_ptr<meta::Object> objectValue = pointerToObject->shared_from_this();
					field.SetValue(instance, objectValue);
				}
				else
				{
					std::shared_ptr<meta::Object> emptyValue;
					field.SetValue(instance, emptyValue);
				}
			}
			else
			{
				//object and array
				if(fieldType.IsArray())
					updateObject(field.GetValue(instance), fieldType, diffJson[field.GetName().getStr()], indexToObjects);
				else
					updateObject(field.GetValueReference(instance), fieldType, diffJson[field.GetName().getStr()], indexToObjects);
			}
		}
	}

	meta::Variant TransactionManager::convertJsonToPrimitiveValue(meta::Type type, const nlohmann::json& value)
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
	}

	bool TransactionManager::CheckIsDerivedFromMetaObject(meta::Type currentType)
	{
		bool isMetaObject = false;
		meta::Type::Set baseClassesType = currentType.GetBaseClasses();
		while (!baseClassesType.empty())
		{
			if(*baseClassesType.begin() == typeof(meta::Object))
				return true;
			baseClassesType = (*baseClassesType.begin()).GetBaseClasses();
		}
		//for (const auto& type : baseClassesType)
		//{
		//	if (type == typeof(meta::Object))
		//		isMetaObject = true;
		//}
		return isMetaObject;
	}

	void TransactionManager::collisionObjects(meta::Variant& object, std::map<int32_t, meta::Object*>& indexToObject, int32_t& currentIndex)
	{
		bool isWeakPtr = object.GetType().IsWeakPtr();
		if (object.GetType().IsSharedPtr())
		{
			std::shared_ptr<meta::Object> metaObject = *static_cast<std::shared_ptr<meta::Object>*>(object.getBase()->GetPtr());
			object = ObjectVariant(metaObject.get());
		}
		else if (isWeakPtr)
		{
			std::shared_ptr<meta::Object> metaObject = (*static_cast<std::weak_ptr<meta::Object>*>(object.getBase()->GetPtr())).lock();
			object = ObjectVariant(metaObject.get());
		}
		else
		{
			if (!CheckIsDerivedFromMetaObject(object.GetType()))
				return;
		}

		//先找出所有的 std::shared_ptr<meta::object> 或者 std::weak_ptr<meta::object>
		auto type = object.GetType();

		bool haveAdded = false;
		for (const auto& item : indexToObject)
		{
			if (item.second == object.getBase()->GetPtr())
			{
				haveAdded = true;
				break;
			}
		}
		if (!haveAdded)
		{
			++currentIndex;
			meta::Object* insertObject = static_cast<meta::Object*>(object.getBase()->GetPtr());
			indexToObject.insert({ currentIndex, insertObject });
		}
		if (isWeakPtr)
		{
			return;
		}

		meta::Object* metaObject = static_cast<meta::Object*>(object.getBase()->GetPtr());
		meta::Variant instance = ObjectVariant(metaObject);
		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;
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
						collisionObjects(item, indexToObject, currentIndex);
					}
				}
				else
				{
					meta::Variant value = field.GetValue(instance);
					if (type.IsSharedPtr())
					{
						std::shared_ptr<meta::Object> metaObject = *static_cast<std::shared_ptr<meta::Object>*>(value.getBase()->GetPtr());
						//object = ObjectVariant(metaObject.get());
						if(metaObject != nullptr)
							collisionObjects(value, indexToObject, currentIndex);
					}
					else if (type.IsWeakPtr())
					{
						std::shared_ptr<meta::Object> metaObject = (*static_cast<std::weak_ptr<meta::Object>*>(value.getBase()->GetPtr())).lock();
						if(metaObject != nullptr)
							collisionObjects(value, indexToObject, currentIndex);
					}
				}
			}
		}
	}

	meta::Variant TransactionManager::convertJsonValueToVariantValue(meta::Type& type, nlohmann::json diff)
	{
		meta::Type fieldType = type;
		meta::Variant fieldValue;
		if (fieldType.IsPrimitive())
		{
			if (fieldType == typeof(int))
				fieldValue = meta::Variant(diff.get<int>());
			else if (fieldType == typeof(unsigned int))
				fieldValue = meta::Variant(diff.get<unsigned int>());
			else if (fieldType == typeof(bool))
				fieldValue = meta::Variant(diff.get<bool>());
			else if (fieldType == typeof(float))
				fieldValue = meta::Variant(diff.get<float>());
			else if (fieldType == typeof(double))
				fieldValue = meta::Variant(diff.get<double>());
			else if (fieldType == typeof(uint8_t))
				fieldValue = meta::Variant(diff.get<uint8_t>());
		}
		return fieldValue;
	}

}