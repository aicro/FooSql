/// @file MysqlStmtGen.h
/// @brief The file defines some statement generators for MYSQL, this should be used only internally.

/// @author Aicro Ai
/// @date 2015/4/2

#ifndef DBCOMM_MYSQL_STATEMENT_GENERATOR_H_
#define DBCOMM_MYSQL_STATEMENT_GENERATOR_H_

#ifdef MYSQL_ENV_AVAILABLE

#include "dbcomm/StmtGenerator.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief INTERNAL USE ONLY. The class is defined to generate replace statement, 
		///	a MYSQL insert-update statement.
        class MysqlReplaceStmtGen : public StmtGenerator
        {
        public:
            virtual string FormStatement(const DbLocation& dbLocation);
        };
        
		/// @brief INTERNAL USE ONLY. The class is defined to generate a insert-ignore statement, 
		///	a MYSQL way to express the meaning that if a duplicate key exception met in the insert,
		/// just ignore this exception and continue for other statement.
        class MysqlInsertIgnoreStmtGen : public StmtGenerator
        {
        public:
            virtual string FormStatement(const DbLocation& dbLocation);
        };
        
        /// @brief 内部类型，主要用于拼凑出对应MYSQL的获取主键语句。
		/// @brief INTERNAL USE ONLY. The class is defined to generate a statement to get the primary key
		/// by querying meta data in MYSQL.
        class MysqlGetPriKeyStmtGen : public StmtGenerator
        {
        public:
            virtual bool MakeupStatement(vector<string>& , string , string values, bool); 
        
            virtual string FormStatement(const DbLocation& dbLocation);
        };
    }
}

#endif

#endif