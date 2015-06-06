/// @file DbAction.h
/// @brief The file defines the base class for Action, which is one of the most
/// important component of the frame. It is a proxy for DB operation by entailing 
/// the @c DbEngine to do the real operation.

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DBACTION_H_
#define COMMON_DBCOMM_DBACTION_H_

#include <string>

#include "dbcomm/CommDef.h"

#include "dbcomm/DbEngine.h"
#include "dbcomm/DbActionFilter.h"
#include "dbcomm/DbRslt.h"

#include "exception/ErrorBox.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        class IDbTasks;
        class DbEngine;
        class DbRslt;

		/// @breif The base class for other ACTION.
        class DbAction : public tr1::enable_shared_from_this<DbAction>
        {
        protected:
            /// @brief the @c IDbTasks for which the action belongs to
            tr1::weak_ptr<IDbTasks> task_;

            /// @brief the @c DbEngine that helps to do the real operation
            tr1::shared_ptr<DbEngine> engine_;

            /// @brief a signal indicating whether the current action is finished
            bool& is_action_finished_;

            /// @brief the result of the current action
            tr1::shared_ptr<DbRslt> action_rslt_;

            /// @brief a mapping indicating which connections have been operated. 
			/// @note When the mapping is empty meaning all the connections have done something.
            map<DbLocation, DbActionFilter*> actioned_db_info_;

        public:
			/// @brief Constructor
			/// @param dbtasks a pointer to a @c DbTasks instance that generate the action
            /// @param engine the real engine instance underline
            /// @param isActionFinished a signal to inform the @c DbTasks instance, the parent of this action, whether the action is finished
            explicit DbAction(tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& isActionFinished);

            /// @brief The deconstructor.
            virtual ~DbAction();

			/// @brief Drive the underneath @c DbEngine to do the tasks defined by the filter toward 
			/// all the connected DBs. 
			/// @param filter the container to hold the details of the tasks.
			/// @param affectedRows an optional output parameter to hold the number of affected rows from each connection
			/// @return success or not
            virtual bool Do(DbActionFilter* filter, map<DbLocation, long long>* affectedRows = 0) throw (COMMON::EXCEPTION::ThrowableException);

			/// @brief Drive the underneath @c DbEngine to do the tasks defined by the filter toward 
			/// a specific connected DBs. 
			/// @param filter the container to hold the details of the tasks.
			/// @param location the specific DB location for the task to carry out. This must belongs to one of the connections.
			/// @param affectedRows an optional output parameter to hold the number of affected rows
			/// @return success or not
            virtual bool Do(DbActionFilter* filter, DbLocation* location, long long* affectedRows = 0) throw (COMMON::EXCEPTION::ThrowableException);

			/// @brief Drive the underneath @c DbEngine to do the tasks defined by the filter toward 
			/// specific connected DBs. 
			/// @param works A mapping of connections and their corresponded tasks to do.
			/// @param affectedRows an optional output parameter to hold the number of affected rows for each connections
			/// @return success or not            
			virtual bool Do(
                map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affectedRows = 0) throw (COMMON::EXCEPTION::ThrowableException) = 0;

            /// @brief End the current action. This always will do some cleaning up things including commit the uncommited data, or free 
			/// any unreleased resources. 
			/// @param affectedRows an optional output parameter to hold the number of affected rows for each connections
			/// @note Because the deconstructor will call this method, it is not necessary to call it every time. 
			/// However, I strongly recommend you to call it by yourself to ensure the resources to be released.
            virtual bool EndAction(map<DbLocation, long long>* affectedRows = 0) throw (COMMON::EXCEPTION::ThrowableException);

            /// @brief Get the details of each underneath connections
            /// @return A list of the details of each underneath connections
            vector<DbLocation>& GetDbLocations();

			/// @brief Get the result of the last operation
            /// @return The result of the last operation
            virtual DbRslt* GetRslt() = 0;

			/// @brief Get the description of the last error
			/// @return the description of the last error
            virtual string GetLastError();

			/// @brief check whether the current exception mode is throw out an exception or just return a C style signature.
			/// @return 
            ///   - true a C++ try..catch style exception
            ///   - false a C style returning error code.
            virtual bool IsExceptionMode();

            /// @brief INTERNAL USE ONLY. Set the exception.
            /// @param e The exception to be thrown out.
            virtual void SetException(tr1::shared_ptr<COMMON::EXCEPTION::ThrowableException> e);

        protected:
            // Set the connections list for those actioned connections
            void SetActionedDbInfo(map<DbLocation, DbActionFilter*>& locFilter);
            void SetActionedDbInfo(DbLocation* location);
            void SetActionedDbInfo();

        private:
            // true means all the connections have been operated
            bool allDbActioned_;
        };
    }
}

#endif
