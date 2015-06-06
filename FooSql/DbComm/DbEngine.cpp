#include <stdlib.h>


#include "dbcomm/DbEngine.h"
#include "dbcomm/DbTasks.h"
#include "dbcomm/DbActionFilter.h"

namespace COMMON
{
    namespace DBCOMM
    {
        //////////////////// ActionTypeDef ///////////////////////
        /* connect and disconnect */
        ActionType_C DbEngine::ActionTypeDef::CONNECT                           = 0;
        ActionType_C DbEngine::ActionTypeDef::DISCONNECT                        = 1;
                                            
        /* query related works */                  
        ActionType_C DbEngine::ActionTypeDef::QUERY                             = 2;
        ActionType_C DbEngine::ActionTypeDef::FETCH                             = 3;
        ActionType_C DbEngine::ActionTypeDef::GET_COLUMNS_LENGTHS               = 4;
        ActionType_C DbEngine::ActionTypeDef::CLOSE_OPEN_RSLT                   = 5;
        ActionType_C DbEngine::ActionTypeDef::GET_AFFECTED_ROWS                 = 6;
        
        /* execute related works */
        ActionType_C DbEngine::ActionTypeDef::DELETE                            = 7;
        ActionType_C DbEngine::ActionTypeDef::UPDATE                            = 8;
        ActionType_C DbEngine::ActionTypeDef::INSERT                            = 9;
        ActionType_C DbEngine::ActionTypeDef::EXECUTE_ON_EXCEPTION              =10;
        ActionType_C DbEngine::ActionTypeDef::TRUNC                             =11;
        ActionType_C DbEngine::ActionTypeDef::EXECUTE                           =12;
        
        /* other db operations */
        ActionType_C DbEngine::ActionTypeDef::COMMIT                            =13;
        ActionType_C DbEngine::ActionTypeDef::ENCODE_TO_ESCAPED_STRING          =14;
        
        /* misc */
        ActionType_C DbEngine::ActionTypeDef::NOTHING                           =15;
        ActionType_C DbEngine::ActionTypeDef::END_THREAD                        =16;
        
        //////////////////// DbEngine ///////////////////////
        static void* db_engine_thread(void* param)
        {
            ThreadStartParam* start_param = (ThreadStartParam*)param;
            
            DbEngine* engine = (DbEngine*)start_param->db_engine_;
            
            engine->RunThread(start_param->location_);
        
            return 0;
        }
        
        DbEngine::DbEngine(vector<DbLocation>& locations, tr1::shared_ptr<IDbTasks> task)
            : mutex_(), result_reached_cond_()
        {
            task_ = task;
            
            thread_start_params_ = new ThreadStartParam [locations.size()];
            
            for (int i = 0; i < locations.size(); i++)
            {
                // prepare input commands
                InputCommand* tmp = new InputCommand();
				tmp->location_ = locations[i];
                works_[ locations[i] ] = tmp;
                
                // prepare thread input parameters
                thread_start_params_[i].location_ = locations[i];
                thread_start_params_[i].db_engine_ = this;
                
                // prepare blocking queue for each different db locations.
                input_blocking_queues_[locations[i]];
            }
        }
        
        DbEngine::~DbEngine()
        {
            if (threads_.size() != 0)
            {
                UninitEngine();
            }
            
            delete [] thread_start_params_;
            
            map<DbLocation, InputCommand* >::iterator it = works_.begin();
            while (it != works_.end())
            {
                delete it->second;
                it++;
            }
        }
        
		// a customer mould in the "producer and customer" design mode.
		// There is a unique db handle in each thread. That is to say,
		// the thread should only do its own work related to a specific db location.
        void DbEngine::RunThread(DbLocation& location)
        {
            InitThread();
        
			// prevent releasing the handle before the thread's uninitilization.
            {
                while(1)
                {
					// catch input commands from its own blocking queue.
                    InputCommand* input_param;
                    input_blocking_queues_[location].Pop(input_param);
            
					// get end command? end it now
                    if (input_param->action_ == ActionTypeDef::END_THREAD)
                    {
                        break;
                    }
            
					// real work here
                    ReturnParam tmp = RealDo(GetRealHandle(location).get(), input_param);
                    
                    {
                        COMMON::THREAD::MutexLockGuard lock(mutex_);
                        thread_return_param_.push_back(tmp);
                        result_reached_cond_.Notify();
                    }
                }
            }
            
            UninitThread();
        }
        
        DbEngine::ReturnParam DbEngine::RealDo(RealHandle* realHandle, InputCommand* inputParam)
        {
            int sqlCode = 0;
            string error_msg;
            void* rslt = 0;
            CharacterSetType cs;
            
            tr1::shared_ptr<EXCEPTION::IException> exception;
            string statement;
            if (inputParam->filter_)
            {
                statement = inputParam->filter_->GetContents();    
            }
            
            switch(inputParam->action_)
            {
            /* Connect and Disconnect */
            case ActionTypeDef::CONNECT:
                rslt = (void*)Connect((void*)realHandle, &(inputParam->location_), exception);
                break;
            
            case ActionTypeDef::DISCONNECT:
                rslt = (void*)Disconnect((void*)realHandle, &(inputParam->location_), exception);
                break;
            
            /* Query related work */
            case ActionTypeDef::QUERY:
				// for a query, the extra information is the map of column name and position index
                rslt = (void*)Query(
                        (void*)realHandle, 
                        &(inputParam->location_), 
                        statement.data(), 
                        statement.length(), 
                        ((map<DbLocation, map<string, int>* >*)(inputParam->filter_->GetAdditionalInfo()))->operator[](inputParam->location_), 
                        exception);
                break;
            
            case ActionTypeDef::FETCH:
                rslt = (void*)Fetch((void*)realHandle, &(inputParam->location_), exception);
                break;
            
            case ActionTypeDef::GET_COLUMNS_LENGTHS:
                rslt = (void*)GetColumnsActureLength((void*)realHandle, &(inputParam->location_), exception);
                break;
            
            case ActionTypeDef::CLOSE_OPEN_RSLT:
                rslt = (void*)CloseOpenRslt((void*)realHandle, &(inputParam->location_), exception);
                break;
            
            case ActionTypeDef::GET_AFFECTED_ROWS:
                rslt = (void*)GetAffectedRows((void*)realHandle, &(inputParam->location_), exception);
                break;
            
            /* execute related work */
            case ActionTypeDef::DELETE:
                rslt = (void*)Delete((void*)realHandle, &(inputParam->location_), statement.data(), statement.length(), exception);
                if (inputParam->commit_judger_ != 0)
                {
                    *(inputParam->already_affected_rows_) = ((long long)rslt) + *(inputParam->already_affected_rows_);
                    if( inputParam->commit_judger_->CanDoCommit(*(inputParam->already_affected_rows_) ))
                    {
                        (void*)Commit((void*)realHandle, &(inputParam->location_), exception);
                    }
                }
                break;
            
            case ActionTypeDef::UPDATE:
                rslt = (void*)Update((void*)realHandle, &(inputParam->location_), statement.data(), statement.length(), exception);
                if (inputParam->commit_judger_ != 0)
                {
                    *(inputParam->already_affected_rows_) = ((long long)rslt) + *(inputParam->already_affected_rows_);
                    if( inputParam->commit_judger_->CanDoCommit(*(inputParam->already_affected_rows_)) )
                    {
                        (void*)Commit((void*)realHandle, &(inputParam->location_), exception);
                    }
                }
                break;
            
            case ActionTypeDef::TRUNC:
                rslt = (void*)Truncate((void*)realHandle, &(inputParam->location_), statement.data(), statement.length(), exception);
                if (inputParam->commit_judger_ != 0)
                {
                    *(inputParam->already_affected_rows_) = ((long long)rslt) + *(inputParam->already_affected_rows_);
                    if( inputParam->commit_judger_->CanDoCommit(*(inputParam->already_affected_rows_) ))
                    {
                        (void*)Commit((void*)realHandle, &(inputParam->location_), exception);
                    }
                }
                break;
            
            case ActionTypeDef::INSERT:
                rslt = (void*)Insert((void*)realHandle, &(inputParam->location_), statement.data(), statement.length(), exception);
                if (inputParam->commit_judger_ != 0)
                {
                    *(inputParam->already_affected_rows_) = ((long long)rslt) + *(inputParam->already_affected_rows_);
                    if( inputParam->commit_judger_->CanDoCommit(*(inputParam->already_affected_rows_) ))
                    {
                        (void*)Commit((void*)realHandle, &(inputParam->location_), exception);
                    }
                }
                break;
            
            case ActionTypeDef::EXECUTE_ON_EXCEPTION:
                assert(1 != 1);
                break;
                
			/* common execute */
            case ActionTypeDef::EXECUTE:    
                rslt = (void*)Execute((void*)realHandle, &(inputParam->location_), statement.data(), statement.length(), exception);
                if (inputParam->commit_judger_ != 0)
                {
                    *(inputParam->already_affected_rows_) = ((long long)rslt) + *(inputParam->already_affected_rows_);
                    if( inputParam->commit_judger_->CanDoCommit(*(inputParam->already_affected_rows_) ))
                    {
                        (void*)Commit((void*)realHandle, &(inputParam->location_), exception);
                    }
                }
                break;
                
            /* other db operations */
            case ActionTypeDef::COMMIT:
                rslt = (void*)Commit((void*)realHandle, &(inputParam->location_), exception);
                break;
            
            case ActionTypeDef::ENCODE_TO_ESCAPED_STRING:
                rslt = (void*)EscapeString((void*)realHandle, &(inputParam->location_), statement.data(), statement.length(), exception);
                break;
                
            /* empty command */
            case ActionTypeDef::NOTHING:
                break;
            
            default:
                // TODO: Throw error
                break;
            }
            
            ReturnParam toReturn;
            toReturn.location = &(inputParam->location_);
            toReturn.return_items_ = rslt;
            toReturn.exception = exception;
            
            return toReturn;
        }
        
        void DbEngine::CreateWorks(
            ActionType_C actionType, 
            map<DbLocation, DbActionFilter*>& locFilter,
            map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows,
            vector<InputCommand* >& rslt)
        {
            map<DbLocation, DbActionFilter*>::iterator it = locFilter.begin();
            for ( ; it != locFilter.end(); it++)
            {
                //if (works_.find(it->first) != works_.end())
                {
                    InputCommand*& input = works_[it->first];
                    
                    input->action_ = actionType;
                    if (alreadyAffectedRows != 0)
                    {
                        input->already_affected_rows_ = &((*alreadyAffectedRows)[it->first].already_affected_rows_);
                        input->commit_judger_ = (*alreadyAffectedRows)[it->first].commit_judger_.get();
                    }
                    else
                    {
                        input->commit_judger_ = 0;
                    }
        
                    input->filter_ = it->second;
                    
                    rslt.push_back(input);
                }
            }
        }
        
        void DbEngine::CreateWorks(
            ActionType_C actionType, 
            DbActionFilter* filter, 
            map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows,
            vector<InputCommand* >& rslt)
        {
            map<DbLocation, InputCommand* >::iterator it = works_.begin();
            while(it != works_.end())
            {
                it->second->action_ = actionType;
                if (alreadyAffectedRows != 0)
                {
                    it->second->already_affected_rows_ = &((*alreadyAffectedRows)[it->first].already_affected_rows_);
                    it->second->commit_judger_ = (*alreadyAffectedRows)[it->first].commit_judger_.get();
                }
                else
                {
                    it->second->commit_judger_ = 0;
                }
        
                it->second->filter_ = filter;
                
                rslt.push_back(it->second);
                
                it++;
            }
        }
        
        void DbEngine::CreateWorks(
            ActionType_C actionType, 
            const DbLocation* location, 
            DbActionFilter* filter, 
            map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows,
            InputCommand*& rslt)
        {
            rslt = works_[*location];
            rslt->action_ = actionType;
            if (alreadyAffectedRows != 0)
            {
                rslt->already_affected_rows_ = &((*alreadyAffectedRows)[*location].already_affected_rows_);
                rslt->commit_judger_ = (*alreadyAffectedRows)[*location].commit_judger_.get();
            }
            else
            {
                rslt->commit_judger_ = 0;
            }
        
            rslt->filter_ = filter;
        }
        
        bool DbEngine::InitEngine()
        {
            // prepare the handle
            map<DbLocation, InputCommand* >::iterator it = works_.begin();
            while(it != works_.end())
            {
                GetRealHandle(it->second->location_);
                it++;
            }
            
			// start a new start for each db location
            int thread_num = works_.size();
        
            for ( int i = 0; i < thread_num; i++)
            {
                tr1::shared_ptr<COMMON::THREAD::Thread> t(
                        new COMMON::THREAD::Thread(
                            db_engine_thread, (void*)(&thread_start_params_[i]) ));
                
				// start the thread
                t->Start();
                
                threads_.push_back(t);
            }
            
            return true;
        }
        
        // uninitialize the engine, to stop the thread pool
        bool DbEngine::UninitEngine()
        {
            int thread_num = threads_.size();
        
            InputCommand* command = new InputCommand();
            command->action_ = ActionTypeDef::END_THREAD;
        
            // stop threads by sending END_THREAD signals
            for (int i = 0; i < thread_num; i++)
            {
                tr1::shared_ptr<InputCommand> input(new InputCommand(ActionTypeDef::END_THREAD));
                
                // send end command to each thread.
                map<DbLocation, BlockingQueue<InputCommand*> >::iterator it = input_blocking_queues_.begin();
                while (it != input_blocking_queues_.end())
                {
                    (it->second).Push(command);
					it++;
                }

            }
        
            // wait all to exit
            for ( int i = 0; i < thread_num; i++)
            {
                threads_[i]->Join();
            }
        
            // clear all the threads object
            threads_.clear();
         
            delete command;
         
            return true;
        }
        
        vector<const DbLocation*> DbEngine::GetDbLocations()
        {
            vector<const DbLocation*> rslt;
        
            map<DbLocation, InputCommand* >::iterator it = works_.begin();
            for ( ; it != works_.end(); it++)
            {
                rslt.push_back(&(it->first));
            }
        
            return rslt;
        }
      
        map<DbLocation*, void*> DbEngine::SyncDo(
                ActionType_C actionType, 
                map<DbLocation, DbActionFilter*>& locFilter, 
                bool & success,
                map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows) throw (ThrowableException)
        {
            success = true;
            map<DbLocation*, void*> rslt;
            
            vector<InputCommand*> work_list;
            CreateWorks(actionType, locFilter, alreadyAffectedRows, work_list);
            
            // do in the current thread
            for (int i = 0; i < work_list.size(); i++)
            {
                ReturnParam toReturn = RealDo(GetRealHandle(work_list[i]->location_).get(), work_list[i]);
                
                if (toReturn.exception)
                {
                    EXCEPTION::ThrowableException e(toReturn.exception);
                    throw e;
                }
                else
                {
                    rslt[&(work_list[i]->location_)] = toReturn.return_items_;
                }
            }
            
            return rslt;
        }
      
        // I have to use the redundant codes to save the time	  
        map<DbLocation*, void*> DbEngine::SyncDo(
            ActionType_C actionType, 
            DbActionFilter* filter, 
            bool & success, 
            map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows) throw (ThrowableException)
        {
            success = true;
            map<DbLocation*, void*> rslt;
            
            vector<InputCommand*> work_list;
            CreateWorks(actionType, filter, alreadyAffectedRows, work_list);
            
            for (int i = 0; i < work_list.size(); i++)
            {
                ReturnParam toReturn = RealDo(GetRealHandle(work_list[i]->location_).get(), work_list[i]);
                if (toReturn.exception)
                {
                    EXCEPTION::ThrowableException e(toReturn.exception);
                    throw e;
                }
                else
                {
                    rslt[&(work_list[i]->location_)] = toReturn.return_items_;
                }                
            }

            return rslt;
        }
        
        void* DbEngine::SyncDo(
            ActionType_C actionType, 
            const DbLocation* location, 
            DbActionFilter* filter, 
            bool & success,
            map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows) throw (ThrowableException)
        {
            success = true;
            void* rslt = 0;

            InputCommand* work;
            CreateWorks(actionType, location, filter, alreadyAffectedRows, work);
            
            ReturnParam toReturn = RealDo(GetRealHandle(*(const_cast<DbLocation*>(location))).get(), work);
            if (toReturn.exception)
            {
                EXCEPTION::ThrowableException e(toReturn.exception);
                throw e;
            }
            else
            {
                rslt = toReturn.return_items_;
            }
            
            return rslt;
        }
      
        map<DbLocation*, void*> DbEngine::Do(
            ActionType_C actionType, 
            DbActionFilter* filter, 
            bool & success,
            map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows) throw (EXCEPTION::ThrowableException)
        {
            success = true;
        
            vector<InputCommand* > work_list;
            CreateWorks(actionType, filter, alreadyAffectedRows, work_list);
            return AsyncDoCheckGetRslt(work_list, success);
        }
        
        void* DbEngine::Do(
            ActionType_C actionType, 
            const DbLocation* location, 
            DbActionFilter* filter, 
            bool & success,
            map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows) throw (EXCEPTION::ThrowableException)
        {
            return SyncDo(actionType, location, filter, success, alreadyAffectedRows);
        }
        
        map<DbLocation*, void*> DbEngine::Do(
            ActionType_C actionType, 
            map<DbLocation, DbActionFilter*>& locFilter, 
            bool & success,
            map<DbLocation, AffectedRowRecorder>* alreadyAffectedRows) throw (EXCEPTION::ThrowableException)
        {
            success = true;
        
            vector<InputCommand* > work_list;
            CreateWorks(actionType, locFilter, alreadyAffectedRows, work_list);
            return AsyncDoCheckGetRslt(work_list, success);
        }
        
        map<DbLocation*, void*> DbEngine::AsyncDoCheckGetRslt( 
            vector<InputCommand* >& work_list, 
            bool &success)
        {
			// give commands and wait for result
            PushWorkAndWait(work_list);
        
            // get results
            map<DbLocation*, void*> rslt = GetRslt();
        
            // check whether there are exception, if positive, handle them
            success = CheckHasException();
        
            // clear result set
            ClearRslts();
        
            return rslt;
        }
        
        void DbEngine::PushWorkAndWait( vector<InputCommand*>& workList )
        {
            int work_count = workList.size();
        
            if (work_count != 0)
            {
                for (int i = 0; i < work_count; i++)
                {
                    input_blocking_queues_[ workList[i]->location_ ].Push(workList[i]);
                }
        
                // wait until all results come out
                {
                    COMMON::THREAD::MutexLockGuard lock(mutex_);
                    while (thread_return_param_.size() < work_count)
                    {
                        result_reached_cond_.Wait(mutex_);
                    }
                }        
            }
        }
        
        bool DbEngine::CheckHasException() throw (EXCEPTION::ThrowableException)
        {
            bool success = true;

            vector<tr1::shared_ptr<EXCEPTION::IException> > real_exception;
            for (int i = 0; i < thread_return_param_.size(); i++)
            {
                if (thread_return_param_[i].exception.use_count() != 0)
                {
                    // exception happened
                    real_exception.push_back(thread_return_param_[i].exception);
                }
            }
        
            // if there is an exception, push out
            if (real_exception.size() != 0)
            {
                tr1::shared_ptr<IDbTasks> tmp = task_.lock();
                
                if (tmp == 0 || tmp->IsExceptionMode() == true)
                {
                    // clear results
                    ClearRslts();
        
                    EXCEPTION::ThrowableException e(real_exception);
                    throw e;
                }
                else
                {
                    success = false;
                    tr1::shared_ptr<EXCEPTION::ThrowableException> e(new EXCEPTION::ThrowableException(real_exception));
                    tmp->SetExceptions(e);
                }
            }
        
            return success;
        }
        
        void DbEngine::ClearRslts()
        {
            COMMON::THREAD::MutexLockGuard lock(mutex_);
            thread_return_param_.clear();
        }
        
        map<DbLocation*, void*> DbEngine::GetRslt()
        {
            map<DbLocation*, void*> rslt;
            for (int i = 0; i < thread_return_param_.size(); i++)
            {
                rslt[thread_return_param_[i].location] = thread_return_param_[i].return_items_;
            }
        
            return rslt;
        }        
    }
}
