#include <stdlib.h>
#include "tool/TypeCheck.h"

namespace COMMON
{
    namespace TOOL
    {
        bool TypeCheck::IsDouble(string s)
        {
            char* p;
            strtod(s.c_str(), &p);
            return *p == '\0';
        }
        
        bool TypeCheck::IsInteger(string s)
        {
            char* p;
            strtol(s.c_str(), &p, 10);
            return *p == '\0';
        }
    }
}
