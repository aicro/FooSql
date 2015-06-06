#include <sstream>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "exception/FileException.h"
#include "exception/CodingException.h"
#include "tool/IniParser.h"
#include "tool/StringHelper.h"

namespace COMMON
{
    namespace TOOL
    {
        const int IniParser::MAX_LINE_IN_BYTES_ = 512;
        const int IniParser::MAX_SECTION_IN_BYTES_ = 128;
        
        IniParser::IniParser(string filename) throw (ThrowableException)
        {
            bool has_error = true;
            do 
            {
                FILE* file;
                file = fopen(filename.c_str(), "r");
                
                if (file == 0)
                {
                    err_no_ = errno;
                    err_msg_ = strerror(errno);
                    break;
                }
                
                error_line_ = Parse(file, err_msg_);
                if (error_line_ != 0)
                {
                    err_no_ = -999;
                    break;
                }
                
                if ( 0 != fclose(file) )
                {
                    err_no_ = errno;
                    err_msg_ = strerror(errno);
                    break;
                }
                
                has_error = false;
                
            } while(0);

            if (has_error)
            {
                tr1::shared_ptr<EXCEPTION::IException> inner_e(
                    new EXCEPTION::FileOpenException(filename, err_no_, err_msg_));

                EXCEPTION::ThrowableException e(inner_e);
                throw e;                
            }
        }
        
        int IniParser::Parse(FILE* file, string& errMsg)
        {
            char line[MAX_LINE_IN_BYTES_] = {0};
            char section[MAX_SECTION_IN_BYTES_] = {0};
            char prev_name[MAX_SECTION_IN_BYTES_] = {0};
            
            char* start = 0;
            char* end   = 0;
            char* name  = 0;
            char* value = 0;
            
            int lineno  = 0;
            int error   = 0;
            
            while (fgets(line, MAX_LINE_IN_BYTES_, file) != NULL) 
            {
                lineno++;

                start = line;
                if (    lineno == 1 
                     && (unsigned char)start[0] == 0xEF 
                     && (unsigned char)start[1] == 0xBB 
                     && (unsigned char)start[2] == 0xBF ) 
                {
                    start += 3;
                }
                
                // remove the right space
                start = LeftSkip(RightStrip(start));

                if (*start == ';' || *start == '#') 
                {
                    // remove all the comments
                }
                else if (*start == '[') 
                {
                    // we have met a section
                    end = FindCharOrComment(start + 1, ']');
                    if (*end == ']') 
                    {
                        *end = '\0';
                        Strncpy0(section, start + 1, sizeof(section));
                        *prev_name = '\0';
                    }
                    else if (!error) 
                    {
                        stringstream ss;
                        ss << "line " << lineno << " : no corresponding ']'";
                        errMsg = ss.str();
                        error = lineno;
                    }
                }
                else if (*start && *start != ';') 
                {
                    end = FindCharOrComment(start, '=');
                    if (*end != '=') 
                    {
                        end = FindCharOrComment(start, ':');
                    }
                    if (*end == '=' || *end == ':') 
                    {
                        *end = '\0';
                        name = RightStrip(start);
                        value = LeftSkip(end + 1);
                        end = FindCharOrComment(value, '\0');
                        if (*end == ';')
                        {
                            *end = '\0';
                        }
                        RightStrip(value);
        
                        Strncpy0(prev_name, name, sizeof(prev_name));
                        
                        Set(section, name, value);
                    }
                    else if (!error) 
                    {
                        error = lineno;
                        stringstream ss;
                        ss << "line " << lineno << ": no '=' or ':' has been found in pattern like \"name[=:]\"";
                        errMsg = ss.str();
                    }
                }
        
                if (error)
                {
                    break;    
                }
            }
            
            return error;
        }

        int IniParser::ParseError(string& errMsg)
        {
            errMsg = err_msg_;
            return error_line_;
        }
        
        string IniParser::FormReason(string key, string type)
        {
            stringstream ss;
            ss << "in file " << file_name_ << " : no key [" << key << "] to be found, or the key has been found but the item can not be transferred to type [" << type << "]";
            return ss.str();
        }

        string IniParser::GetString(string section, string name) throw (ThrowableException)
        {
            string key = MakeKey(section, name);
            
            if (values_.count(key) == 0)
            {
                string key = section + string("::") + name;
                tr1::shared_ptr<EXCEPTION::IException> inner_e(
                    new EXCEPTION::ObjectNotExistingInContainerException(key, FormReason(key, "string")));

                EXCEPTION::ThrowableException e(inner_e);
                throw e; 
            }
            
            return values_[key];
        }

        long IniParser::GetInteger(string section, string name) throw (ThrowableException)
        {
            string valstr = GetString(section, name);
            const char* value = valstr.c_str();
            char* end;
            long n = strtol(value, &end, 0);
            if (end <= value)
            {
                string key = section + string("::") + name;
                tr1::shared_ptr<EXCEPTION::IException> inner_e(
                    new EXCEPTION::ObjectNotExistingInContainerException(key, FormReason(key, "integer")));

                EXCEPTION::ThrowableException e(inner_e);
                throw e; 
            }
            
            return n;
        }

        double IniParser::GetReal(string section, string name) throw (ThrowableException)
        {
            string valstr = GetString(section, name);
            const char* value = valstr.c_str();
            char* end;
            double n = strtod(value, &end);
            
            if (end <= value)
            {
                string key = section + string("::") + name;
                tr1::shared_ptr<EXCEPTION::IException> inner_e(
                    new EXCEPTION::ObjectNotExistingInContainerException(key, FormReason(key, "double")));

                EXCEPTION::ThrowableException e(inner_e);
                throw e; 
            }
            
            return n;
        }

        bool IniParser::GetBoolean(string section, string name) throw (ThrowableException)
        {
            bool rslt;
            string valstr = GetString(section, name);
            valstr = StringHelper::ToLower(valstr);
            
            if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
            {
                rslt = true;
            }
            else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
            {
                rslt = false;
            }   
            else
            {
                string key = section + string("::") + name;
                tr1::shared_ptr<EXCEPTION::IException> inner_e(
                    new EXCEPTION::ObjectNotExistingInContainerException(key, FormReason(key, "bool")));

                EXCEPTION::ThrowableException e(inner_e);
                throw e; 
            }
            
            return rslt;
        }

        string IniParser::MakeKey(string section, string name)
        {
            string key = section + string("=") + name;
            key = StringHelper::ToLower(key);
            return key;
        }

        void IniParser::Set(const char* section, const char* name, const char* value)
        {
            string key = MakeKey(section, name);
            values_[key] = value;
        }
        
        char* IniParser::RightStrip(char* s)
        {
            char* p = s + strlen(s);
            while (p > s && isspace((unsigned char)(*--p)))
                *p = '\0';
            return s;
        }

        char* IniParser::LeftSkip(const char* s)
        {
            while (*s && isspace((unsigned char)(*s)))
                s++;
            return (char*)s;
        }

        char* IniParser::FindCharOrComment(const char* s, char c)
        {
            int was_whitespace = 0;
            while (*s && *s != c && !(was_whitespace && *s == ';')) 
			{
                was_whitespace = isspace((unsigned char)(*s));
                s++;
            }
            return (char*)s;
        }

        char* IniParser::Strncpy0(char* dest, const char* src, size_t size)
        {
            strncpy(dest, src, size);
            dest[size - 1] = '\0';
            return dest;
        }
    }
}
