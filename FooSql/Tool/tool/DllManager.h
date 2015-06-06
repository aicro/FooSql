/// @file DllManager.h
/// @brief The file defines a class representing operations 
/// on dynamic linking library.

/// @author Aicro Ai
/// @date 2014/9/19

#ifndef COMMON_TOOL_DLL_MANAGER_H_
#define COMMON_TOOL_DLL_MANAGER_H_

#include <string>

using namespace std;

namespace COMMON
{
    namespace TOOL
    {
        class DllManager
        {
        public:
            DllManager(string dllFullPath);

            bool Open();

            void* GetFunction(string funcName);

            ~DllManager();

            string GetLastError();

            string GetLastErrorRaw();

            string GetFilePath();

        private:
            string dll_full_path_;
            void* dll_handle_;

            string error_;
        };
    }
}

#endif
