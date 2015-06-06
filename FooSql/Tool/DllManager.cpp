#include <dlfcn.h> 
#include <sstream>

#include "tool/DllManager.h"

namespace COMMON
{
    namespace TOOL
    {
        void* DllManager::GetFunction(string funcName)
        {
            void* func = dlsym(dll_handle_, funcName.c_str());
            if (func == 0)
            {
                stringstream ss;
                ss << "dlsym(\"" << funcName + "\") fails , error code is " << dlerror();
                error_ = ss.str();
            }

            return func;
        }

        DllManager::~DllManager()
        {
            if (dll_handle_)
            {
                dlclose(dll_handle_);
            }
        }

        bool DllManager::Open()
        {
            if (dll_handle_ == 0)
            {
                dll_handle_ = dlopen(dll_full_path_.c_str(), RTLD_LAZY);

                if (dll_handle_ == 0)
                {
                    stringstream ss;
                    ss << "dlopen(\"" << dll_full_path_ + "\") fails, error code is " << dlerror();
                    error_ = ss.str();
                }
            }
            
            return dll_handle_ != 0;
        }

        DllManager::DllManager(string dllFullPath)
            :dll_full_path_(dllFullPath)
        {
            dll_handle_ = 0;
        }

        string DllManager::GetLastError()
        {
            return error_;
        }

        string DllManager::GetLastErrorRaw()
        {
            return dlerror();
        }

        std::string DllManager::GetFilePath()
        {
            return dll_full_path_;
        }

    }
}