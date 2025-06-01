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
	private:
		std::stack<Transaction> m_undoStack;
		std::stack<Transaction> m_redoStack;
		
		Transaction m_currentTransaction;

		bool m_bInTransaction = false;
	};
}