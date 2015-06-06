#include <sstream>
#include "dbcomm/Row.h"

namespace COMMON
{
    namespace DBCOMM
    {
        Row::Row(char** row, map<string, int>* column_index)
                : row_(row), column_index_(column_index)
        {
        }

        Row::Row()
            : row_(0), column_index_(0)
        {
        }
        
        Row::Row(const Row& row)
        {
            *this = row;
        }

        Row& Row::operator=(const Row& other)
        {
            if (this == &other)
            {
                return *this;
            }

            row_ = other.row_;
            column_index_ = other.column_index_;
            return *this;
        }

        Row::operator char**()
        {
            return row_;
        }

        const char* Row::operator [](string colName) const throw(ThrowableException)
        {
            TOOL::StringHelper::ToLower(colName);
            TOOL::StringHelper::Trim(colName, " \t");

            if (column_index_->find(colName) == column_index_->end())
            {
                tr1::shared_ptr<IException> inner_e(
                    new ObjectNotExistingInContainerException(colName, string("valid items are as follows : ") + FormCandidates()));		
                ThrowableException e(inner_e);
                
                throw e;
            }

            return row_[(*column_index_)[colName]];
        }

        const char* Row::operator [](int index)
        {
            return row_[index];
        }
        
        int Row::GetIndexByColName(string colName) const throw(ThrowableException)
        {
            TOOL::StringHelper::ToLower(colName);
            TOOL::StringHelper::Trim(colName, " \t");
            
            if (column_index_->find(colName) == column_index_->end())
            {
                tr1::shared_ptr<IException> inner_e(
                    new ObjectNotExistingInContainerException(colName, string("The valid items are ") + FormCandidates()));		
                ThrowableException e(inner_e);
                
                throw e;
            }
            
            return (*column_index_)[colName];
        }      
		
        string Row::FormCandidates() const
        {
            stringstream ss;
            map<string, int>::iterator it = column_index_->begin();
            while (it != column_index_->end())
            {
                ss << it->first << "(" << it->second << "),";
                it++;
            }
            return ss.str();
        }
    }
}