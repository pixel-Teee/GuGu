#include <pch.h>

#include "TransactionManager.h"
#include <Core/AssetManager/AssetManager.h>

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
				serializeJson(item.first.m_object, false);
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

		auto& transaction = std::move(m_undoStack.top());
#ifdef false
		m_undoStack.pop();
#endif
		for (size_t i = 0; i < transaction.m_currentObjects.size(); ++i)
		{
			for (const auto& item : transaction.m_currentObjects[i])
			{
				//modify every object
				const TrackObject& trackObject = item.first;
				std::shared_ptr<meta::Object> object = trackObject.m_object;
				//get json to diff
				const nlohmann::json& beforeState = item.second.m_beforeState;
				const nlohmann::json& afterState = item.second.m_afterState;
				//from after to before
				nlohmann::json diffJson = getDiffJson(afterState, beforeState);
				if (!diffJson.empty())
				{
					//modify object
					updateObject(object, diffJson);
				}
			}
		}
#ifdef false
		m_redoStack.push(std::move(transaction));
#endif
	}

	void TransactionManager::redo()
	{
		//if (m_bInTransaction)
		//{
		//	GuGu_LOGE("cannot redo during a transaction");
		//}
		//
		//if (m_redoStack.empty())
		//{
		//	GuGu_LOGD("nothing to redo");
		//}
		//
		//auto transaction = std::move(m_redoStack.top());
		//m_redoStack.pop();
		//
		//for (size_t i = 0; i < transaction.m_afterState.size(); ++i)
		//{
		//	std::string afterStateStr(reinterpret_cast<const char*>(transaction.m_afterState[i].data()), transaction.m_afterState[i].size());
		//	nlohmann::json afterStateJson = nlohmann::json::parse(afterStateStr.c_str());
		//	if (transaction.m_currentObjects[i].lock())
		//	{
		//		meta::Type objectType = transaction.m_currentObjects[i].lock()->GetType();
		//		std::shared_ptr<meta::Object> afterObject = AssetManager::getAssetManager().deserializeJsonNormalObject(afterStateJson, objectType);
		//		//implement diff and copy
		//	}
		//	else
		//	{
		//		GuGu_LOGE("missing object");
		//	}	
		//}
		//
		//m_undoStack.push(std::move(transaction));
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

	void TransactionManager::serializeJson(std::shared_ptr<meta::Object> inObject, bool isBeforeState)
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
			if(findMap)
				break;
		}
		if (!findMap)
		{
			m_currentTransaction.m_currentObjects.push_back(std::map<TrackObject, ModifyState>());
			findIndex = m_currentTransaction.m_currentObjects.size() - 1;
		}

		//SerializeDeserializeContext context;
		//context.m_indexToObject.clear();
		//context.m_indexToSharedPtrObject.clear();
		//context.index = 0;

		TrackObject rootTrackObject;
		rootTrackObject.m_object = inObject;
		rootTrackObject.isRoot = true;

		int32_t maxIndex = 0;
		for (const auto& item : m_currentTransaction.m_currentObjects[findIndex][rootTrackObject].m_indexToObjects)
		{
			maxIndex = std::max(maxIndex, item.first);
		}

		meta::Variant rootObject = ObjectVariant(inObject.get());
		collisionObjects(rootObject, m_currentTransaction.m_currentObjects[findIndex][rootTrackObject].m_indexToObjects, maxIndex);

		for (const auto& item : m_currentTransaction.m_currentObjects[findIndex])
		{
			meta::Variant object = ObjectVariant(item.first.m_object.get());
			std::string objectIndex = std::to_string(item.first);
			nlohmann::json jsonObject = serializeJson(object.GetType(), object, m_currentTransaction.m_currentObjects[findIndex][rootTrackObject].m_indexToObjects);
			nlohmann::json indexAndObject = nlohmann::json::object();
			indexAndObject[objectIndex.c_str()] = jsonObject;

			TrackObject trackObject;
			trackObject.m_object = item.second->shared_from_this();
			trackObject.isRoot = (inObject == item.second->shared_from_this());

			if (isBeforeState)
			{
				m_currentTransaction.m_currentObjects[findIndex][trackObject].m_beforeState = indexAndObject;
			}
			else
			{
				m_currentTransaction.m_currentObjects[findIndex][trackObject].m_afterState = indexAndObject;
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

	void TransactionManager::updateObject(std::shared_ptr<meta::Object> inObject, const nlohmann::json& diffJson)
	{

	}

	void TransactionManager::collisionObjects(meta::Variant& object, std::map<int32_t, meta::Object*>& indexToObject, int32_t& currentIndex)
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
						collisionObjects(item, indexToObject, currentIndex);
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