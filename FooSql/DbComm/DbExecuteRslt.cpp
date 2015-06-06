#include "dbcomm/DbExecuteRslt.h"
#include "dbcomm/DbTasks.h"

#include "exception/CodingException.h"

namespace COMMON
{
    namespace DBCOMM
    {
        DbExecuteRslt::DbExecuteRslt(
            tr1::shared_ptr<DbAction> action, 
            map<DbLocation, long long>& affected_rows)
            :DbRslt(action)
        {
            map<DbLocation, long long>::iterator it = affected_rows.begin();
            for (; it != affected_rows.end(); it++)
            {
                affected_rows_[it->first] = it->second;
            }
        }

        map<DbLocation, long long> DbExecuteRslt::GetAffectedRows( bool& success )
        {
            success = true;
            return affected_rows_;
        }

        long long DbExecuteRslt::GetAffectedRows(DbLocation* location, bool& success) throw (COMMON::EXCEPTION::ThrowableException)
        {
            long long rslt = 0;
            success = true;

            if (affected_rows_.find(*location) != affected_rows_.end())
            {
                rslt = affected_rows_[*location];
            }
            else
            {
                success = false;

				// errors are found in the input parameters
                tr1::shared_ptr<COMMON::EXCEPTION::IException> inner_e(
                    new COMMON::EXCEPTION::ObjectNotExistingInContainerException(location->ToString()));		
                if (action_.lock()->IsExceptionMode())
                {
                    COMMON::EXCEPTION::ThrowableException e(inner_e);
                    throw e;
                }
                else
                {
                    tr1::shared_ptr<COMMON::EXCEPTION::ThrowableException> e(new COMMON::EXCEPTION::ThrowableException(inner_e));
                    SetException(e);
                }
            }
            
            return rslt;
        }

        map<DbLocation, long long> DbExecuteRslt::GetAffectedRows(vector<const DbLocation*>& locations, bool& success) throw (COMMON::EXCEPTION::ThrowableException)
        {
            map<DbLocation, long long> rslt;
            success = true;

            vector<const DbLocation*>::iterator it = locations.begin();
            for (; it != locations.end(); it++)
            {
                if (affected_rows_.find(**it) != affected_rows_.end())
                {
                    rslt[**it] = affected_rows_[**it];
                }
                else
                {
                    success = false;

                    // errors are found in the input parameters
                    tr1::shared_ptr<COMMON::EXCEPTION::IException> inner_e(
                        new COMMON::EXCEPTION::ObjectNotExistingInContainerException((*it)->ToString()) );
                    if (action_.lock()->IsExceptionMode())
                    {
                        COMMON::EXCEPTION::ThrowableException e( inner_e );
                        throw e;
                    }
                    else
                    {
                        tr1::shared_ptr<COMMON::EXCEPTION::ThrowableException> e(new COMMON::EXCEPTION::ThrowableException(inner_e));
                        SetException(e);
                    }

                    rslt.clear();
                    break;
                }
            }

            return rslt;
        }        
    }
}