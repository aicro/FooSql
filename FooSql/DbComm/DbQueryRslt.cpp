#include "dbcomm/DbQueryRslt.h"
#include "exception/IException.h"
#include "dbcomm/DbAction.h"
#include "dbcomm/DbQueryAction.h"
#include "dbcomm/Row.h"

namespace COMMON
{
    namespace DBCOMM
    {
        DbQueryRslt::DbQueryRslt(tr1::shared_ptr<DbAction> action, tr1::shared_ptr<DbEngine> engine)
            :DbRslt(action) 
        {
            current_fetch_db_index_ = 0;
            engine_ = engine;

            db_locations_ = action_.lock()->GetDbLocations();
            
            query_action_ = tr1::static_pointer_cast<DbQueryAction>(action_.lock()); 
        }

        Row DbQueryRslt::Fetch(bool& success)
        {
            Row rsltRow;
            success = true;

            if (current_fetch_db_index_ < db_locations_.size())
            {
				// There are more results to traverse
                do 
                {
                    rsltRow = Fetch(&(db_locations_[current_fetch_db_index_]), success);

                    if (success)
                    {
                        if (((char**)rsltRow) == 0)
                        {
                            // go to the next target
                            current_fetch_db_index_++;
                        }
                    }
                    else
                    {
                        // error 
                        break;
                    }

                } while (((char**)rsltRow) == 0 && current_fetch_db_index_ < db_locations_.size());
            }

            return rsltRow;
        }

        Row DbQueryRslt::Fetch(const DbLocation* target, bool& success)
        {
            Row rslt;
            success = true;

            DbActionFilter filter;
            void* t = engine_->SyncDo(DbEngine::ActionTypeDef::FETCH, target, &filter, success);

            if (success)
            {
                rslt = Row((char**)(t), query_action_->GetColumnStringIndex(*const_cast<DbLocation*>(target)) );	
            }

            return rslt;
        }

        map<DbLocation, Row> DbQueryRslt::Fetch(vector<DbLocation*>& locations, bool& success, bool getNull)
        {
            map<DbLocation, Row> rslt;
            success = true;

            // simulate an input
            DbActionFilter filter;
            map<DbLocation, DbActionFilter*> input;
            for (int i = 0; i < locations.size(); i++)
            {
                input[*(locations[i])] = &filter;
            }
            map<DbLocation*, void*> t = engine_->SyncDo(DbEngine::ActionTypeDef::FETCH, input, success);

            if (success)
            {
                map<DbLocation*, void*>::iterator it = t.begin();
                for (; it != t.end(); it++)
                {
                    if (it->second == 0 && !getNull)
                    {
                        continue;
                    }
                    rslt[*(it->first)] = Row((char**)(it->second), query_action_->GetColumnStringIndex(*(it->first)));
                }	
            }

            return rslt;
        }

        unsigned long* DbQueryRslt::GetCurrentRowColumnsLength(bool& success)
        {
            unsigned long* rsltColumnLength = 0;
            success = true;

            rsltColumnLength = GetCurrentRowColumnsLength(&(db_locations_[current_fetch_db_index_]), success);

            return rsltColumnLength;
        }

        unsigned long* DbQueryRslt::GetCurrentRowColumnsLength(const DbLocation* target, bool& success)
        {
            unsigned long* rsltColumnLength = 0;
            success = true;

            DbActionFilter f;
            void* t = engine_->Do(DbEngine::ActionTypeDef::GET_COLUMNS_LENGTHS, target, &f, success);

            if (success)
            {
                rsltColumnLength = (unsigned long*)(t);	
            }

            return rsltColumnLength;
        }

        map<DbLocation, unsigned long*> DbQueryRslt::GetCurrentRowColumnsLength(vector<DbLocation*>& locations, bool& success, bool getNull)
        {
            map<DbLocation, unsigned long*> rslt;
            success = true;

            // simulate an input
            DbActionFilter filter;
            map<DbLocation, DbActionFilter*> input;
            for (int i = 0; i < locations.size(); i++)
            {
                input[*(locations[i])] = &filter;
            }
            map<DbLocation*, void*> t = engine_->Do(DbEngine::ActionTypeDef::GET_COLUMNS_LENGTHS, input, success);

            if (success)
            {
                map<DbLocation*, void*>::iterator it = t.begin();
                for (; it != t.end(); it++)
                {
                    if (it->second == 0 && !getNull)
                    {
                        continue;
                    }
                    rslt[*(it->first)] = (unsigned long*)(it->second);
                }	
            }

            return rslt;
        }
    }
}