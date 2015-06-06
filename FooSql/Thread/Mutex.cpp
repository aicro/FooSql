#include "thread/Mutex.h"

namespace COMMON
{
    namespace THREAD
    {
        Mutex::Mutex()
        {
            pthread_mutex_init(&mutex, NULL);
        }

        void Mutex::Lock()
        {
            pthread_mutex_lock(&mutex);
        }

        void Mutex::Unlock()
        {
            pthread_mutex_unlock(&mutex);
        }

        Mutex::~Mutex()
        {
            pthread_mutex_destroy(&mutex);
        }

        pthread_mutex_t* Mutex::GetMutex()
        {
            return &mutex;
        }
    }
}

