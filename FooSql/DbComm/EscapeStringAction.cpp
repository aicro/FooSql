#include "dbcomm/EscapeStringAction.h"

namespace COMMON
{
    namespace DBCOMM
    {
        EscapeStringAction::EscapeStringAction(
            tr1::shared_ptr<IDbTasks> dbtasks, 
            tr1::shared_ptr<DbEngine> engine, bool& is_action_finished)
            : DbExecuteAction(dbtasks, engine, is_action_finished, 0),
              current_escaped_string_index_(0)
        {
        }

        ActionType_C EscapeStringAction::GetRealActionType() 
        { 
            return DbEngine::ActionTypeDef::ENCODE_TO_ESCAPED_STRING; 
        }

        void EscapeStringAction::PrepareEscapedString(map<DbLocation*, void*>& workRslt)
        {
            escaped_strings_.clear();
            escaped_strings_index_.clear();

            map<DbLocation*, void*>::iterator it = workRslt.begin();
            int i = 0;
            for (; it != workRslt.end(); it++)
            {
                escaped_strings_.push_back(string((char*)(it->second)));
                escaped_strings_index_[*(it->first)] = i;
            }
            
            current_escaped_string_index_ = 0;
        }
        
        bool EscapeStringAction::Do(DbActionFilter* filter, map<DbLocation, long long>* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            return DbAction::Do(filter, affected_rows);
        }

        bool EscapeStringAction::Do(DbActionFilter* filter, DbLocation* location, long long* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            return DbAction::Do(filter, location, affected_rows);
        }

        bool EscapeStringAction::Do(
            map<DbLocation, DbActionFilter*>& works, map<DbLocation, long long>* affected_rows) throw (COMMON::EXCEPTION::ThrowableException)
        {
            bool success = true;

            if (works.size() == 0)
            {
                return success;
            }

            map<DbLocation*, void*> workRslt = engine_->SyncDo(GetRealActionType(), works, success);

            if (success)
            {
                PrepareEscapedString(workRslt);
            }

            return success;
        }

        DbRslt* EscapeStringAction::GetRslt()
        {
            return 0;
        }

        bool EscapeStringAction::EndAction() throw (COMMON::EXCEPTION::ThrowableException) 
        { 
			// nothing more should be done
            return true; 
        }        
        
        string EscapeStringAction::GetEscapedString()
        {
            if (current_escaped_string_index_ == escaped_strings_.size())
            {
                return "";
            }
            
            return escaped_strings_[current_escaped_string_index_++];
        }
        
        string EscapeStringAction::GetEscapedString(DbLocation& location)
        {
            return escaped_strings_[ escaped_strings_index_[location] ];
        }
    }
}
