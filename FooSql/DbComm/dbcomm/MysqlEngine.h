/// @file MysqlEngine.h
/// @brief The file implements all the methods for MYSQL defined by @c DbEngine.

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_MYSQLENGINE_H_
#define COMMON_DBCOMM_MYSQLENGINE_H_

#ifdef MYSQL_ENV_AVAILABLE

/* To Avoid a bug caused by Mysql, 
   http://stackoverflow.com/questions/4597281/mysql-headers-conflict-with-stl-algorithm-in-c */
#include <string>
#include <algorithm>

// DB OPERATION
#include "DbEngine.h"

// THREAD 
#include "thread/Mutex.h"

#include "my_global.h"
#include "mysql.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief The class implements all the operations for 
        class MysqlEngine : public DbEngine
        {
        private:
            /// @brief The handle wrapper for each MYSQL connection
            class MysqlRealHandle : public RealHandle
            {
            public:
                MysqlRealHandle()
                {
                    res = 0;
                    mysql_init(&mysql);
                }
                
                ~MysqlRealHandle()
                {
                    // Make sure to free memory
                    mysql_close(&mysql);
                }
                
            public:
                /// @brief MYSQL handle
                MYSQL       mysql;
                    
                /// @brief MYSQL result set
                MYSQL_RES*  res; 
            };
            /// @brief A set of handles for different connections
            map<DbLocation, tr1::shared_ptr<MysqlRealHandle> > handles_;
            
        public:
            /// @brief Constructor
            /// @param locations The locations for MYSQL to connect
            /// @param task The pointer to a @c IDbTasks, from which this instance was generated
            MysqlEngine(vector<DbLocation>& locations, tr1::shared_ptr<IDbTasks> task);
            
            virtual ~MysqlEngine();
            
            virtual bool Connect(void* handle, DbLocation* location, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();
            
            virtual bool Disconnect(void* handle, DbLocation* location, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual bool            Query(void* handle, DbLocation* location, const char* statement, size_t length, map<string, int>* colIndexMap, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual char**          Fetch(void* handle,  DbLocation* location, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual bool            CloseOpenRslt(void* handle,  DbLocation* location, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual unsigned long*  GetColumnsActureLength(void* handle,  DbLocation* location, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual bool            Commit(void* handle,  DbLocation* location, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual long long       GetAffectedRows(void* handle,  DbLocation* location, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual long long       Delete(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();
            
            virtual long long       Update(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();
            
            virtual long long       Truncate(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual long long       Insert(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual unsigned int    Execute(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();

            virtual char*   EscapeString(void* handle, DbLocation* location, const char* src, long length, tr1::shared_ptr<EXCEPTION::IException>& exception) throw ();
            
        protected:
            long long GetAffectedRows( long long &affectRows, void* handle, int sqlCode, string errorMsg );
            
        protected:
            virtual void InitThread();
                
            virtual void UninitThread();
            
            virtual tr1::shared_ptr<RealHandle> GetRealHandle(DbLocation& location);
            
        private:
            string& ExtractErrMsg( int sqlCode, void* handle, string& errorMsg );
            
            static void HandleMysqlLibrary();
        };   
        
        class MysqlLibraryHandler
        {
        public:
            MysqlLibraryHandler();
            ~MysqlLibraryHandler();
        };
    }
}
#endif

#endif
