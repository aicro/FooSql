/// @file DbExecuteAction.h
/// @brief The file defines an action for Execute, including Insert, Update, Delete and other non-query operation.

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DBEXECUTEACTION_H_
#define COMMON_DBCOMM_DBEXECUTEACTION_H_

#include "dbcomm/DbAction.h"

namespace COMMON
{
    namespace DBCOMM
    {
        struct AffectedRowRecorder;

        /// @brief The action for EXECUTE
        class DbExecuteAction : public DbAction
        {
        protected:
            /// @brief the affected rows of different connections
            map<DbLocation, AffectedRowRecorder> already_affected_rows_;

        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            DbExecuteAction(tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished, int timesToCommit = 5000);

            virtual ~DbExecuteAction();
        
            virtual DbRslt* GetRslt();

            virtual bool Do(DbActionFilter* filter, map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            virtual bool Do(DbActionFilter* filter, DbLocation* location, long long* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);
            
            virtual bool Do(map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows = 0) throw (EXCEPTION::ThrowableException);

        protected:
            /// @brief Get the current action type. 
			/// This is the methods to be inherited by expanding class to illustrate 
			/// what the action really do.
            /// @return The type of action the class really do.
            virtual ActionType_C GetRealActionType() { return DbEngine::ActionTypeDef::EXECUTE; }

        protected:
            virtual void GetAffectedRows(map<DbLocation*, void*>& workRslt, map<DbLocation, long long>* affected_rows);
        };

        /// @brief The action for DELETE operation
        class DbDeleteAction : public DbExecuteAction
        {
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            DbDeleteAction(tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished, int timesToCommit = 5000)
                : DbExecuteAction(dbtasks, engine, isActionFinished, timesToCommit) {}

        protected:
            virtual ActionType_C GetRealActionType() { return DbEngine::ActionTypeDef::DELETE; }
        };


        /// @brief The action for UPDATE operation
        class DbUpdateAction : public DbExecuteAction
        {
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            DbUpdateAction(tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished, int timesToCommit = 5000)
                : DbExecuteAction(dbtasks, engine, isActionFinished, timesToCommit) {}

        protected:
            virtual ActionType_C GetRealActionType() { return DbEngine::ActionTypeDef::UPDATE; }
        };

        /// @brief The action for INSERT operation
		class DbInsertAction : public DbExecuteAction
        {
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            DbInsertAction(tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished, int timesToCommit = 5000)
                : DbExecuteAction(dbtasks, engine, isActionFinished, timesToCommit) {}

        protected:
            virtual ActionType_C GetRealActionType() { return DbEngine::ActionTypeDef::INSERT; }
        };

		/// @brief The action for TRUNCATE operation
        class DbTruncateAction : public DbExecuteAction
        {
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
			/// @param timesToCommit This parameter indicates the limit of uncommited affected rows. If it has been reached, we should send a commit statement to the server
            DbTruncateAction(tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished)
                : DbExecuteAction(dbtasks, engine, isActionFinished, 1) {}

            virtual ActionType_C GetRealActionType() { return DbEngine::ActionTypeDef::TRUNC; }
        };
    }
}

#endif