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
			std::shared_ptr<meta::Object> beforeObject = AssetManager::getAssetManager().deserializeJsonNormalObject(beforeStateJson);
			//implement diff and copy
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
			std::shared_ptr<meta::Object> afterObject = AssetManager::getAssetManager().deserializeJsonNormalObject(afterStateJson);
			//implement diff and copy
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
			std::shared_ptr<meta::Object> beforeObject = AssetManager::getAssetManager().deserializeJsonNormalObject(beforeStateJson);
			//implement diff and copy
		}
		m_bInTransaction = false;
		m_currentTransaction.m_beforeState.clear();
		m_currentTransaction.m_afterState.clear();
		m_currentTransaction.m_currentObjects.clear();
	}

}