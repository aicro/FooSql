#ifdef MYSQL_ENV_AVAILABLE

#include "dbcomm/MysqlDbTasks.h"
#include "dbcomm/MysqlEngine.h"
#include "dbcomm/DbExecuteAction.h"
#include "dbcomm/MysqlSpecialAction.h"
#include "dbcomm/DbQueryAction.h"
#include "dbcomm/MysqlStmtGen.h"

namespace COMMON
{
    namespace DBCOMM
    {
        MysqlDbTasks::MysqlDbTasks(vector<DbLocation>& dbLocations, bool exception)
            : DbTasks(dbLocations, exception)
        {
        }

        MysqlDbTasks::~MysqlDbTasks() throw ()
        {
            if (is_connected_)
            {
                // Catch all the uncaught exception before leave the deconstructor
                try
                {
                    DbTasks::Disconnect();
                }
                catch(EXCEPTION::ThrowableException& e)
                {
                    string err = string("In ~MysqlDbTasks():\n") + e.What();
                    WRITE_LOG_TO_ERR(err);
                
                }	
            }
            else if (is_engine_initialized_ == true)
            {
                // this command will only be done when there is an exception during connection
                UninitEngine();
            }
        }
        
        DbExecuteAction* MysqlDbTasks::BatchInsertIgnore( int commitLimit, int valuesLimit)
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = 
                tr1::shared_ptr<DbBatchAction>(
                new MysqlInsertIgnoreBatchAction(
                    shared_from_this(), 
                    db_engine_, 
                    is_action_finished_, 
                    valuesLimit, 
                    commitLimit));

            return (DbExecuteAction*)current_work_.get();
        }  

        DbExecuteAction* MysqlDbTasks::BatchReplace( int commitLimit, int valuesLimit)
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = 
                tr1::shared_ptr<DbBatchAction>(
                new MysqlReplaceBatchAction(
                    shared_from_this(), 
                    db_engine_, 
                    is_action_finished_, 
                    valuesLimit, 
                    commitLimit));

            return (DbExecuteAction*)current_work_.get();
        }
        
        DbQueryAction* MysqlDbTasks::GetPriKeys()
        {
            if (false == CanStartAction())
            {
                return 0;
            }

            current_work_.reset();

            current_work_ = tr1::shared_ptr<DbQueryAction>(
                            new MysqlGetPriKeysAction(
                                shared_from_this(), 
                                db_engine_, 
                                is_action_finished_));
            return (DbQueryAction*)current_work_.get();
        }

        bool MysqlDbTasks::InitEngine()
        {
            db_engine_ = tr1::shared_ptr<MysqlEngine>(new MysqlEngine(db_locations_, shared_from_this()));
            db_engine_->InitEngine();
            
            return true;
        }

        bool MysqlDbTasks::UninitEngine()
        {
            db_engine_->UninitEngine();
            
            return true;
        }        
    }
}

#else
#warning "MYSQL ENV NOT AVAILABLE... NOT COMPILED..."

#endif