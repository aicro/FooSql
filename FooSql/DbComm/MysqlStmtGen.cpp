#include <sstream>

#include "dbcomm/MysqlStmtGen.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /////////////////////////////////////////
        // MysqlReplaceStmtGen
        /////////////////////////////////////////
        string MysqlReplaceStmtGen::FormStatement(const DbLocation& dbLocation)
        {
            if (has_value_ == false)
            {
                return "";
            }
            
            stringstream statement;
            statement << "REPLACE INTO " << table_name_ << " (" << col_list_ << ") VALUES";

            statement << values_.str();

            return statement.str();
        }
        
        /////////////////////////////////////////
        // MysqlInsertIgnoreStmtGen
        /////////////////////////////////////////
        string MysqlInsertIgnoreStmtGen::FormStatement(const DbLocation& dbLocation)
        {
            if (has_value_ == false)
            {
                return "";
            }
            
            stringstream statement;
            statement << "INSERT IGNORE INTO " << table_name_ << " (" << col_list_ << ") VALUES";

            statement << values_.str();

            return statement.str();
        }
        
        /////////////////////////////////////////
        // MysqlGetPriKeyStmtGen
        /////////////////////////////////////////
        bool MysqlGetPriKeyStmtGen::MakeupStatement(vector<string>& columns, string tableName, string values, bool)
        {
            table_name_ = tableName;
            
            // always compatible
            return false;
        }
        
        string MysqlGetPriKeyStmtGen::FormStatement(const DbLocation& dbLocation)
        {
            if (GetTblName() == "")
            {
                return "";
            }
            
            stringstream statement;
            statement << "SELECT COLUMN_NAME AS COL_NAME from INFORMATION_SCHEMA.COLUMNS WHERE "
                      << "COLUMN_KEY = 'PRI' AND TABLE_NAME = '" << GetTblName() << "' AND table_schema = '" << dbLocation.GetDbId() << "'";

            return statement.str();
        }
    }
}



