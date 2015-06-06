/// @file DB2SpecialAction.h
/// @brief The file defines some special action for DB2

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DB2_SPECIAL_ACTION_H_
#define COMMON_DBCOMM_DB2_SPECIAL_ACTION_H_

#ifdef DB2_ENV_AVAILABLE

#include "dbcomm/DbBatchAction.h"
#include "dbcomm/DB2StmtGen.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief A INSERT-ON-DUPLICATE-PRI-KEY-UPDATE action for DB2
        class DB2ReplaceBatchAction : public DbBatchAction
        {
        public:
            /// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param valuesPerBatch This parameter indicates the limit of buffered values. If it has been reached, we should send a full statement to the server
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            DB2ReplaceBatchAction(            
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
                    elems_[allDb[i]].reset(new DB2ReplaceStmtGen());
                }
            }

            virtual ~DB2ReplaceBatchAction() {}
        };   
        
        /// @brief 适用于DB2做查找主键的动作
        class DB2GetPriKeysAction : public DbGetPriKeysAction
        {
        public:
            /// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			DB2GetPriKeysAction(            
                tr1::shared_ptr<IDbTasks> dbtasks, 
                tr1::shared_ptr<DbEngine> engine, 
                bool& isActionFinished)
                : DbGetPriKeysAction(
                    dbtasks, engine, isActionFinished)
            {
                vector<DbLocation>& allDb = task_.lock()->GetDbLocations();
                for (int i = 0; i < allDb.size(); i++)
                {
                    stmt_gen_[allDb[i]].reset(new DB2GetPriKeyStmtGen());
                }
            }

            virtual ~DB2GetPriKeysAction() {}
        };
        
        /// @brief 适用于DB2的批量INSERT-IGNORE动作
        class DB2InsertIgnoreBatchAction : public DbBatchAction
        {
        public:
            /// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param valuesPerBatch This parameter indicates the limit of buffered values. If it has been reached, we should send a full statement to the server
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
			DB2InsertIgnoreBatchAction(            
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
                    elems_[allDb[i]].reset(new DB2InsertIgnoreStmtGen());
                }
            }

            virtual ~DB2InsertIgnoreBatchAction() {}
        }; 
    }
}

#endif

#endif