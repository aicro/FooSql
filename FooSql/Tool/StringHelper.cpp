#include "tool/StringHelper.h"

namespace COMMON
{
    namespace TOOL
    {
        void StringHelper::Split(vector<string> &tokens, const string &text, char delimiter)
        {
            int start = 0, end = 0;
            while ((end = text.find(delimiter, start)) != string::npos) 
            {
                tokens.push_back(text.substr(start, end - start));
                start = end + 1;
            }
            tokens.push_back(text.substr(start));
        }
        
        string StringHelper::TrimRight(string& text, string toTrim)
        {
            size_t endpos = text.find_last_not_of(toTrim);
            if( string::npos != endpos )
            {
                text = text.substr( 0, endpos + 1 );
            }
            else
            {
                // the whole text is composed of characters to be trimed
                text = "";
            }
            
            return text;
        }
        
        string StringHelper::TrimRight(const char* text, const char* toTrim)
        {
            string n_text(text);
            string n_toTrim(toTrim);
            
            return TrimRight(n_text, n_toTrim);
        }
        
        string StringHelper::TrimLeft(string& text, string toTrim)
        {
            size_t startpos = text.find_first_not_of(toTrim);
            if( string::npos != startpos )
            {
                text = text.substr( startpos );
            }
            else
            {
                // the whole text is composed of characters to be trimed
                text = "";
            }
            
            return text;
        }
        
        string StringHelper::TrimLeft(const char* text, const char* toTrim)
        {
            string n_text(text);
            string n_toTrim(toTrim);
            
            return TrimLeft(n_text, n_toTrim);
        }
        
        string StringHelper::Trim(string& text, string toTrim)
        {
            if ("" == text)
            {
                return "";
            }
            
            text = StringHelper::TrimLeft(text, toTrim);
            text = StringHelper::TrimRight(text, toTrim);
            
            return text;
        }
        
        string StringHelper::Trim(const char* text, const char* toTrim)
        {
            string n_text(text);
            string n_toTrim(toTrim);
            
            return Trim(n_text, n_toTrim);
        }
        
        string StringHelper::ToLower(string& text)
        {
            for (int i = 0; i < text.length(); i++)
            {
                text[i] = ::tolower(text[i]);
            }
            return text;
        }
            
        string StringHelper::ToUper(string& text)
        {
            for (int i = 0; i < text.length(); i++)
            {
                text[i] = ::toupper(text[i]);
            }
            return text;
        }
    }
}