#include "dbcomm/DbTasks.h"
#include "dbcomm/DbQueryAction.h"

#include "exception/IException.h"
#include "exception/CodingException.h"

#include "tool/StringHelper.h"

namespace COMMON
{
    namespace DBCOMM
    {
        ////////////////////////////////////////////
        // DbQueryAction
        ////////////////////////////////////////////
        DbQueryAction::DbQueryAction(tr1::shared_ptr<IDbTasks> dbtasks, tr1::shared_ptr<DbEngine> engine, bool& is_action_finished)
            :DbAction(dbtasks, engine, is_action_finished)
        {
            is_rslt_opened_ = false;
            
			// prepare a column_name and index map to save time
            vector<DbLocation>& locations = GetDbLocations();
            for (int i = 0; i < locations.size(); i++)
            {
                column_name_index_map_[locations[i]] = new map<string, int>();
            }
        }

        DbQueryAction::~DbQueryAction()
        {
            map<DbLocation, map<string, int>* > ::iterator it = column_name_index_map_.begin();
            while (it != column_name_index_map_.end())
            {
                delete it->second;
                it++;
            }
        }

        DbRslt* DbQueryAction::GetRslt()
        {
            if (is_rslt_opened_ == false)
            {
                // fetch the result without an open result set. This is an coding exception
                tr1::shared_ptr<EXCEPTION::IException> inner_e(new EXCEPTION::CodeSequenceException());
                if (task_.lock()->IsExceptionMode() == true)
                {
                    EXCEPTION::ThrowableException e(inner_e);
                    throw e;
                }
                else
                {
                    tr1::shared_ptr<EXCEPTION::ThrowableException> e(new EXCEPTION::ThrowableException(inner_e));
                    SetException(e);
                }
                
                return 0;
            }

            action_rslt_.reset();

            action_rslt_ = tr1::shared_ptr<DbQueryRslt>(new DbQueryRslt(shared_from_this(), engine_));
            
            return action_rslt_.get();
        }

        void DbQueryAction::GetAffectedRows(
            map<DbLocation*, void*>* work_rslt, map<DbLocation, long long>* affected_rows)
        {
            if (affected_rows)
            {
                affected_rows->clear();
                map<DbLocation*, void*>::iterator it = work_rslt->begin();
                for (; it != work_rslt->end(); it++)
                {
                    (*affected_rows)[*(it->first)] = 0;
                }
            }
        }
        
        bool DbQueryAction::Do(DbActionFilter* filter, map<DbLocation, long long>* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            return DbAction::Do(filter, affected_rows);
        }

        bool DbQueryAction::Do(DbActionFilter* filter, DbLocation* location, long long* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            return DbAction::Do(filter, location, affected_rows);
        }

        bool DbQueryAction::Do(
            map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows) throw (EXCEPTION::ThrowableException)
        {
            bool success = true;

            // begin the action
            is_action_finished_ = false;
            
            // we need to query the column name and its associated position index in the query statement
            map<DbLocation, DbActionFilter*>::iterator it = works.begin();
            while (it != works.end())
            {
                it->second->SetAdditionalInfo((void*)&column_name_index_map_);
                it++;
            }
            
            // begin to work
            map<DbLocation*, void*> rslt;
            if (works.size() == 1)
            {
                rslt = engine_->SyncDo(DbEngine::ActionTypeDef::QUERY, works, success);    
            }
            else
            {
                rslt = engine_->Do(DbEngine::ActionTypeDef::QUERY, works, success);    
            }

            if (success)
            {   
                SetActionedDbInfo(works);
                GetAffectedRows(&rslt, affected_rows);

                is_rslt_opened_ = success;
            }

            return success;
        }

        bool DbQueryAction::EndAction(map<DbLocation, long long>* affected_rows) throw (EXCEPTION::ThrowableException)
        {
			// for query, an extra close step should be done before end
         
			bool success = false;

            if (is_rslt_opened_ == true)
            {
                if (actioned_db_info_.size() == 0)
                {
                    // send close command to all the db targets
                    DbActionFilter f;
                    engine_->Do(DbEngine::ActionTypeDef::CLOSE_OPEN_RSLT, &f, success);
                }
                else
                {
                    // send close command to a specific db target
                    engine_->Do(DbEngine::ActionTypeDef::CLOSE_OPEN_RSLT, actioned_db_info_, success);
                }
                
                if (success)
                {
                    success = DbAction::EndAction(affected_rows);
                }
            }

            if (success)
            {
                is_rslt_opened_ = false;
                is_action_finished_ = true;
            }

            return success;
        }

        map<string, int>* DbQueryAction::GetColumnStringIndex(DbLocation& location)
        {
            return column_name_index_map_[location];
        } 

        ////////////////////////////////////////////
        // DbGetPriKeysAction
        ////////////////////////////////////////////
        bool DbGetPriKeysAction::Do(
            map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows) throw (ThrowableException)
        {
			// we need some new filters instanse to make sure that all the works 
			// have a filter that is still alive when they are working
            vector<DbGetPriKeysFilter> real_filters(works.size());
            
            vector<string> columns;
            string values;
            
            // refill the select statement
            int i = 0;
            map<DbLocation, DbActionFilter*>::iterator it = works.begin();
            while (it != works.end())
            {
                string tbl_name = it->second->GetContents();
                stmt_gen_[it->first]->MakeupStatement(columns, tbl_name, values);
                real_filters[i].SetContents(stmt_gen_[it->first]->FormStatement(it->first));
                
                it->second = &(real_filters[i]);
                
                it++;
                i++;
            }
            
            return DbQueryAction::Do(works, affected_rows);
        }
    }
}
