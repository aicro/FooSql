/// @file DbQueryRslt.h
/// @brief The file defines a class representing the result of a query.

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DBQUERYRSLT_H_
#define COMMON_DBCOMM_DBQUERYRSLT_H_

#include <map>

#include "dbcomm/CommDef.h"
#include "dbcomm/DbRslt.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        class DbEngine;
        class Row;
        class DbQueryAction;
        
        /// @brief The class representing the result of the query
        class DbQueryRslt : public DbRslt
        {
        public:
            /// @brief Constructor
            /// @param action the Action object from which this result comes from
            /// @param engine the underlying @c DbEngine of the parent @c DbAction
            DbQueryRslt(tr1::shared_ptr<DbAction> action, tr1::shared_ptr<DbEngine> engine);

            /// @brief Fetch a row from all the connections. The method will switch to the next connection
			/// automatically when it comes to the end of the current connection.
			/// @param success success or not
            /// @return the next row. If there is no more rows left, an object with 0 inside will be returned.
            virtual Row Fetch(bool& success);

            /// @brief Fetch a row from a specific connection.
			/// @param target the connection from which to get the next row
			/// @param success success or not
            /// @return the next row. If there is no more rows left, an object with 0 inside will be returned.
            virtual Row Fetch(const DbLocation* target, bool& success);

            /// @brief Fetch a row from specific connections respectively.
			/// @param locations the connections from which to get the next row
			/// @param success success or not
			/// @param getNull true means to put a Row with 0 inside into the return object, false means ignore such cases
            /// @return a map of connections and its rows fetched
            virtual map<DbLocation, Row> Fetch(vector<DbLocation*>& locations, bool& success, bool getNull = false);

            /// @brief Get the actual lengths of each columns of the current row.
			/// @param success success or not
            /// @return a list of actual lengths to each columns orderly
            virtual unsigned long* GetCurrentRowColumnsLength(bool& success);

            /// @brief Get the actual lengths of each columns of the current row of a specific connection.
			/// @param target the connection from which to get the next row
			/// @param success success or not
            /// @return a list of actual lengths to each columns orderly
            virtual unsigned long* GetCurrentRowColumnsLength(const DbLocation* target, bool& success);

            /// @brief Fetch the actual lengths to each columns of the current row from specific connections respectively.
			/// @param locations the connections from which to get the lengths of columns
			/// @param success success or not
			/// @param getNull true means to put a Row with 0 inside into the return object, false means ignore such cases
            /// @return a map of connections and its associated column lengths
            virtual map<DbLocation, unsigned long*> GetCurrentRowColumnsLength(
                vector<DbLocation*>& locations, bool& success, bool getNull = false);

        protected:
            // the current DB index from which to fetch the next row
            int current_fetch_db_index_;

            // a buffer for all the connections
            vector<DbLocation> db_locations_;

            // the buffer to the engine
            tr1::shared_ptr<DbEngine> engine_;

            // buffer to avoid from calling static_cast
            tr1::shared_ptr<DbQueryAction> query_action_;
        };
    }
}


#endif

