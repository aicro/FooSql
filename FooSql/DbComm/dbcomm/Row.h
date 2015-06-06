/// @file Row.h
/// @brief The file defines a wrapper class for holding data representing a row.

/// @author Aicro Ai
/// @date 2015/2/19

#ifndef ROW_H_
#define ROW_H_

#include <string>
#include <map>
#include <tr1/memory>

#include "tool/StringHelper.h"

#include "exception/CodingException.h"
#include "exception/ThrowableException.h"

using namespace std;
using namespace COMMON::EXCEPTION;

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief A wrapper class for data representing a row queried from a SELECT statement.
        class Row
        {
        public:
            /// @brief Constructor
            /// @param row the raw data retrieved from DBMS by a SELECT statement
            /// @param column_index a map for column name and its associated position
            Row(char** row, map<string, int>* column_index);

            /// @brief Default constructor
            Row();
            
            /// @brief Copy constructor
			/// @param other the @c Row instance to copy
            Row(const Row& row);

            /// @brief Assignment operation
			/// @param other the @c Row instance to copy
            Row& operator=(const Row& other);

            /// @brief For compatibility. Implicitly convert the instance to its raw data.
            operator char**();

            /// @brief Get the column value by using the column name
            /// @param column the column name
            /// @return The column value. 
			/// @note If the column name specified cannot be found, an exception will be thrown out
            const char* operator [](string column) const throw(ThrowableException);

            /// @brief Get the column value by using position
            /// @param index the position, begin with 0
            /// @return The column value. 
            const char* operator [](int index);
            
            /// @brief Get the position of the specified column.
            /// @param colName column name
            /// @return The corresponding position index of the column name, if not found, -1 will be returned.
            int GetIndexByColName(string colName) const throw(ThrowableException);

        private:
            string FormCandidates() const;
            
        private:
            char** row_;
            map<string, int>* column_index_;
        };
    }
}


#endif

