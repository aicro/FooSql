/// @file BatchFilter.h
/// @brief The file defines a filter for multiple value operation.

/// @author Aicro Ai
/// @date 2015/2/19

#ifndef COMMON_DBCOMM_BATCHFILTER_H_
#define COMMON_DBCOMM_BATCHFILTER_H_

#include <sstream>
#include <map>
#include <vector>
#include <string>

#include "dbcomm/DbActionFilter.h"
#include "dbcomm/Value.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
		/// @brief The class is a filter for batch value process, such as multi-value insert, insert-update.
		/// The class stores such information as columns and values.
        class BatchFilter : public DbActionFilter
        {
        protected:
            /// @brief table name
            string table_name_;

            /// @brief a list of column name
            vector<string> columns_;
            
            /// @brief a list of values
            stringstream values_;
            
            /// @brief whether we have value currently
            bool has_value_;
            
            /// @brief to force check the compatibility?
            bool force_check_;
        public:
			/// @brief Constructor.
			/// @param table_name table name
			/// @param column_value_map A map for columns and their associated value
			/// @param forceCheck is it necessary to check the compatibility of the new statement and the origin one?
            explicit BatchFilter(string table_name, map<string, Value>& column_value_map, bool forceCheck = false);
            
            /// @brief Default constructor
            explicit BatchFilter();
            
            /// @brief Copy Constructor
            /// @param other the other object to copy
            BatchFilter(const BatchFilter& other);
            
            /// @brief Constructor
            /// @param tableName affected table's name
            /// @param forceCheck whether to check the compatibility
            BatchFilter(string tableName, bool forceCheck = false);

            
        public:
			/// @brief Append column and its associate value seperately
			/// @param column column
            /// @param value value
			/// @param ignoreColumn should we ignore appending the column. It is useful
			/// when you want to save the time by not copying columns.
            void AppendColumnValue(string column, Value value, bool ignoreColumn);
            
			/// @brief Append column and its associate value seperately
			/// @param column column
            /// @param value value
			/// @param ignoreColumn should we ignore appending the column. It is useful
            void AppendColumnValue(string column, string value, bool ignoreColumn);
        
            /// @brief clear columns only
            void ClearColumns();
            
            /// @brief clear values only
            void ClearValues();
            
            /// @brief Set table name
            /// @param tableName the new table name
            void SetTableName(string tableName);
        
        public:
            /// @brief Get table name
            /// @return the table name
            string GetTableName();
            
			/// @brief Get the associate column names
			/// @return The associated column names.
            vector<string>& GetColumns();
            
			/// @brief Get associated values
			/// @return The associated values
            string GetValues();
            
            /// @brief should we need to check the compatibility?
            /// @return the necessity for checking the compatibility
            bool CheckCompatible();
        };
    }
}

#endif