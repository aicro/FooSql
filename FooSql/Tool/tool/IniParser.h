/// @file IniParser.h
/// @brief The file defines a simple INI reader tool.

#ifndef _COMMON_TOOL_INI_PARSER_H__
#define _COMMON_TOOL_INI_PARSER_H__

#include <map>
#include <string>

#include "exception/ThrowableException.h"

using namespace std;
using namespace COMMON::EXCEPTION;

namespace COMMON
{
    namespace TOOL
    {
        /// @brief A class to read a name/value pair from an INI file.
        class IniParser
        {
        public:
            /// @brief Constructor
            /// @param filename A full path to a INI file.
            IniParser(string filename) throw (ThrowableException);

            /// @brief Return the line number and detail error msg
            /// @param errMsg Detail for the error.
            /// @return the line number lies the error.
            int ParseError(string& errMsg);

            /// @brief Convert the value extracted from the specified section and property to a string.
            /// @param section The section name.
            /// @param name The property name.
            /// @return The string represents the value extracted from the specified section and property
            string GetString(string section, string name) throw (ThrowableException);

            /// @brief Convert the value extracted from the specified section and property to an integer.
			/// If the value can not be converted to an integer, an exception will be thrown out.
            /// @param section The section name.
            /// @param name The property name.
            /// @return An integer represents the value extracted from the specified section and property
            long GetInteger(string section, string name) throw (ThrowableException);

            /// @brief Convert the value extracted from the specified section and property to a double type.
			/// If the value can not be converted to a double, an exception will be thrown out.
            /// @param section The section name.
            /// @param name The property name.
            /// @return A real number represents the value extracted from the specified section and property
            double GetReal(string section, string name) throw (ThrowableException);

            /// @brief Convert the value extracted from the specified section and property to a boolean type.
			/// If the value can not be converted to a boolean, an exception will be thrown out.
            /// @param section The section name.
            /// @param name The property name.
            /// @return A boolean represents the value extracted from the specified section and property
			/// @note The following words in the INI file can be converted to a true value. 
			/// "true", "yes", "on", "1", "false", "no", "off", "0".
            bool GetBoolean(string section, string name) throw (ThrowableException);

        private:
            int Parse(FILE* file, string& errMsg);
            
            // remove the right space.
            char* RightStrip(char* s);
            
			// remove the left space.
            char* LeftSkip(const char* s);
            
			// find a specified character or ";" in the input string.
			// if neither can be found, a NULL will be returned.
            char* FindCharOrComment(const char* s, char c);
            
            char* Strncpy0(char* dest, const char* src, size_t size);
            
            static string MakeKey(string section, string name);
            
            void Set(const char* section, const char* name, const char* value);
            
            string FormReason(string key, string type);
            
        private:
            static const int MAX_LINE_IN_BYTES_;
            static const int MAX_SECTION_IN_BYTES_;
        
            string file_name_;
            int error_line_;
            int err_no_;
            string err_msg_;
            map<string, string> values_;
        };        
    }
}

#endif