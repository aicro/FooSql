#ifdef DB2_ENV_AVAILABLE

#include "dbcomm/DB2DbTasks.h"
#include "dbcomm/DB2Engine.h"
#include "dbcomm/DbExecuteAction.h"
#include "dbcomm/DB2SpecialAction.h"
#include "dbcomm/DbQueryAction.h"

namespace COMMON
{
    namespace DBCOMM
    {
        DB2DbTasks::DB2DbTasks(vector<DbLocation>& dbLocations, bool exception)
            : DbTasks(dbLocations, exception)
        {
        }
        
        DB2DbTasks::~DB2DbTasks() throw ()
        {
            if (is_connected_)
            {
				// we should avoid exception being thrown out from the deconstructor
                try
                {
                    DbTasks::Disconnect();
                }
                catch(COMMON::EXCEPTION::ThrowableException& e)
                {
                    WRITE_LOG_TO_ERR("exception occurs during deconstructor£¡");
                    WRITE_LOG_TO_ERR(e.What());
                }	
            }
        }
        
        bool DB2DbTasks::InitEngine()
        {
            db_engine_ = tr1::shared_ptr<DB2Engine>(new DB2Engine(db_locations_, shared_from_this()));
            db_engine_->InitEngine();
            
            return true;
        }
        
        bool DB2DbTasks::UninitEngine()
        {
            db_engine_->UninitEngine();
            
            return true;
        }       

        DbExecuteAction* DB2DbTasks::BatchInsertIgnore( int commitLimit, int valuesLimit)
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = 
                tr1::shared_ptr<DbBatchAction>(
                new DB2InsertIgnoreBatchAction(
                    shared_from_this(), 
                    db_engine_, 
                    is_action_finished_, 
                    valuesLimit, 
                    commitLimit));

            return (DbExecuteAction*)current_work_.get();
        }        
        
        DbExecuteAction* DB2DbTasks::BatchReplace( int commitLimit, int valuesLimit)
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = 
                tr1::shared_ptr<DbBatchAction>(
                new DB2ReplaceBatchAction(
                    shared_from_this(), 
                    db_engine_, 
                    is_action_finished_, 
                    valuesLimit, 
                    commitLimit));

            return (DbExecuteAction*)current_work_.get();
        }
        
        DbQueryAction* DB2DbTasks::GetPriKeys()
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = tr1::shared_ptr<DbQueryAction>(
                            new DB2GetPriKeysAction(
                                shared_from_this(), 
                                db_engine_, 
                                is_action_finished_));
            return (DbQueryAction*)current_work_.get();
        }
    }
}

#else
#warning "DB2 ENV NOT AVAILABLE... NOT COMPILED..."

#endif
