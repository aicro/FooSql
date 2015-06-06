/// @file DbEngine.h
/// @brief The file defines some DB operation methods for the concrete classes to implement

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DBENGINE_H_
#define COMMON_DBCOMM_DBENGINE_H_

#include <string.h>
#include <map>
#include <vector>
#include <tr1/memory>

#include "dbcomm/CommDef.h"
#include "dbcomm/DbLocation.h"
#include "dbcomm/DbActionFilter.h"

#include "exception/IException.h"
#include "exception/ThrowableException.h"

#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/BlockingQueue.h"
#include "thread/Thread.h"

using namespace std;
using namespace COMMON::THREAD;
using namespace COMMON::EXCEPTION;

namespace COMMON
{
    namespace DBCOMM
    {
        class IDbTasks;

        /// @brief For INNER USE ONLY.
		/// This is the class that helps to records current affected rows, and take
		/// an appropreate way to judge whether we should do a commit or not.
        class CommitJudger
        {
        protected:
            /// @brief the limit for affected rows before a commit
            int commit_limit_; 

        private:
            /// @brief times to reach commit limit.
            unsigned int commit_limit_reach_times_;
            
        public:
            /// @brief Constructor.
            /// @param commitLimit the limit for affected rows before a commit
            CommitJudger(int commitLimit)
                : commit_limit_reach_times_(0), commit_limit_(commitLimit)
            {
            }
            
            virtual ~CommitJudger() {}
        
            /// @brief Judge whether we should do a commit or not.
            /// @param affectedRows affected rows currently
            /// @return whether we should do a commit
            ///   - true Do a commit 
            ///   - false Do not need a commitment
            virtual bool CanDoCommit(long long affectedRows)
            {
                if (commit_limit_ == 0)
                {
                    return false;
                }

				// the default way is to reach the commit_limit_
                bool do_commit = false;

                unsigned int times = affectedRows / commit_limit_;
                if (commit_limit_reach_times_ < times)
                {
                    commit_limit_reach_times_ = times;
                    do_commit = true;
                }
                
                return do_commit;
            }
        };
        
		/// @brief INNER USE ONLY. A container for the number of affected rows and the judge strategy.
        struct AffectedRowRecorder
        {
            /// @brief Commit strategy
            tr1::shared_ptr<CommitJudger> commit_judger_;
            
            /// @brief the number of rows affected
            long long already_affected_rows_;
        };
        
        class DbEngine;
		
		/// @brief INNER USE ONLY. The parameter for thread to start.
        struct ThreadStartParam
        {
            DbLocation location_;
            DbEngine* db_engine_;
        };
		
        /// @brief The class which defines all the necessary methods for the concrete DBPMs to implements.
		/// At same time, it also implements the basic running model based on production-consumption model. 
		/// 
		/// All the concrete classes must implements all the simple methods showed here, and override the RealHandle 
		/// according to their own underneath HANDLE.
		class DbEngine : public tr1::enable_shared_from_this<DbEngine>
        {
        public:
            /// @brief The type of commands, which has formed a set of signal for outside component to communicate
            class ActionTypeDef
            {
            public:
                /// @brief CONNECT commands
                static ActionType_C CONNECT                         ;    
                /// @brief DISCONNECT commands
                static ActionType_C DISCONNECT                      ;

                /// @brief QUERY commands
                static ActionType_C QUERY                           ;
                /// @brief FETCH a row from the open result set
                static ActionType_C FETCH                           ;
                /// @brief Get lengths for all the columns affected
                static ActionType_C GET_COLUMNS_LENGTHS             ;
                /// @brief Close an open result set
                static ActionType_C CLOSE_OPEN_RSLT                 ;
                /// @brief Get affected rows from the last operation
                static ActionType_C GET_AFFECTED_ROWS               ;

                /// @brief DELETE commands
                static ActionType_C DELETE                          ;
                /// @brief UPDATE commands
                static ActionType_C UPDATE                          ;
                /// @brief INSERT commands
                static ActionType_C INSERT                          ;
                /// @brief RESERVER only
                static ActionType_C EXECUTE_ON_EXCEPTION            ;
                /// @brief TRUNCATE commands
                static ActionType_C TRUNC                           ;
                /// @brief COMMON EXECUTE
                static ActionType_C EXECUTE                         ;
                
                /// @brief COMMIT commands
                static ActionType_C COMMIT                          ;
                /// @brief ESCAPE STRING commands
                static ActionType_C ENCODE_TO_ESCAPED_STRING        ;
                /// @brief Get ESCAPED STRING commands
                static ActionType_C GET_ESCAPED_STRING              ;
                
                /// @brief EMPTY commands
                static ActionType_C NOTHING                         ;
                /// @brief END THREAD commands
                static ActionType_C END_THREAD                      ;
            };
            
        protected:
            /// @brief The commands to transfer to the working threads, including action type, commands and connection.
            class InputCommand
            {
            public:
                /// @brief Constructor
                /// @param action The real action to do 
                InputCommand(ActionType_C action = ActionTypeDef::NOTHING) 
                    : action_(action)
                { 
                    already_affected_rows_ = 0;
                    commit_judger_ = 0;
                }
                
            public:
                /// @brief Action type
                ActionType  action_;
                
                /// @brief Affected rows
                long long* already_affected_rows_;
                
                /// @brief The way to judge whether it is the time to do commit
                CommitJudger* commit_judger_;
                
                /// @brief The filter to convey real commands
                DbActionFilter* filter_;        
				
                /// @brief The DB location info which delegates the connection information
                DbLocation  location_;
            };
            
            /// @brief The handle wrapper
            class RealHandle
            {
            public:
                RealHandle() { memset(inner_buf_, '\0', 4096 + 1); }
                virtual ~RealHandle() { }
                
				// extra buffer for additional information to convert to the working thread
                char inner_buf_[4096 + 1];
            };

        protected:
            /// @brief The @c IDbTasks pointer to which that has created the instance
            tr1::weak_ptr<IDbTasks>                          task_;
            
            /// @brief The buffer for tasks input.
            map<DbLocation, InputCommand* >    works_;
            
        private:
            // handles for all the working threads
            vector<tr1::shared_ptr<Thread> >  threads_;

            // the parameters for thread to start
            ThreadStartParam* thread_start_params_;
            
            // Mutex and Conditions for getting results from working threads
            Mutex       mutex_;
            Condition   result_reached_cond_;

        protected:
            /// @brief A blocking queue for communication between leader thread and working threads
            map<DbLocation, BlockingQueue<InputCommand* > > input_blocking_queues_;

            /// @brief A wrapper for results from working threads
            struct ReturnParam
            {
                /// @brief exceptions happened
                tr1::shared_ptr<IException> exception;
                
                /// @brief items to return 
                void* return_items_; 
                
                /// @brief the DB information that represents connection
                DbLocation* location;
            };
            
            /// @brief A queue for output results
            vector<ReturnParam> thread_return_param_;

        public:
            /// @brief Constructor
            /// @param locations a list of DB information to connect
            /// @param task a pointer to the @c IDbTasks that has created this instance
            DbEngine(vector<DbLocation>& locations, tr1::shared_ptr<IDbTasks> task);
            
            virtual ~DbEngine();

            /// @brief Asynchronise do the tasks input. This method will do the input commands for 
			/// all the connected DB illustrated in the constructor.
            /// @param actionType The action type
            /// @param filter the filter for carrying commands
            /// @param success output parameter, indicating whether the methods is success or not
            /// @param alreadyAffectedRows optional. If it is not 0, the number of affected rows will
			/// be added to it.
			/// @return A map for affected connections and their associate results from the tasks
            virtual map<DbLocation*, void*> Do(
                    ActionType_C actionType, 
                    DbActionFilter* filter, 
                    bool & success, 
                    map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows = 0) throw (ThrowableException);

            /// @brief Asynchronise do the tasks input. This method will do the input commands for 
			/// a specific connected DB.
            /// @param actionType The action type
			/// @param location the DB information illustrating the connection to be executed on
            /// @param filter the filter for carrying commands
            /// @param success output parameter, indicating whether the methods is success or not
            /// @param alreadyAffectedRows optional. If it is not 0, the number of affected rows will
			/// be added to it.
			/// @return results from the tasks
            virtual void* Do(
                    ActionType_C actionType, 
                    const DbLocation* location, 
                    DbActionFilter* filter, 
                    bool & success,
                    map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows = 0) throw (ThrowableException);

            /// @brief Asynchronise do the tasks input. This method will do the input commands for 
			/// a range of specific connected DBs.
            /// @param actionType The action type
			/// @param locFilter the DB information, which illustrate the connection, and their associated commands to be executed
            /// @param success output parameter, indicating whether the methods is success or not
            /// @param alreadyAffectedRows optional. If it is not 0, the number of affected rows will
			/// be added to it.
			/// @return A map for affected connections and their associate results from the tasks
            virtual map<DbLocation*, void*> Do(
                    ActionType_C actionType, 
                    map<DbLocation, DbActionFilter*>& locFilter, 
                    bool & success,
                    map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows = 0) throw (ThrowableException);

            /// @brief Synchronise do the tasks input. This method will do the input commands for 
			/// all the connected DB illustrated in the constructor.
            /// @param actionType The action type
            /// @param filter the filter for carrying commands
            /// @param success output parameter, indicating whether the methods is success or not
            /// @param alreadyAffectedRows optional. If it is not 0, the number of affected rows will
			/// be added to it.
			/// @return A map for affected connections and their associate results from the tasks
            virtual map<DbLocation*, void*> SyncDo(
                    ActionType_C actionType, 
                    DbActionFilter* filter, 
                    bool & success, 
                    map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows = 0) throw (ThrowableException);

            /// @brief Synchronise do the tasks input. This method will do the input commands for 
			/// a specific connected DB.
            /// @param actionType The action type
			/// @param location the DB information illustrating the connection to be executed on
            /// @param filter the filter for carrying commands
            /// @param success output parameter, indicating whether the methods is success or not
            /// @param alreadyAffectedRows optional. If it is not 0, the number of affected rows will
			/// be added to it.
			/// @return results from the tasks
            virtual void* SyncDo(
                    ActionType_C actionType, 
                    const DbLocation* location, 
                    DbActionFilter* filter, 
                    bool & success,
                    map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows = 0) throw (ThrowableException);
                    
            /// @brief Synchronise do the tasks input. This method will do the input commands for 
			/// a range of specific connected DBs.
            /// @param actionType The action type
			/// @param locFilter the DB information, which illustrate the connection, and their associated commands to be executed
            /// @param success output parameter, indicating whether the methods is success or not
            /// @param alreadyAffectedRows optional. If it is not 0, the number of affected rows will
			/// be added to it.
			/// @return A map for affected connections and their associate results from the tasks
            virtual map<DbLocation*, void*> SyncDo(
                    ActionType_C actionType, 
                    map<DbLocation, DbActionFilter*>& locFilter, 
                    bool & success,
                    map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows = 0) throw (ThrowableException);                    
                    
            // Run the threads
            void RunThread(DbLocation& location);

            /// @brief Initialize the engine
            /// @return success or not
            ///   - true success
            ///   - false failure
            virtual bool InitEngine();

            /// @brief Uninitialize the engine
            /// @return success or not
            ///   - true success
            ///   - false failure
            virtual bool UninitEngine();
           
            /// @brief Get connected DB locations
            /// @return connected DB locations
            vector<const DbLocation*> GetDbLocations();

        private:
            bool CheckHasException() throw (ThrowableException);

            void PushWorkAndWait( vector<InputCommand*>& workList );

            map<DbLocation*, void*> GetRslt();

            void ClearRslts();

            // a helper methods to do commands, check exceptions and get results
            map<DbLocation*, void*> AsyncDoCheckGetRslt( 
                vector<InputCommand*>& work_list, 
                bool &success );

        protected:
            /// @brief Create tasks that can be processed by working threads from the raw commands
            /// @param actionType the action type
            /// @param locFilter the DB information, which illustrate the connection, and their associated commands to be executed
            /// @param affectedRows a map for buffer to record how many rows are affected by each commands for each connection
            /// @param rslt that can be processed by working threads
            void CreateWorks(
                ActionType_C actionType, 
                map<DbLocation, DbActionFilter*>& locFilter,
                map<DbLocation, AffectedRowRecorder>* affectedRows,
				vector<InputCommand*>& rslt);

            /// @brief Create tasks that can be processed by working threads from the raw commands
            /// @param actionType the action type
			/// @param location the connection to execute the commands
            /// @param filter associated commands to be executed
            /// @param affectedRows a map for buffer to record how many rows are affected by each commands for each connection
            /// @param rslt that can be processed by working threads
              
            void CreateWorks(
                ActionType_C actionType, 
                const DbLocation* location, 
                DbActionFilter* filter, 
                map<DbLocation, AffectedRowRecorder>* affectedRows,
                InputCommand*& rslt);

            /// @brief Create tasks that can be processed by working threads from the raw commands
            /// @param actionType the action type
            /// @param filter associated commands to be executed on all connections
            /// @param affectedRows a map for buffer to record how many rows are affected by each commands for each connection
            /// @param rslt that can be processed by working threads
            void CreateWorks(
                ActionType_C actionType, 
                DbActionFilter* filter, 
                map<DbLocation, AffectedRowRecorder>* affectedRows,
                vector<InputCommand* >& rslt);

            /// @brief 初始化线程，该函数会在线程的初始阶段被调用
            virtual void InitThread() {}    
                
            /// @brief Uninitialize the threads, this function will be run at the last step of the thread
            virtual void UninitThread() {}
            
            /// @brief Get related handles
			/// @param location the connection that handle belongs to
			/// @return that handle belongs to the specific connection
            virtual tr1::shared_ptr<RealHandle> GetRealHandle(DbLocation& location) = 0;
            
			// a helper method to do the work
            DbEngine::ReturnParam RealDo(RealHandle* realHandle, InputCommand* inputParam);  
            
        protected:
            // Some operations to be implemented by the DBMS
			
            /// @brief Connect to the DB
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param exception output parameter. It is the exception that may occur in the operation
            /// @return success or not
            virtual bool Connect(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw () = 0;
            
            /// @brief Disconnect from the DB
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param exception output parameter. It is the exception that may be occur in the operation
			/// @return success or not
            virtual bool Disconnect(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()  = 0;

            // Query
			
            /// @brief Query, open the result set and other operations necessary
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param statement the buffer for query statement to perform
            /// @param length the length of the query buffer
            /// @param exception output parameter. It is the exception that may be occur in the operation
			/// @return success or not
            virtual bool            Query(void* handle, DbLocation* location, const char* statement, size_t length, map<string, int>* colIndexMap, tr1::shared_ptr<IException>& exception) throw ()  = 0;

            /// @brief Fetch a row from an open result set
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param exception output parameter. It is the exception that may be occur in the operation
			/// @return the contents of the current row. Note that all the contents returned are in string.
            virtual char**          Fetch(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()  = 0;
            
            /// @brief Close an open result set
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return success or not
            virtual bool            CloseOpenRslt(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()  = 0;
            
            /// @brief Get the real lengths of each column of current row
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return The real lengths of each column of current row
            virtual unsigned long*  GetColumnsActureLength(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()  = 0;

            /// @brief Commit
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return success or not
            virtual bool            Commit(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()  = 0;
            
            /// @brief Get the number of affected rows of last operation
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return the number of affected rows of last operation
            virtual long long       GetAffectedRows(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()  = 0;
            
            // Execute
            
            /// @brief Delete
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param statement the buffer for delete statement to perform
            /// @param length the length of the delete buffer
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return the number of affected rows
            virtual long long       Delete(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()  = 0;
            
            /// @brief Update
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param statement the buffer for update statement to perform
            /// @param length the length of the buffer
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return the number of affected rows
            virtual long long       Update(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()  = 0;
            
            /// @brief Truncate 
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param statement the buffer for truncate statement to perform
            /// @param length the length of the buffer
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return the number of affected rows
            virtual long long       Truncate(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()  = 0;

            /// @brief Insert
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param statement the buffer for query statement to perform
            /// @param length the length of the buffer
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return the number of affected rows
            virtual long long       Insert(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()  = 0;

            /// @brief Common execute
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param statement the buffer for any statement to perform
            /// @param length the length of the buffer
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return The real error code
            virtual unsigned int     Execute(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()  = 0;
            
            // Other 
            
            /// @brief Escape a string
            /// @param handle handle for the connection
            /// @param location DB location representing the connection
            /// @param src the buffer for string to be escape
            /// @param length the length of the buffer
            /// @param exception output parameter. It is the exception that may be occur in the operation
            /// @return the result escaped string, terminated by '\0'
            virtual char*   EscapeString(void* handle, DbLocation* location, const char* src, long length, tr1::shared_ptr<IException>& exception) throw ()  = 0;
        };
    }
}

#endif
