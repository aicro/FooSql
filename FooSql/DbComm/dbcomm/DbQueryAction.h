/// @file DbQueryAction.h
/// @brief The file defines an action for Query

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DBQYERTACTUIB_H_
#define COMMON_DBCOMM_DBQYERTACTUIB_H_

#include "dbcomm/DbEngine.h"
#include "dbcomm/DbQueryRslt.h"
#include "dbcomm/DbActionFilter.h"
#include "dbcomm/DbAction.h"
#include "dbcomm/StmtGenerator.h"

#include "exception/IException.h"

namespace COMMON
{
    namespace DBCOMM
    {
        class IDbTasks;
        class DbEngine;

        /// @brief The class representing a query action
        class DbQueryAction : public DbAction
        {
        private:
            // whether a result set has been opened
            bool is_rslt_opened_;

            // a mapping for columns' name and its position for each connections.
            map<DbLocation, map<string, int>* >  column_name_index_map_;

        public:
            /// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
            DbQueryAction(tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished);

            ~DbQueryAction();

            virtual DbRslt* GetRslt();

            virtual bool Do(DbActionFilter* filter, map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            virtual bool Do(DbActionFilter* filter, DbLocation* location, long long* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);
            
            virtual bool Do(map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            virtual bool EndAction(map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            /// @brief INTERNAL USE ONLY. Get the column name and its associated position for a specific connection
            /// @return The mapping for column name and its associated position for a specific connection
            virtual map<string, int>* GetColumnStringIndex(DbLocation& location);

        protected:
            void GetAffectedRows(map<DbLocation*, void*>* work_rslt, map<DbLocation, long long>* affected_rows);
        };
        
        /// @brief An action for getting primary keys of a table
        class DbGetPriKeysAction : public DbQueryAction
        {
        protected:
            map<DbLocation, tr1::shared_ptr<StmtGenerator> > stmt_gen_;
            
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
            DbGetPriKeysAction(
                tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished)
                : DbQueryAction(dbtasks, engine, isActionFinished) 
            {
            }

            virtual bool Do(
                map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);
        };
    }
}

#endif