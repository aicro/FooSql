/// @file EscapeStringAction.h
/// @brief The file defines an action for escaping string

/// @author Aicro Ai


#ifndef COMMON_DBCOMM_ESCAPE_STRING_ACTION_H_
#define COMMON_DBCOMM_ESCAPE_STRING_ACTION_H_

#include "dbcomm/DbEngine.h"
#include "dbcomm/DbQueryRslt.h"
#include "dbcomm/DbActionFilter.h"
#include "exception/IException.h"

#include "dbcomm/DbExecuteAction.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief The class representing the action for escaping string
        class EscapeStringAction : public DbExecuteAction
        {
        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
            EscapeStringAction(
                tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished);

            virtual ActionType_C GetRealActionType();

            virtual bool Do(DbActionFilter* filter, map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            virtual bool Do(DbActionFilter* filter, DbLocation* location, long long* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);
            
            virtual bool Do(map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            virtual DbRslt* GetRslt();
			
			virtual bool EndAction() throw (COMMON::EXCEPTION::ThrowableException);

		public:
            /// @brief Get the escaped string.
			/// @return the escaped string.
            /// @note  If there is more than one connection, the return order is random.
            string GetEscapedString();
            
            /// @brief Get the escaped string from the specific connection.
			/// @return the escaped string.
            string GetEscapedString(DbLocation& location);

        protected:
            void PrepareEscapedString(map<DbLocation*, void*>& workRslt);
            
        private:
            map<DbLocation, int> escaped_strings_index_;
            vector<string> escaped_strings_;
            
            int current_escaped_string_index_;
        };
    }
}

#endif