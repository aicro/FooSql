/// @file DbExecuteRslt.h
/// @brief The file defines a class to hold the result of any Execute operation.

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DBEXECUTERSLT_H_
#define COMMON_DBCOMM_DBEXECUTERSLT_H_

#include "dbcomm/CommDef.h"
#include "dbcomm/DbRslt.h"

namespace COMMON
{
    namespace DBCOMM
    {
        class DbAction;

        /// @brief The class representing the holder of the result of the execute.
        class DbExecuteRslt : public DbRslt
        {
        public:
            /// @brief Constructor
            /// @param action The @c DbAction from which this result comes from.
            /// @param affectedRows the affected rows of the last execute operation
            DbExecuteRslt(tr1::shared_ptr<DbAction> action, map<DbLocation, long long>& affectedRows);

        public:
            /// @brief Get the number of affected rows of the last execution
            /// @param success success or not
            /// @return a map for connections and their associated number of affected rows
            virtual map<DbLocation, long long> GetAffectedRows(bool& success);
            
            /// @brief Get the number of affected rows of the last execution from a specific connection
            /// @param location the specific connection to which to get the number of affected rows
			/// @param success success or not
            /// @return the number of affected rows to that specific connection
            virtual long long GetAffectedRows(DbLocation* location, bool& success) throw (COMMON::EXCEPTION::ThrowableException);
            
            /// @brief Get the number of affected rows of the last execution from some specific connection
            /// @param locations the specific connection to which to get the number of affected rows
			/// @param success success or not
            /// @return the number of affected rows to those specific connection
            virtual map<DbLocation, long long> GetAffectedRows(vector<const DbLocation*>& locations, bool& success) throw (COMMON::EXCEPTION::ThrowableException);

        protected:
            map<DbLocation, long long> affected_rows_;
        };
        
        /// @brief The result holder for DELETE
        class DbDeleteRslt : public DbExecuteRslt {};

        /// @brief The result holder for UPDATE
        class DbUpdateRslt : public DbExecuteRslt {};

        /// @brief The result holder for TRUNCATE
        class DbTruncateRslt : public DbExecuteRslt {};

        /// @brief The result holder for INSERT
        class DbInsertRslt : public DbExecuteRslt {};
        
        /// @brief The result holder for INSERT-UPDATE
        class DbInsertUpdateRslt : public DbExecuteRslt {};
    }
}

#endif