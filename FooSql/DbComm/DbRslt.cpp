#include "dbcomm/DbLocation.h"
#include "dbcomm/DbRslt.h"

namespace COMMON
{
    namespace DBCOMM
    {
        DbRslt::DbRslt(tr1::shared_ptr<DbAction> action)
            :action_(action)
        {
        }

        DbRslt::~DbRslt()
        {
        }

        string DbRslt::GetLastError()
        { 
            return action_.lock()->GetLastError(); 
        }

        void DbRslt::SetException(tr1::shared_ptr<COMMON::EXCEPTION::ThrowableException> e)
        { 
            action_.lock()->SetException(e); 
        }
    }
}

