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
			//get after state
			if (m_currentTransaction.m_currentObjects[i].lock())
			{
				nlohmann::json objectJson = AssetManager::getAssetManager().serializeJson(m_currentTransaction.m_currentObjects[i].lock());
				std::string objectStr = objectJson.dump();//deep copy
				m_currentTransaction.m_afterState.push_back(GuGuUtf8Str(objectStr).getBinary());
			}
			else
			{
				GuGu_LOGD("missing object");
			}		
		}

		m_undoStack.push(std::move(m_currentTransaction));

		while (!m_redoStack.empty())
		{
			m_redoStack.pop();
		}

		m_bInTransaction = false;
		m_currentTransaction.m_beforeState.clear();
		m_currentTransaction.m_afterState.clear();
		m_currentTransaction.m_currentObjects.clear();
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

		auto transaction = std::move(m_undoStack.top());
		m_undoStack.pop();

		for (size_t i = 0; i < transaction.m_beforeState.size(); ++i)
		{
			std::string beforeStateStr(reinterpret_cast<const char*>(transaction.m_beforeState[i].data()), transaction.m_beforeState[i].size());
			nlohmann::json beforeStateJson = nlohmann::json::parse(beforeStateStr.c_str());
			if (transaction.m_currentObjects[i].lock())
			{
				meta::Type objectType = transaction.m_currentObjects[i].lock()->GetType();
				std::shared_ptr<meta::Object> beforeObject = AssetManager::getAssetManager().deserializeJsonNormalObject(beforeStateJson, objectType);
				//implement diff and copy
			}
			else
			{
				GuGu_LOGE("missing object");
			}
		}

		m_redoStack.push(std::move(transaction));
	}

	void TransactionManager::redo()
	{
		if (m_bInTransaction)
		{
			GuGu_LOGE("cannot redo during a transaction");
		}

		if (m_redoStack.empty())
		{
			GuGu_LOGD("nothing to redo");
		}

		auto transaction = std::move(m_redoStack.top());
		m_redoStack.pop();

		for (size_t i = 0; i < transaction.m_afterState.size(); ++i)
		{
			std::string afterStateStr(reinterpret_cast<const char*>(transaction.m_afterState[i].data()), transaction.m_afterState[i].size());
			nlohmann::json afterStateJson = nlohmann::json::parse(afterStateStr.c_str());
			if (transaction.m_currentObjects[i].lock())
			{
				meta::Type objectType = transaction.m_currentObjects[i].lock()->GetType();
				std::shared_ptr<meta::Object> afterObject = AssetManager::getAssetManager().deserializeJsonNormalObject(afterStateJson, objectType);
				//implement diff and copy
			}
			else
			{
				GuGu_LOGE("missing object");
			}	
		}

		m_undoStack.push(std::move(transaction));
	}

	Transaction& TransactionManager::getCurrentTransaction()
	{
		return m_currentTransaction;
	}

	void TransactionManager::modifyObject(std::shared_ptr<meta::Object> inObject)
	{
		nlohmann::json objectJson = AssetManager::getAssetManager().serializeJson(inObject);
		std::string objectStr = objectJson.dump();//deep copy
		m_currentTransaction.m_beforeState.push_back(GuGuUtf8Str(objectStr).getBinary());
		m_currentTransaction.m_currentObjects.push_back(inObject);
	}

	void TransactionManager::rollback()
	{
		if (!m_bInTransaction)
		{
			GuGu_LOGE("no transaction to rollback");
		}

		for (size_t i = 0; i < m_currentTransaction.m_beforeState.size(); ++i)
		{
			std::string beforeStateStr(reinterpret_cast<const char*>(m_currentTransaction.m_beforeState[i].data()), m_currentTransaction.m_beforeState[i].size());
			nlohmann::json beforeStateJson = nlohmann::json::parse(beforeStateStr.c_str());
			if (m_currentTransaction.m_currentObjects[i].lock())
			{
				meta::Type objectType = m_currentTransaction.m_currentObjects[i].lock()->GetType();
				std::shared_ptr<meta::Object> beforeObject = AssetManager::getAssetManager().deserializeJsonNormalObject(beforeStateJson, objectType);
				//implement diff and copy
			}
			else
			{
				GuGu_LOGE("missing object");
			}
		}
		m_bInTransaction = false;
		m_currentTransaction.m_beforeState.clear();
		m_currentTransaction.m_afterState.clear();
		m_currentTransaction.m_currentObjects.clear();
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