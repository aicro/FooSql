#include "thread/Condition.h"
#include "thread/Mutex.h"

/* Usage
   
   Thread 1 checks a condition. If the condition doesn't meet, he waits on the condition variable until the condition meets. 
            Because the condition is checked first, he shouldn't care whether the condition variable was signaled:

        pthread_mutex_lock(&mutex); 
        while (!condition)
            pthread_cond_wait(&cond, &mutex); 
        pthread_mutex_unlock(&mutex);
        
   Thread 2 changes the condition and signals the change via the condition variable. He doesn't care whether threads are waiting or not:

        pthread_mutex_lock(&mutex); 
        changeCondition(); 
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex); 
        
   The bottom line is: the communication is done via some condition. A condition variable only wakes up waiting threads so they can check the condition.

   Examples for conditions:

   1.Queue is not empty, so a member can be taken from the queue
   2.A boolean flag is set,so the thread wait s until the other thread signal it's okay to continue
   3.some bits in a bitset are set, so the waiting thread can handle the corresponding events
*/

namespace COMMON
{
    namespace THREAD
    {
        Condition::Condition()
        {
            pthread_cond_init(&cond,NULL);
        }

        Condition::~Condition()
        {
            pthread_cond_destroy(&cond);
        }

        void Condition::Wait(Mutex& mutex)
        {
            pthread_cond_wait(&cond, mutex.GetMutex());
        }

        void Condition::Notify()
        {
            pthread_cond_signal(&cond);
        }

        void Condition::NotifyAll()
        {
            pthread_cond_broadcast(&cond);
        }        
    }
}