/// @file TypeCheck.h
/// @brief The file defines some functions to identify whether a string 
/// can be converted to a number type.

/// @author Aicro Ai
/// @date 2014/9/19
 
#ifndef TYPE_CHECK_H_
#define TYPE_CHECK_H_

#include <string>

using namespace std;

namespace COMMON
{
    namespace TOOL
    {
        /// @brief A class to judge whether a string can be converted to a number type.
        class TypeCheck
        {
        public:
            /// @brief To determin whether a string can be converted to a double type.
            /// @param s The string to be test
            /// @return Result
            /// -true The string can be converted to a double type.
            /// -false he string cannot be converted to a double type.
            static bool IsDouble(string s);
            
            /// @brief To determin whether a string can be converted to an integer type.
            /// @param s The string to be test
            /// @return Result
            /// -true The string can be converted to a integer type.
            /// -false he string cannot be converted to a integer type.
            static bool IsInteger(string s);
        };
    }
}
#endif
