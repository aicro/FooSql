/// @file DbBatchAction.h
/// @brief The file defines an action suitable for multi-value operations such as INSERT, MERGE.

/// @author Aicro Ai
/// @date 2015/4/19

#ifndef COMMON_DBCOMM_BATCHACTION_H_
#define COMMON_DBCOMM_BATCHACTION_H_

#include "dbcomm/DbExecuteAction.h"
#include "dbcomm/BatchFilter.h"
#include "dbcomm/StmtGenerator.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief The base class for batch action. It is designed according to strategy design pattern, 
		/// that it just defines some common works and framework for batch operations, while the details are left
		/// for concrete @c StatementGenerator instance.
        class DbBatchAction : public DbInsertAction
        {
        protected:
            // This parameter indicates the limit of buffered values. 
			// If it has been reached, we should send a full statement to the server
            int values_per_batch_;

            // A buffer recording already affected rows for each DB connections.
            map<DbLocation, unsigned int> values_now_;

			// A buffer storing different concrete statement generator for each DB connections.
            map<DbLocation, tr1::shared_ptr<StmtGenerator> > elems_;

        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param valuesPerBatch This parameter indicates the limit of buffered values. If it has been reached, we should send a full statement to the server
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            DbBatchAction(
                tr1::shared_ptr<IDbTasks> dbtasks, 
                tr1::shared_ptr<DbEngine> engine, 
                bool& isActionFinished, 
                int valuesPerBatch, 
                int timesToCommit);

            virtual ~DbBatchAction();

            virtual bool Do(DbActionFilter* filter, map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            virtual bool Do(DbActionFilter* filter, DbLocation* location, long long* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            virtual bool Do(map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            virtual bool EndAction(map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);
         
        private:
            // Make up the whole statement. The concrete work is done by the elem. The commands are passed by filter.
            bool MakeupStatement(const tr1::shared_ptr<StmtGenerator>& elem, BatchFilter* filter);

            // Do all the left work
            bool DoAllLeft(map<DbLocation, long long>* affected_rows = 0);
        };

        /// @brief An action for multi-value insert work.
        class BatchInsertAction : public DbBatchAction
        {
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param valuesPerBatch This parameter indicates the limit of buffered values. If it has been reached, we should send a full statement to the server
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            BatchInsertAction(            
                tr1::shared_ptr<IDbTasks> dbtasks, 
                tr1::shared_ptr<DbEngine> engine, 
                bool& isActionFinished, 
                int valuesPerBatch, 
                int timesToCommit)
                : DbBatchAction(
                    dbtasks, engine, isActionFinished, valuesPerBatch, timesToCommit)
            {
                vector<DbLocation>& allDb = task_.lock()->GetDbLocations();
                for (int i = 0; i < allDb.size(); i++)
                {
                    elems_[allDb[i]].reset(new InsertStmtGen());
                }
            }

            virtual ~BatchInsertAction() {}
        };
    }
}

#endif