/// @file ThreadPool.h
/// @brief A simple thread pool has been defined in this file.

/// @author Aicro Ai
/// @date 2015/2/25

#ifndef THREAD_THREAD_POOL_H_
#define THREAD_THREAD_POOL_H_

#include <vector>
#include <tr1/functional>
#include <tr1/memory>

#include "thread/BlockingQueue.h"

using namespace std;

namespace COMMON 
{
    namespace THREAD 
    {
        class Thread;
    
        /// @brief Wrapper Tasks to be transferred to the thread pool.
		/// The real work should be put in the inner variable of this class. 
        class ThreadPoolTask
        {
        public:
			/// @brief explicit constructor.
			/// @param task the real input parameter to the working thread.
            explicit ThreadPoolTask(void* task = 0) 
                : task_(task), to_stop_(false) {}

            /// @brief the real work.
            void* task_; 

        private:
            friend class ThreadPool;
            bool to_stop_; // signal to stop
            bool clear_all_undo_tasks_; // whether we should clear those undone tasks
            bool clear_all_untaken_rslts_; // whether we should clear the results not gained from the caller
        };

        /// @brief A class represents a thread pool
        class ThreadPool 
        {
        public:
            /// @brief Constructor. To create a ThreadPool object.
            /// @param[in] threadNum Threads number in the thread pool, the default is 8
            /// @param[in] maxQueueLength The capacity of the blocking queue in the thread pool. The default is 1024.
            ThreadPool(int threadNum = 8, int maxQueueLength = 1024);

            ~ThreadPool();

            /// @brief Adjust the number of the inner thread. The function is only valid before any the thread pool is running.
            /// @param[in] threadNum Threads number in the thread pool, the default is 8
            void SetThreadsNum(int threadNum);

			/// @brief The callback funtion for the thread pool. We can only specify the callback function before any the thread pool is running.
			/// @param[in] aFunc A pointer to the callback function.
            /// @return Success or not.
            bool SetCallbackForThreads(void* (*aFunc)(void*));

            /// @brief Inform the thread pool to start
            /// @return Threads IDs in the thread pool.
            vector<int> Start();
            
			/// @brief Inform the thread pool to be stopped after all the existing tasks to be finished. 
			/// The method won't interrupt the running thread, but to insert a signal hints stop 
			/// to the end of the blocking queue, which indicates the thread pool to finish all 
			///	tasks before stop.
			///	The two parameters hint the behaviour towards the existing tasks waiting to be run 
			/// and any results not taken by the caller after the stop.
			/// This function will return immediately after the signal has been sent, except in the case 
			/// that the blocking queue is full.
            /// @param[in] clearAllUndoTasks true means to clear all undo tasks when stops. The default is true.
            /// @param[in] clearUntakenRslt true means to clear all untaken result when stops. The default is true.
			void Stop(bool clearAllUndoTasks = true, bool clearUntakenRslt = true);
            
			/// @brief Inform the thread pool to be stopped immediately, not regarding any tasks
			/// waiting in the blocking queue.
			/// The method won't interrupt the running thread, but to insert a signal hints stop 
			/// to the front of the blocking queue, which indicates the thread pool to stop after 
			/// finishing the current work.
			///	The two parameters hint the behaviour towards the existing tasks waiting to be run 
			/// and any results not taken by the caller after the stop.
			/// This function will return immediately after the signal has been sent, except in the case 
			/// that the blocking queue is full.
            /// @param[in] clearAllUndoTasks true means to clear all undo tasks when stops. The default is true.
            /// @param[in] clearUntakenRslt true means to clear all untaken result when stops. The default is true.
            void FastStop(bool clearAllUndoTasks = true, bool clearUntakenRslt = true);

            /// @brief Push a task to the thread pool.
			/// @param task task to be done
            /// @retrun true if success, false means post failure, which may be caused by a non-working thread pool
			bool PushTask(ThreadPoolTask task);
            
            /// @brief Push multiple tasks to the thread pool in their corresponding sequence.
			/// @param tasks tasks to be done
            /// @retrun true if success, false means post failure, which may be caused by a non-working thread pool
            bool PushTask(vector<ThreadPoolTask> tasks);
            
            /// @brief 向线程池中传递任务，并让其中的线程优先处理
            /// @param[in] task 需要向线程池中投递的任务
            /// @retrun 返回false表示投递任务失败，即线程池已经停止工作，不再接受新的任务
			
			/// @brief Push a task with top priority to the thread pool
			/// @param task task to be done
            /// @retrun true if success, false means post failure, which may be caused by a non-working thread pool
            bool PushTaskFront(ThreadPoolTask task);

			/// @brief Push multiple tasks to the thread pool in their corresponding sequences
			/// @param tasks tasks to be done
            /// @retrun true if success, false means post failure, which may be caused by a non-working thread pool            
            bool PushTaskFront(vector<ThreadPoolTask> tasks);

            /// @brief 从线程池中获取任务结果.
            /// 注意，先后结果顺序一般并不等同于输入的任务顺序。所以
            /// 如果你需要获取结果，那么需要在线程回调函数中自行添加对应关系。
            /// 如果线程池因为任务没有处理完毕而导致输出队列中没有任务结果，则调用该函数的线程将会被阻塞。
			/// 用户需要自行确保在线程池中仍然有任务的返回值，即push和pop要相互匹配，否则会遇到直到有下一次
			/// 任务才会让调用线程醒来的尴尬。
            /// @retrun 需要从线程池中返回的任务结果

			/// @brief Get a working result from the thread pool. Note that the order of the results may different 
			/// from their incoming sequences. Therefore, it is the coder's responsibility to maintain the sequence 
			/// by thread synchronization.
            /// @retrun A result comming from the inner thread. The output sequence is determined by the finish time
			/// of different tasks.
            void* PopResult();
            
			/// @brief Clear all unrun tasks, including the task for stop.
            void CancelAllUnrunTasks();
            
			/// @brief Clear any ready but untakeng results
            void ClearReadyRslts();
            
            /// @brief Block the current thread until all the inner thread return.
			/// This method are always called after Stop*()
			void WaitAllThreadsEnd();
            
        private:
            ThreadPool(const ThreadPool&);
            ThreadPool& operator=(const ThreadPool&);
           
            ThreadPoolTask PrepareStop(bool clearAllUndoTasks, bool clearUntakenRslt);
            vector<ThreadPoolTask> PrepareStopTasks(bool clearAllUndoTasks, bool clearUntakenRslt);

            void InitThreads(int threadNum);
            
        private:
            vector<tr1::shared_ptr<Thread> > threads_;     ///< threads
            tr1::function<void* (void*)> tFunc_; ///< callback functions

            BlockingQueue<ThreadPoolTask> input_queue_;   ///< input blocking queue
            BlockingQueue<ThreadPoolTask> output_queue_;  ///< output blocking queue

            bool running_;               ///< True true if running
            
            unsigned int stoped_threads_num_; ///< number for stopped threads.

            Mutex stop_mutex_;          
            Condition stop_condition_;  
        
        private:
            static void * wrapper_function(void * aArg);
        };
    }
}
#endif
