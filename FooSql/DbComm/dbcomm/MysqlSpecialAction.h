/// @file MysqlSpecialAction.h
/// @brief The file defines some actions that are different from traditional CRUD.

/// @author Aicro Ai
/// @date 2015/4/19

#ifndef DBOMM_MYSQL_SPECIAL_ACTION_H_
#define DBOMM_MYSQL_SPECIAL_ACTION_H_

#ifdef MYSQL_ENV_AVAILABLE

#include "dbcomm/DbBatchAction.h"
#include "dbcomm/BatchFilter.h"
#include "dbcomm/MysqlStmtGen.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief An action to generate REPLACE statement for MYSQL
        class MysqlReplaceBatchAction : public DbBatchAction
        {
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
            /// @param valuesPerBatch This parameter indicates the limit of buffered values. If it has been reached, we should send a full statement to the server
            /// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            MysqlReplaceBatchAction(            
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
                    elems_[allDb[i]].reset(new MysqlReplaceStmtGen());
                }
            }

            virtual ~MysqlReplaceBatchAction() {}
        };  

        /// @brief This class represents an action for getting primary key of a table for MYSQL
        class MysqlGetPriKeysAction : public DbGetPriKeysAction
        {
        public:
            /// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
            MysqlGetPriKeysAction(            
                tr1::shared_ptr<IDbTasks> dbtasks, 
                tr1::shared_ptr<DbEngine> engine, 
                bool& isActionFinished)
                : DbGetPriKeysAction(
                    dbtasks, engine, isActionFinished)
            {
                vector<DbLocation>& allDb = task_.lock()->GetDbLocations();
                for (int i = 0; i < allDb.size(); i++)
                {
                    stmt_gen_[allDb[i]].reset(new MysqlGetPriKeyStmtGen());
                }
            }

            virtual ~MysqlGetPriKeysAction() {}
        };
        
        /// @brief An action to generate INSERT IGNORE statement for MYSQL
        class MysqlInsertIgnoreBatchAction : public DbBatchAction
        {
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
            /// @param valuesPerBatch This parameter indicates the limit of buffered values. If it has been reached, we should send a full statement to the server
            /// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            MysqlInsertIgnoreBatchAction(            
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
                    elems_[allDb[i]].reset(new MysqlInsertIgnoreStmtGen());
                }
            }

            virtual ~MysqlInsertIgnoreBatchAction() {}
        }; 
    }
}
#endif
#endif