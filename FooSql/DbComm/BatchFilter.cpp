#include "dbcomm/BatchFilter.h"

#include "tool/StringHelper.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        BatchFilter::BatchFilter()
            : force_check_(false), has_value_(false)
        {
        }
        
        BatchFilter::BatchFilter(const BatchFilter& other)
        {
            table_name_ = other.table_name_;
            columns_ = other.columns_;
            values_ << other.values_.str();
            has_value_ = other.has_value_;
            force_check_ = other.force_check_;
        }
        
        BatchFilter::BatchFilter( string table_name, map<string, Value>& column_value_map, bool forceCheck )
            : table_name_(table_name), force_check_(forceCheck), has_value_(false)
        {
            map<string, Value>::iterator it = column_value_map.begin();
            for (int i = 0; it != column_value_map.end(); it++, i++)
            {
                columns_.push_back(it->first);
                
                if (has_value_)
                {
                    values_ << ",";    
                }
                values_ << it->second.GetValue();    
                has_value_ = true;
            }
        }
        
        BatchFilter::BatchFilter(string table_name, bool forceCheck)
            : table_name_(table_name), force_check_(forceCheck), has_value_(false)
        {
        }
        
        void BatchFilter::AppendColumnValue(string column, Value value, bool ignoreColumn)
        {
            if (!ignoreColumn)
            {
                columns_.push_back(TOOL::StringHelper::Trim(column, " "));
            }
            
            if (has_value_)
            {
                values_ << ",";    
            }
            values_ << value.GetValue();    
            has_value_ = true;
        }
        
        void BatchFilter::AppendColumnValue(string column, string value, bool ignoreColumn)
        {
            if (!ignoreColumn)
            {
                columns_.push_back(TOOL::StringHelper::Trim(column, " "));
            }
            
            if (has_value_)
            {
                values_ << ",";    
            }
            values_ << value;    
            has_value_ = true;
        }
        
        void BatchFilter::SetTableName(string tableName)
        {
            table_name_ = tableName;
        }
        
        void BatchFilter::ClearColumns()
        {
            columns_.clear();
        }
            
        void BatchFilter::ClearValues()
        {
            values_.str("");
            has_value_ = false;
        }
        
        string BatchFilter::GetTableName()
        {
            return table_name_;
        }
        
        vector<string>& BatchFilter::GetColumns()
        {
            return columns_;
        }
        
        string BatchFilter::GetValues()
        {
            return values_.str();
        }        
        
        bool BatchFilter::CheckCompatible()
        {
            return force_check_;
        }
    }
}
