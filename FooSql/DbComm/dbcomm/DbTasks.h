/// @file DbTasks.h
/// @brief The file implements a class that implements some major methods of the @c IDbTasks interfaces.

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DBTASKS_H_
#define COMMON_DBCOMM_DBTASKS_H_

#include <memory>
#include <vector>

#include "dbcomm/CommDef.h"

#include "dbcomm/IDbTasks.h"
#include "dbcomm/DbAction.h"
#include "dbcomm/DbEngine.h"

namespace COMMON
{
    namespace DBCOMM
    {
        class DbAction;
        class DbQueryAction;
        class DbExecuteAction;
        class DbEngine;

        /// @brief The class implements some major methods of the @c IDbTasks interfaces.
        class DbTasks : public IDbTasks
        {
        protected:
            // is the action it generates finished
            bool is_action_finished_;

            // the current action 
            tr1::shared_ptr<DbAction> current_work_;

            // true if using the C++ exception, false using the C return code
            bool exception_;

            // the error box 
            COMMON::EXCEPTION::ErrorBox error_box_;

            // is connected?
            bool is_connected_;
            
            // is engine initialized?
            bool is_engine_initialized_;

            // database engine
            tr1::shared_ptr<DbEngine> db_engine_;

            // connections 
            vector<DbLocation> db_locations_;

        public:
            /// @brief Constructor
            /// @param dbLocations the database informations to the connections
            /// @param exception using the C++ exception mode?
            ///   - true C++ exception mode
            ///   - false the C style return code
            DbTasks(vector<DbLocation>& dbLocations, bool exception = false);
            
            virtual ~DbTasks() throw ();

            virtual bool Connect() throw (COMMON::EXCEPTION::ThrowableException);
            
            virtual bool Disconnect() throw (COMMON::EXCEPTION::ThrowableException);

            virtual DbQueryAction*   Select();
            
            virtual DbExecuteAction* Insert( int commitLimit = 5000 );
            
            virtual DbExecuteAction* BatchInsert( int commitLimit = 5000, int valuesLimit = 10);

            virtual DbExecuteAction* Delete( int commitLimit = 5000 );
            
            virtual DbExecuteAction* Truncate();
            
            virtual DbExecuteAction* Update( int commitLimit = 5000 );
            
            virtual DbExecuteAction* Execute();
            
            virtual DbExecuteAction* EscapeString();
            
            virtual vector<DbLocation>& GetDbLocations();

            virtual string GetLastError() { return error_box_.GetLastError(); }
            
            virtual void SetExceptions(tr1::shared_ptr<COMMON::EXCEPTION::ThrowableException> exception)
            { return error_box_.SetException(exception); }

            virtual bool SetExceptionMode(bool exception_mode)
            { bool old = exception_; exception_ = exception_mode; return old; }

            virtual bool IsExceptionMode() { return exception_; }
         
            virtual COMMON::EXCEPTION::ErrorBox* GetErrorBox() { return &error_box_; }

        protected:
            bool CanStartAction();
            
            virtual bool InitEngine() = 0;
            virtual bool UninitEngine() = 0;
        };
    }
}


#endif