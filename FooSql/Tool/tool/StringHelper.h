/// @file StringHelper.h
/// @brief The file defines some basic helper methods to operate a string.

/// @author Aicro Ai
/// @date 2015/03/17
 
#ifndef COMMON_TOOL_STRING_HELPER_H_
#define COMMON_TOOL_STRING_HELPER_H_

#include <string>
#include <vector>

using namespace std;

namespace COMMON
{
    namespace TOOL
    {
        class StringHelper
        {
        public:
            static void Split(vector<string> &tokens, const string &text, char delimiter);
            
            // if you need to identify a space and a "\t", 
			// the second parameter should be identified as " \t"
            static string TrimLeft(string& text, string toTrim = " ");
            
            static string TrimLeft(const char* text, const char* toTrim = " ");
            
            static string TrimRight(string& text, string toTrim = " ");
            
            static string TrimRight(const char* text, const char* toTrim = " ");
            
            static string Trim(string& text, string toTrim = " ");
            
            static string Trim(const char* text, const char* toTrim = " ");
            
            static string ToLower(string& text);
            
            static string ToUper(string& text);
        };
    }
}
#endif
