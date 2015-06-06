#include "dbcomm/DbLocation.h"
#include "dbcomm/DbAction.h"
#include "dbcomm/DbTasks.h"

namespace COMMON
{
    namespace DBCOMM
    {
        bool DbAction::Do(
            DbActionFilter* filter, map<DbLocation, long long>* affected_rows) throw (EXCEPTION::ThrowableException)
        {
            vector<DbLocation>& locations = GetDbLocations();
            
            map<DbLocation, DbActionFilter*> works;
            
            // get DbLocation
            for (int i = 0; i < locations.size(); i++)
            {
                works[locations[i]] = filter;
            }
            
            return Do(works, affected_rows);
        }

        bool DbAction::Do(
            DbActionFilter* filter, DbLocation* location, long long* affected_rows) throw (EXCEPTION::ThrowableException)
        {
            map<DbLocation, DbActionFilter*> works;
            works[*location] = filter;
            
            map<DbLocation, long long> ar;
            ar[*location] = 0;
        
            bool rslt = Do(works, &ar);
            if (affected_rows)
            {
                *affected_rows = ar[*location];
            }
            
            return rslt;
        }
        
        bool DbAction::EndAction(map<DbLocation, long long>* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            bool success = true;
        
            if (actioned_db_info_.size() == 0)
            {
                DbActionFilter f;
                engine_->Do(DbEngine::ActionTypeDef::COMMIT, &f, success);

                if (affected_rows)
                {
                    if (task_.lock())
                    {
                        vector<DbLocation>& t = task_.lock()->GetDbLocations();
                        vector<DbLocation>::const_iterator it = t.begin();
                        while (it != t.end())
                        {
                            // COMMIT won't affect any rows
                            (*affected_rows)[*it] = 0;
                            it++;
                        }    
                    }
                }
            }
            else
            {
                engine_->Do(DbEngine::ActionTypeDef::COMMIT, actioned_db_info_, success);
                
                if (affected_rows)
                {
                    map<DbLocation, DbActionFilter*>::iterator it = actioned_db_info_.begin();
                    while (it != actioned_db_info_.end())
                    {
                        // COMMIT操作不影响行数
                        (*affected_rows)[it->first] = 0;
                        it++;
                    }
                }
            }
            
            if (success)
            {
                is_action_finished_ = true;
            }
            
            return success;
        }
        
        DbAction::DbAction(tr1::shared_ptr<IDbTasks> tasks, tr1::shared_ptr<DbEngine> engine, bool& is_action_finished)
            : task_(tasks), engine_(engine), is_action_finished_(is_action_finished) 
        {    
            is_action_finished_ = false;
            
            allDbActioned_ = false;
        }
        
        
        DbAction::~DbAction()
        {
            is_action_finished_ = true;
        }
        
        void DbAction::SetActionedDbInfo(map<DbLocation, DbActionFilter*>& locFilter)
        {
            map<DbLocation, DbActionFilter*>::iterator it = locFilter.begin();
            for (; it != locFilter.end(); it++)
            {
                actioned_db_info_[it->first] = 0;
            }
        }
        
        void DbAction::SetActionedDbInfo(DbLocation* location)
        {
            assert(location != 0);
        
            actioned_db_info_[*location] = 0;
        }
        
        void DbAction::SetActionedDbInfo()
        {
            if (allDbActioned_ == false)
            {
                allDbActioned_ = true;
        
                vector<DbLocation>&  tmp = task_.lock()->GetDbLocations();
                for (int i = 0; i < tmp.size(); i++)
                {
                    actioned_db_info_[tmp[i]] = 0;
                }
            }
        }
        
        vector<DbLocation>& DbAction::GetDbLocations()
        {
            return task_.lock()->GetDbLocations();
        }
        
        bool DbAction::IsExceptionMode() 
        { 
            return task_.lock()->IsExceptionMode(); 
        }
        
        void DbAction::SetException(tr1::shared_ptr<EXCEPTION::ThrowableException> e)
        { 
            task_.lock()->SetExceptions(e); 
        }
        
        string DbAction::GetLastError()
        {
            return task_.lock()->GetLastError(); 
        }       
    }
}