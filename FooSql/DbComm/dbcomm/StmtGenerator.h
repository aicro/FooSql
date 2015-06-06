/// @file StmtGenerator.h
/// @brief The file defines a helper class that should be use internally. 
/// This class represents a statement generator, the extend classes of which 
/// may help to generate a INSERT or other related statements that can be worked 
/// together with @c BatchAction.

/// @author Aicro Ai
/// @date 2015/4/2

#ifndef DBCOMM_STATEMENT_GENERATOR_H_
#define DBCOMM_STATEMENT_GENERATOR_H_

#include <vector>
#include <string>

#include "dbcomm/DbLocation.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief This class is only for inner use, the main purpose of which is to generate 
		/// valid part of statements such as multiple value insert. Therefore, this class will
		/// only be used with @c BatchAction or its heritage classes.
		/// @note Note that the class is an abstract one. All the concrete batch (or multiple value) 
		/// statement generator should extend from it.
        class StmtGenerator
        {
        protected:
            string col_list_; // column list separated by ',', such as "col1, col2, col3"
            string table_name_; // table name
            vector<string> columns_; // Columns name lists
            stringstream values_; // 以单列的方式给出的值(已含有语法信息,例如'20150402'中的"'")的集合
            bool has_value_; // 是否有过value

            bool clear_all_; // should we clear all the existing contents? This may be set to true when an incompatible values are input.
            
        public:
            StmtGenerator();
        
            /// @brief Get the inner table name
			/// @return table name
            virtual string GetTblName();
        
            /// @brief clear all the contents
            virtual void ClearContent();

            /// @brief Make up a partial statement. This function will check whether columns and values 
			/// input are compatible with that of buffered. And if it is compatible, the function will
			/// will append the new values to the existing ones. If it is not, it just return a signal
			/// to inform the user of the incompatibility.
            /// @param columns a list of column names
            /// @param tableName table name
            /// @param values a list of values with their correct format in a SQL statment
            /// @return Whether the input value is compatible with the existing ones.
            /// -true the input value is not compatible with the existing one, therefore we need to process the existing ones first.
            /// -false the input value is compatible with the existing one, no extra work should be done
            virtual bool MakeupStatement(vector<string>& columns, string tableName, string values, bool forceCheck = true);
            
            /// @brief Form a complete statement based on elements inside the instance.
			/// If there is nothing passed before, an empty statement ("") will be returned.
            /// @param dbLocation The location of the target DB
            /// @return A complete SQL statement. If there is nothing passed before, an empty statement ("") will be returned.
            virtual string FormStatement(const DbLocation& dbLocation) = 0;
            
        protected:
            // form a statement like "([prefix.]xxx, [prefix.]bbb, [prefix.]ccc)", 
            string FormOneList(vector<string>& toForm, string prefix = "");
        };

        /// @brief Internal use only. The class is designed for generate an INSERT statement.
        class InsertStmtGen : public StmtGenerator
        {
        public:
            virtual string FormStatement(const DbLocation& dbLocation);
        };
    }
}

#endif