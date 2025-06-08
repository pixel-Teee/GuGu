#pragma once

#include "Transaction.h"
#include <stack>

namespace GuGu {
	namespace meta {
		class Object;
	}
	class TransactionManager {
	public:
		TransactionManager();

		~TransactionManager();

		static TransactionManager& getTransactionManager();

		void beginTransaction();

		void commit();

		void undo();

		void redo();

		bool isInTransaction() const { return m_bInTransaction; }

		bool canUndo() const { return !m_undoStack.empty() && !m_bInTransaction; }

		bool canRedo() const { return !m_redoStack.empty() && !m_bInTransaction; }

		Transaction& getCurrentTransaction();

		void modifyObject(std::shared_ptr<meta::Object> inObject);

		void rollback();

		void preDiff(std::shared_ptr<meta::Object> inObject, const nlohmann::json& diff);

		void applyDiff(std::shared_ptr<meta::Object> inObject, const nlohmann::json& diff);

		void applyFieldDiff(std::shared_ptr<meta::Object> inObject, meta::Field& field, const nlohmann::json& diff);

		void serializeJson(std::shared_ptr<meta::Object> inObject, bool isBeforeState = true);

		struct SerializeDeserializeContext
		{
			uint32_t index = 0;//用于序列化的时候，给指针类型赋予一个索引
			std::unordered_map<uint32_t, meta::Object*> m_indexToObject;

			std::unordered_map<uint32_t, std::shared_ptr<meta::Object>> m_indexToSharedPtrObject;
		};

		nlohmann::json serializeJson(meta::Type type, const meta::Variant& instance, SerializeDeserializeContext& context);

		nlohmann::json getDiffJson(const nlohmann::json& lhs, const nlohmann::json& rhs);

		void updateObject(std::shared_ptr<meta::Object> inObject, const nlohmann::json& diffJson);
	private:

		void collisionObjects(meta::Variant& object, SerializeDeserializeContext& context);

		meta::Variant convertJsonValueToVariantValue(meta::Type& type, nlohmann::json diff);
		std::stack<Transaction> m_undoStack;
		std::stack<Transaction> m_redoStack;
		
		Transaction m_currentTransaction;

		bool m_bInTransaction = false;
	};
}