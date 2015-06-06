#include <sstream>

#include "dbcomm/StmtGenerator.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /////////////////////////////////////////////////
        ///// StmtGenerator
        /////////////////////////////////////////////////
        StmtGenerator::StmtGenerator()
            : clear_all_(false), has_value_(false)
        {
        }
        
        void StmtGenerator::ClearContent()
        {
            // VALUE值是一定要清空的
            values_.str("");
            has_value_ = false;
            
            if (clear_all_)
            {
                col_list_ = "";
                table_name_ = "";
                columns_.clear();    
            }            
        }

        string StmtGenerator::GetTblName()
        {
            return table_name_;
        }
        
        bool StmtGenerator::MakeupStatement( vector<string>& columns, string tableName, string values, bool forceCheck )
        {
            clear_all_ = false;
            
            // if an empty input was given, skip it
            if (columns.size() == 0 || values == "" || tableName == "")
            {
                return false;
            }
            
            // check the new input commands are compatible with the exsiting ones
            if (forceCheck)
            {
            	if (    col_list_ != "" // we have assigned column list before
            	    && (columns.size() != columns_.size() || table_name_ != tableName) ) // incompatible
            	{
            	    clear_all_ = true;
            	    return true;
				}
            }
            
            if (forceCheck || col_list_ == "")
			{			
            	// the newly input column lists are the same with the old?
				string new_col_list = "";
            	int i = 0;
           		for (;i < columns.size() - 1; i++)
	            {
    	            new_col_list += (columns[i] + ",");
        	    }
            	new_col_list += columns[i];

	            if (col_list_ != "" && new_col_list != col_list_)
    	        {
        	        clear_all_ = true;
            	    return true;
	            }

    	        // now we have confirmed that the column lists are compatible
        	    col_list_ = new_col_list;
			}
			
            table_name_ = tableName;

            if (columns_.size() == 0)
            {
                columns_.resize(columns.size());
                std::copy(columns.begin(), columns.end(), columns_.begin());    
            }

            if (has_value_ == true)
            {
                values_ << ",";
            }
            values_ << "(" << values << ")";
            has_value_ = true;
            
            return false;
        }

        string StmtGenerator::FormOneList( vector<string>& toForm , string prefix)
        {
            stringstream ss;
            
            ss << "(";
            for (int i = 0; i < toForm.size(); i++)
            {
                if (prefix != "")
                {
                    ss << (prefix + ".");
                }
                ss << toForm[i];

                if (i != toForm.size() - 1)
                {
                    ss << ",";
                }
            }
            ss << ")";

            return ss.str();
        }

        ////////////////////////////////////////////////////////
        //// InsertStmtGen
        ////////////////////////////////////////////////////////
        string InsertStmtGen::FormStatement(const DbLocation& dbLocation)
        {
            if (has_value_ == false)
            {
                return "";
            }
            
            stringstream statement;
            statement << "INSERT INTO " << table_name_ << " (" << col_list_ << ") VALUES";
            statement << values_.str();

            return statement.str();
        }
    }
}

