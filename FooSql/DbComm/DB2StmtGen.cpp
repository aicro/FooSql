#ifdef DB2_ENV_AVAILABLE

#include <sstream>

#include "dbcomm/DB2DbTasks.h"
#include "dbcomm/Row.h"
#include "dbcomm/DB2StmtGen.h"

#include "thread/MutexLockGuard.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        ///////////////////////////////////////////////
        //// DB2PriKeys
        ///////////////////////////////////////////////
        map<string, vector<string> > DB2PriKeys::primary_keys_;
        Mutex DB2PriKeys::pri_key_mutex_;
        
        void DB2PriKeys::SetPriKeys(const DbLocation& location, const string& tblName, vector<string>& priKeys)
        {
            stringstream ss;
            ss << location.ToString() << tblName;
            
            MutexLockGuard g(pri_key_mutex_);
            primary_keys_[ss.str()].resize(priKeys.size());
            std::copy(priKeys.begin(), priKeys.end(), primary_keys_[ss.str()].begin());
        }
        
        vector<string> DB2PriKeys::GetPriKeys(const DbLocation& location, const string& tblName)
        {
            stringstream ss;
            ss << location.ToString() << tblName;
            
            bool found = true;
            {
                MutexLockGuard g(pri_key_mutex_);
                found = (primary_keys_.end() != primary_keys_.find(ss.str()));
            }
            
            if (!found)
            {
                // no found before? get it
                vector<DbLocation> dbLocations_array;
                dbLocations_array.push_back(location);
                tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
                db2Tasks->Connect();
                DbQueryAction* query_action = db2Tasks->GetPriKeys();
                DbGetPriKeysFilter filter(tblName);
                query_action->Do(&filter);
                DbQueryRslt* query_rslt = (DbQueryRslt*)query_action->GetRslt();

                vector<string> pri_keys;
                Row rslt;
                bool success = false;
                while ((char**)(rslt = query_rslt->Fetch(success)) != NULL) 
                {
                    pri_keys.push_back(rslt[0]);
                }
                DB2PriKeys::SetPriKeys(location, tblName, pri_keys); 
                
                query_action->EndAction();
                db2Tasks->Disconnect();
                
                return pri_keys;
            }
            else
            {
                MutexLockGuard g(pri_key_mutex_);
                return primary_keys_[ss.str()];
            }
        }
        
        ///////////////////////////////////////////////
        //// DB2ReplaceStmtGen
        ///////////////////////////////////////////////
        string DB2ReplaceStmtGen::FormStatement(const DbLocation& dbLocation)
        {
            if (has_value_ == false)
            {
                return "";
            }
            
			// get the primary key, if none, throw an exception
            vector<string> pri_key = DB2PriKeys::GetPriKeys(dbLocation, table_name_);
            
			// generate an "merge into" statement
            stringstream ss;
            ss << "MERGE INTO " << table_name_ << " AS T USING ( "
                << "SELECT * FROM TABLE ( VALUES " << values_.str() << ")"
                << ") AS TMPTABLE(" << col_list_ << ") ON "
                << "(";
            
            for (int i = 0; i < pri_key.size(); i++)
            {
                ss << "T." << pri_key[i] << "=TMPTABLE." << pri_key[i];
                if (i != pri_key.size() - 1)
                {
                    ss << " AND ";
                }
            }
            
            ss << ") ";
            ss << "WHEN MATCHED THEN UPDATE SET ";
            for (int i = 0; i < columns_.size(); i++)
            {
                ss << columns_[i] << " = TMPTABLE." << columns_[i];

                if (i != columns_.size() - 1)
                {
                    ss << ",";
                }
            }
            ss << " WHEN NOT MATCHED THEN INSERT (" << col_list_ << ")";
            ss << " VALUES" << FormOneList(columns_,"TMPTABLE");

            return ss.str();
        }
        
        ///////////////////////////////////////////////
        //// DB2InsertIgnoreStmtGen
        ///////////////////////////////////////////////
        string DB2InsertIgnoreStmtGen::FormStatement(const DbLocation& dbLocation)
        {
            if (has_value_ == false)
            {
                return "";
            }
            
            // get primary key, if none, throw exception
            vector<string> pri_key = DB2PriKeys::GetPriKeys(dbLocation, table_name_);
            
            stringstream ss;
            ss << "MERGE INTO " << table_name_ << " AS T USING ( "
                << "SELECT * FROM TABLE ( VALUES " << values_.str() << ")"
                << ") AS TMPTABLE(" << col_list_ << ") ON "
                << "(";
            
            for (int i = 0; i < pri_key.size(); i++)
            {
                ss << "T." << pri_key[i] << "=TMPTABLE." << pri_key[i];
                if (i != pri_key.size() - 1)
                {
                    ss << " AND ";
                }
            }
            
            ss << ") WHEN NOT MATCHED THEN INSERT (" << col_list_ << ")";
            ss << "  VALUES" << FormOneList(columns_,"TMPTABLE");

            return ss.str();
        }
        
        ///////////////////////////////////////////////
        //// DB2GetPriKeyStmtGen
        ///////////////////////////////////////////////
        bool DB2GetPriKeyStmtGen::MakeupStatement(vector<string>& columns, string tableName, string values, bool)
        {
            table_name_ = tableName;
            
            // 无需立即重做
            return false;
        }
        
        string DB2GetPriKeyStmtGen::FormStatement(const DbLocation& dbLocation)
        {
            string tbl_name = GetTblName();
            if (tbl_name == "")
            {
                return "";
            }
            
			// check whether the schema information is brought with the table name
            string schema = "";
            string real_tbl_name = tbl_name;
            size_t found = tbl_name.find_first_of(".");
            if (found !=std::string::npos)
            {
                schema = tbl_name.substr(0, found);
                real_tbl_name = tbl_name.substr(found + 1);
            }
            
            stringstream statement;
            statement << "SELECT b.NAME AS COL_NAME FROM SYSIBM.SYSTABLES a, SYSIBM.SYSCOLUMNS b "
                      << "WHERE a.NAME = b.TBNAME AND b.KEYSEQ = 1 AND UPPER(b.TBNAME) = UPPER('" << real_tbl_name << "')";
            if (schema != "")
            {
                statement << " AND UPPER(a.CREATOR) = UPPER('" << schema << "')";
            }
            
            return statement.str();
        }
    }
}

#endif