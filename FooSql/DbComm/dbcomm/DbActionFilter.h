#ifndef DBACTION_FILTER_H_
#define DBACTION_FILTER_H_

#include <string>
#include <stdio.h>
#include <sstream>


#include "dbcomm/CommDef.h"

using namespace std;
namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief Filters for actions. Filter is such a class that acts as a container
		/// for different commands, usually they are SQL statements
        class DbActionFilter
        {
        public:
			/// @brief Constructor
            DbActionFilter()
                : addition_info_(0)
            {
            }
        
			/// @brief Constructor
			/// @param commands the commands to send
			/// @param additionInfo additional information to send, usually it is ignored
            DbActionFilter(string contents, void* additionInfo = 0) 
            {   
                addition_info_ = additionInfo;
                contents_ << contents;
            }

			/// @brief Constructor
			/// @param commands the commands to send
			/// @param length the length of the command buffer
			/// @param additionInfo additional information to send, usually it is ignored
            DbActionFilter(const char* contents, long length, void* additionInfo = 0) 
            { 
                addition_info_ = additionInfo;
                contents_.write(contents, length);
            }
    
			/// @brief Copy Constructor
			/// @param other the other instance to copy
            DbActionFilter(const DbActionFilter& other) 
            {   
                *this = other;
            }
    
			/// @brief Assign operation
			/// @param other the other instance to copy
            DbActionFilter& operator=(const DbActionFilter& other) 
            { 
                if (this == &other)
                {
                    return *this;
                }
    
                SetContents(other.contents_.str());
                addition_info_ = other.addition_info_;
            }
    
			/// @brief Explicitly set the commands
			/// @param commands the commands to send
			/// @param length the length of the command buffer
            void SetContents(const char* contents, long length) 
            { 
                contents_.str("");
                contents_.write(contents, length);
            }
    
			/// @brief Explicitly set the commands
			/// @param commands the commands to send
            void SetContents(const string contents) 
            { 
                SetContents(contents.c_str(), contents.size());
            }
			
			/// @brief Get the inner commands
			/// @return inner commands
            string GetContents()
            {
            	return contents_.str();
            }
            
			/// @brief Get the inner additional information if any
			/// @return the inner additional information if any
            void* GetAdditionalInfo()
            {
                return addition_info_;
            }
            
			/// @brief Explicitly set the inner additional information
			/// @param additionInfo the additional information to set
            void SetAdditionalInfo(void* additionInfo)
            {
                addition_info_ = additionInfo;
            }
            
        private:
            stringstream contents_;
            
            void* addition_info_;
        };

        // Execute
        
        /// @brief A filter for common execute
        typedef DbActionFilter ExecuteFilter;
        
		/// @brief A filter for delete
        typedef DbActionFilter DeleteFilter;
        
		/// @brief A filter for truncate
        typedef DbActionFilter TruncateFilter;
        
		/// @brief A filter for update
        typedef DbActionFilter UpdateFilter;
        
		/// @brief A filter for insert
        typedef DbActionFilter InsertFilter;
        
        // Query
        
		/// @brief A filter for query
        typedef DbActionFilter QueryFilter;

		/// @brief A filter for get primary key
        typedef DbActionFilter DbGetPriKeysFilter;
        
        // Commit Filter
        /// @brief A filter for commit
        typedef DbActionFilter CommitFilter;
        
		/// @brief A filter for escape string
        typedef DbActionFilter EscapeSrtingFilter;        
    }
}

#endif