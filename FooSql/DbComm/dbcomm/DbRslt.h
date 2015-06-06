/// @file DbRslt.h
/// @brief The file defines a class representing the result set of a DB operation. This class is always used as a base class.

/// @author Aicro Ai
/// @date 2015/4/19

#ifndef COMMON_DBCOMM_DBRSLT_H_
#define COMMON_DBCOMM_DBRSLT_H_

#include <vector>
#include <string>
#include <tr1/memory>

#include "dbcomm/CommDef.h"

#include "dbcomm/DbEngine.h"
#include "dbcomm/DbAction.h"

#include "exception/ErrorBox.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        class DbAction;

        /// @brief The class representing the result set of a DB operation. 
		/// Here we only defines some methods about errors and exceptions.
        class DbRslt
        {
        protected:
            /// @brief the action from which this result comes
            tr1::weak_ptr<DbAction> action_;

        public:
            /// @brief Constructor
            /// @param action the point to the action from which this result comes
            DbRslt(tr1::shared_ptr<DbAction> action);
            
            virtual ~DbRslt();

            /// @brief Get the description of the last error. This method is usually used under "error to return mode"
            /// @return the description of the last error
            string GetLastError();

            /// @brief Set the last exception
            /// @param e the point to the exception
            void SetException(tr1::shared_ptr<COMMON::EXCEPTION::ThrowableException> e);
        };
    }
}

#endif