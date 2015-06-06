#include "dbcomm/DbExecuteAction.h"
#include "dbcomm/DbTasks.h"
#include "dbcomm/DbExecuteRslt.h"

#include "exception/CodingException.h"

namespace COMMON
{
    namespace DBCOMM
    {
        DbExecuteAction::DbExecuteAction(
            tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& is_action_finished, int times_to_commit)
            : DbAction(dbtasks, engine, is_action_finished)
        {
            vector<DbLocation>& tmp = dbtasks->GetDbLocations();
            for (int i = 0; i < tmp.size(); i++)
            {
                already_affected_rows_[tmp[i]].already_affected_rows_ = 0;
                already_affected_rows_[tmp[i]].commit_judger_.reset(new CommitJudger(times_to_commit));
            }
        }

        DbExecuteAction::~DbExecuteAction()
        {
        }
    
        DbRslt* DbExecuteAction::GetRslt()
        {
            action_rslt_.reset();
    
            map<DbLocation, long long> affected_rows;
            map<DbLocation, AffectedRowRecorder>::iterator it = already_affected_rows_.begin();
            for (; it != already_affected_rows_.end(); it++)
            {
                affected_rows[it->first] = it->second.already_affected_rows_;
            }
    
            action_rslt_ = tr1::shared_ptr<DbExecuteRslt>(new DbExecuteRslt(shared_from_this(), affected_rows));
    
            return action_rslt_.get();
        }

        bool DbExecuteAction::Do(DbActionFilter* filter, map<DbLocation, long long>* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            bool success = true;
    
            map<DbLocation*, void*> work_rslt = 
                engine_->Do(
                    GetRealActionType(), 
                    filter, 
                    success, 
                    &already_affected_rows_);  

            if (success)
            {
                SetActionedDbInfo();   
                GetAffectedRows(work_rslt, affected_rows);
            }
    
            return success;
        }

        bool DbExecuteAction::Do(DbActionFilter* filter, DbLocation* location, long long* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            bool success = true;
    
            void* work_rslt = 
                engine_->Do(
                    GetRealActionType(), 
                    location, 
                    filter,
                    success, 
                    &already_affected_rows_);  

            if (success)
            {
                SetActionedDbInfo(location); 
                
                if (affected_rows)
                {
                    *affected_rows = (long long)work_rslt;
                }
            }
    
            return success;
        }
        
        bool DbExecuteAction::Do(
            map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows) throw (EXCEPTION::ThrowableException)
        {
            bool success = true;
    
            if (works.size() == 0)
            {
                return success;
            }
    
            map<DbLocation*, void*> work_rslt;
            if (works.size() == 1)
            {
                work_rslt = engine_->SyncDo(
                    GetRealActionType(), 
                    works, 
                    success, 
                    &already_affected_rows_);
            }
            else
            {
                work_rslt = engine_->Do(
                    GetRealActionType(), 
                    works, 
                    success, 
                    &already_affected_rows_);    
            }

            if (success)
            {
                if (work_rslt.size() != works.size())
                {
                    string detail = "there is a duplicate key in the std::map, the input parameters are : ";
                    map<DbLocation, DbActionFilter*>::iterator it = works.begin();
                    while (it != works.end())
                    {
                        detail += it->first.ToString() + " ; ";
                        it++;
                    }
                    
                    // duplicate key in the std::map
                    tr1::shared_ptr<EXCEPTION::IException> inner_e(
                        new EXCEPTION::WorkNumNoMatchResultNumException(works.size(), work_rslt.size(), detail));
    
                    if (task_.lock()->IsExceptionMode())
                    {
                        EXCEPTION::ThrowableException e(inner_e);
                        throw e;
                    }
                    else
                    {
                        success = false;
                        tr1::shared_ptr<EXCEPTION::ThrowableException> e(new EXCEPTION::ThrowableException(inner_e));
                        SetException(e);
                    }
                }
                else
                {
                    // ok
                    SetActionedDbInfo(works);   
    
                    GetAffectedRows(work_rslt, affected_rows);
                }
            }
    
            return success;
        }
    
        void DbExecuteAction::GetAffectedRows(
            map<DbLocation*, void*>& workRslt, map<DbLocation, long long>* affected_rows)
        {
            if (affected_rows)
            {
                affected_rows->clear();
            }
    
            map<DbLocation*, void*>::iterator it = workRslt.begin();
            for (; it != workRslt.end(); it++)
            {
                if (affected_rows)
                {
                    (*affected_rows)[*(it->first)] = (long long)(it->second);
                }	
            }  
        }
    }
}