/// @file DB2StmtGen.h
/// @brief The file defines some statment generator for DB2

/// @author Aicro Ai
/// @date 2015/4/2

#ifndef DBCOMM_DB2_STATEMENT_GENERATOR_H_
#define DBCOMM_DB2_STATEMENT_GENERATOR_H_

#ifdef DB2_ENV_AVAILABLE

#include <string>
#include <vector>
#include <map>

#include "dbcomm/DbLocation.h"
#include "dbcomm/StmtGenerator.h"
#include "thread/Mutex.h"

using namespace std;
using namespace COMMON::THREAD;

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief INNER USE ONLY. The class is used to globally buffer primary keys for all DB2 tables
        class DB2PriKeys
        {
        private:
			// The primary key buffers. 
			// The KEY is a string composed by connected DB information and table name.
			// The VALUE is the columns composing the primary keys.
            static map<string, vector<string> > primary_keys_;

            // The mutex to ensure that the global buffer for primary keys may be sequenced accessed
            static Mutex pri_key_mutex_;
            
        public:
            // The method is to get the primary keys
            static vector<string> GetPriKeys(const DbLocation& location, const string& tblName);
            
            // The method is to set the primary keys by connected DB information, table name
            static void SetPriKeys(const DbLocation& location, const string& tblName, vector<string>& priKeys);    
        };
        
        /// @brief INNER USE ONLY. The class is to generate a INSERT-UPDATE statement.
		/// For DB2, that will be MERGE INTO statement.
        class DB2ReplaceStmtGen : public StmtGenerator
        {
        public:
            virtual string FormStatement(const DbLocation& dbLocation);
        };
        
        /// @brief INNER USE ONLY. The class is to generate a INSERT-IGNORE statement.
		/// For DB2, that will be MERGE INTO statement.
        class DB2InsertIgnoreStmtGen : public StmtGenerator
        {
        public:
            virtual string FormStatement(const DbLocation& dbLocation);
        };
        
        /// @brief 内部类型，主要用于拼凑出对应DB2的获取主键语句。
		/// @brief INNER USE ONLY. The class is to generate a statement to get the primary key
		/// by querying the meta data.
        class DB2GetPriKeyStmtGen : public StmtGenerator
        {
        public:
            virtual bool MakeupStatement(vector<string>&, string, string , bool);
            
            virtual string FormStatement(const DbLocation& dbLocation);
        };
    }
}
#endif
#endif