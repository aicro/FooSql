#include <algorithm>

#include "dbcomm/CommDef.h"
#include "dbcomm/DbTasks.h"
#include "dbcomm/DbAction.h"
#include "dbcomm/DbQueryAction.h"
#include "dbcomm/DbExecuteAction.h"
#include "dbcomm/EscapeStringAction.h"
#include "dbcomm/DbBatchAction.h"
#include "dbcomm/DbException.h"

#include "exception/CodingException.h"
#include "exception/IException.h"

namespace COMMON
{
    namespace DBCOMM
    {
        ////////////  DbTasks  ////////////////////////
        DbTasks::DbTasks(vector<DbLocation>& dbLocations, bool exception)
        {
            exception_ = exception;
            is_connected_    = false;
            is_action_finished_ = true;
            is_engine_initialized_ = false;
            
            db_locations_ = dbLocations;
        }

        DbTasks::~DbTasks() throw()
        {
        }

        bool DbTasks::Connect() throw (COMMON::EXCEPTION::ThrowableException)
        {
            if (is_connected_)
            {
                return true;
            }	

            if (is_engine_initialized_ == false)
            {
                InitEngine();
                is_engine_initialized_ = true;    
            }            
            
            bool success = false;

            DbActionFilter f;
            db_engine_->Do(DbEngine::ActionTypeDef::CONNECT, &f, success);
            
            if (success == true)
            {
                is_connected_ = true;
            }

            return success;
        }

        bool DbTasks::Disconnect() throw (COMMON::EXCEPTION::ThrowableException)
        {
            bool success = true;

			// disconnect should be always done for clearing the memory
            if (is_engine_initialized_)
            {
                try
                {
                    // end the previous action
                    if (current_work_ )
                    {
                        if (is_action_finished_ == false)
                        {
                            current_work_->EndAction();
                        } 

                        current_work_.reset();
                    }
                    
                    DbActionFilter f;
                    db_engine_->Do(DbEngine::ActionTypeDef::DISCONNECT, &f, success);
                    is_connected_ = false;
                    
                    UninitEngine();
                    is_engine_initialized_ = false;
                }
                catch(ThrowableException& e)
                {
                    // EndAction will possibly thrown exception 
                    
                    DbActionFilter f;
                    db_engine_->Do(DbEngine::ActionTypeDef::DISCONNECT, &f, success);
                    is_connected_ = false;
                    
                    // uninitialize the engine for memory clearence
                    UninitEngine();
                    is_engine_initialized_ = false;
                    
                    // re-throw the exception
                    throw e;
                }    
            }

            return success;
        }

        vector<DbLocation>& DbTasks::GetDbLocations()
        {
            return db_locations_;
        }

        bool DbTasks::CanStartAction()
        {
            bool success = true;
            
            if (is_action_finished_ == false)
            {
                tr1::shared_ptr<COMMON::EXCEPTION::IException> inner_e(new COMMON::EXCEPTION::PrevWorkNotFinishedException());
                
                if (exception_)
                {
                    COMMON::EXCEPTION::ThrowableException e(inner_e);
                    throw  e;
                }
                else
                {
                    tr1::shared_ptr<COMMON::EXCEPTION::ThrowableException> e(new COMMON::EXCEPTION::ThrowableException(inner_e));
                    error_box_.SetException(e);
                    success = false;
                }
            }
            
            return success;
        }

        DbQueryAction* DbTasks::Select()
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();
            
            current_work_ = tr1::shared_ptr<DbQueryAction>(new DbQueryAction(shared_from_this(), db_engine_, is_action_finished_));
            return (DbQueryAction*)current_work_.get();
        }

        DbExecuteAction* DbTasks::Insert( int commitLimit /*= 5000*/ )
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = tr1::shared_ptr<DbInsertAction>(new DbInsertAction(shared_from_this(), db_engine_, is_action_finished_, commitLimit));
            return (DbExecuteAction*)current_work_.get();
        }

        DbExecuteAction* DbTasks::BatchInsert( int commitLimit /*= 5000*/, int valuesLimit /*= 10*/)
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = 
                tr1::shared_ptr<BatchInsertAction>(
                    new BatchInsertAction(
                        shared_from_this(), 
                        db_engine_, 
                        is_action_finished_, 
                        valuesLimit, 
                        commitLimit));

            return (DbExecuteAction*)current_work_.get();
        }

        DbExecuteAction* DbTasks::Delete( int commitLimit /*= 5000 */ )
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = tr1::shared_ptr<DbDeleteAction>(new DbDeleteAction(shared_from_this(), db_engine_, is_action_finished_, commitLimit));
            return (DbExecuteAction*)current_work_.get();
        }

        DbExecuteAction* DbTasks::Truncate()
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();
            
            current_work_ = tr1::shared_ptr<DbTruncateAction>(new DbTruncateAction(shared_from_this(), db_engine_, is_action_finished_));
            return (DbExecuteAction*)current_work_.get();
        }

        DbExecuteAction* DbTasks::Update( int commitLimit /*= 5000 */ )
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();
            
            current_work_ = tr1::shared_ptr<DbUpdateAction>(new DbUpdateAction(shared_from_this(), db_engine_, is_action_finished_, commitLimit));
            return (DbExecuteAction*)current_work_.get();
        }

        DbExecuteAction* DbTasks::Execute()
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();
            
            current_work_ = tr1::shared_ptr<DbExecuteAction>(new DbExecuteAction(shared_from_this(), db_engine_, is_action_finished_));
            return (DbExecuteAction*)current_work_.get();
        }
        
        DbExecuteAction* DbTasks::EscapeString()
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();
            
            current_work_ = tr1::shared_ptr<DbExecuteAction>(
                                new EscapeStringAction(
                                    shared_from_this(), db_engine_, is_action_finished_));
            return (DbExecuteAction*)current_work_.get();
        }

    }
}

