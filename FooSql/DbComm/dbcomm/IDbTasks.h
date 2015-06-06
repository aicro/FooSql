/// @file IDbTasks.h
/// @brief The file defines the interface for the start point of any operation.
/// Any DB operation must start from an implement of an @c IDbTasks, which delegates 
/// a specific DBMS. Then you can use the interfaces listed here to get the Working Action
/// which may help you to do the specific work.

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_IDBTASKS_H_
#define COMMON_DBCOMM_IDBTASKS_H_

#include "CommDef.h"

#include <string>
#include <vector>

#include "dbcomm/DbQueryAction.h"
#include "dbcomm/DbQueryRslt.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        class DbExecuteAction;
        
        /// @brief The interface for any DBMS to implement.
        class IDbTasks : public tr1::enable_shared_from_this<IDbTasks>
        {
        public:
            // Connect and disconnect
            
            /// @brief Connect to a DBMS
            /// @return Success or not
            virtual bool Connect() = 0;
            
            /// @brief Disconnect from a DBMS
            /// @return Success or not
            virtual bool Disconnect() = 0;

            // Query
            
            /// @brief Get the action for a QUERY
            /// @return an action for a query
            virtual DbQueryAction*   Select() = 0;
            
            // Execute
            
            /// @brief Generate an action for INSERT
            /// @param commitLimit commit limit. If the number of affected rows have reached it, a commit will do for you.
            /// @return an action for INSERT
            virtual DbExecuteAction* Insert(int commitLimit) = 0;
            
            /// @brief Generate an action for MULTIVALUE INSERT or BATCH INSERT
            /// @param commitLimit commit limit. If the number of affected rows have reached it, a commit will do for you.
			/// @param valuesLimit the number of values reach which a complete SQL statement formed
            /// @return an action for MUTILVALUE INSERT
            virtual DbExecuteAction* BatchInsert( int commitLimit, int valuesLimit) = 0;

            /// @brief Generate an action for MULTIVALUE INSERT UPDATE or BATCH REPALCE.
			/// That is to say, if a duplicate key error occurs, we will replace the original value with the current insert value.
            /// @param commitLimit commit limit. If the number of affected rows have reached it, a commit will do for you.
			/// @param valuesLimit the number of values reach which a complete SQL statement formed
            /// @return an action for MUTILVALUE INSERT UPDATE
			/// @note The automatic update will only occurs when a duplicate key error met, that is to say this is only useful for those tables having primary keys.
            virtual DbExecuteAction* BatchReplace( int commitLimit, int valuesLimit) = 0;
            
            /// @brief Generate an action for MULTIVALUE INSERT IGNORE. That is to say, 
			/// if a duplicate key error occurs, we will skip the current insert value and leave the original one untouched.
            /// @param commitLimit commit limit. If the number of affected rows have reached it, a commit will do for you.
			/// @param valuesLimit the number of values reach which a complete SQL statement formed
            /// @return an action for MUTILVALUE INSERT UPDATE
			/// @note The automatic update will only occurs when a duplicate key error met, that is to say this is only useful for those tables having primary keys.
            virtual DbExecuteAction* BatchInsertIgnore( int commitLimit, int valuesLimit) = 0;
            
            /// @brief Generate an action for DELETE
            /// @param commitLimit commit limit. If the number of affected rows have reached it, a commit will do for you.
            /// @return an action for DELETE
            virtual DbExecuteAction* Delete(int commitLimit) = 0;
            
            /// @brief Generate an action for TRUNCATE
            /// @param commitLimit commit limit. If the number of affected rows have reached it, a commit will do for you.
            /// @return an action for TRUNCATE
            virtual DbExecuteAction* Truncate() = 0;
            
            /// @brief Generate an action for UPDATE
            /// @param commitLimit commit limit. If the number of affected rows have reached it, a commit will do for you.
            /// @return an action for UPDATE
            virtual DbExecuteAction* Update(int commitLimit) = 0;
            
            /// @brief Generate an action for EXECUTE
            /// @param commitLimit commit limit. If the number of affected rows have reached it, a commit will do for you.
            /// @return an action for EXECUTE
            virtual DbExecuteAction* Execute() = 0;

            // misc
            /// @brief Get a escape string action
            /// @return a escape string action
            virtual DbExecuteAction* EscapeString() = 0;
            
            /// @brief Get a trying to get primary keys action
            /// @return a trying to get primary keys action
            virtual DbQueryAction* GetPriKeys() = 0;
            
            /// @brief Get all connections' information
            /// @return all connections' information
            virtual vector<DbLocation>& GetDbLocations() = 0;

            /// @brief Get the error message of the last operation
            /// @return the error message of the last operation
            virtual string GetLastError() = 0;
            
            /// @brief INTERNAL USE ONLY. Set the exception
            /// @param the exception occurs
            virtual void SetExceptions(tr1::shared_ptr<COMMON::EXCEPTION::ThrowableException> exception) = 0;
            
            /// @brief INTERNAL USE ONLY. Get the error box
            /// @return the error box
            virtual COMMON::EXCEPTION::ErrorBox* GetErrorBox() = 0;

            /// @brief Set exception mode, c++ exception or c return code
            /// @param exceptionMode true means exception mode, false means c return code
            /// @return the original exception mode, true means exception mode, false means c return code
            virtual bool SetExceptionMode(bool exceptionMode) = 0;
            
            /// @brief Check whether the current exception mode is C++'s exception
            /// @return true means exception mode, false means c return code
            virtual bool IsExceptionMode() = 0;
        };
    }
}

#endif
