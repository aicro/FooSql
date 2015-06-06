/// @file DB2Engine.h
/// @brief The file implements methods for operations of DB2. In fact, the methods are implemented by CLI, therefore, 
/// we can use these methods by other DBMS just replace the connections strings.

/// @author Aicro Ai
/// @date 2015/01/27

#ifndef COMMON_DBCOMM_DB2ENGINE_H_
#define COMMON_DBCOMM_DB2ENGINE_H_

#ifdef DB2_ENV_AVAILABLE

// heads for CLI
extern "C"
{
    #include <sqlcli.h>
    #include <sqlcli1.h>
    #include <sqlenv.h>
    #include <sqlda.h>
    #include <sqlca.h>
}

#include <string>
#include <algorithm>

#include "dbcomm/DbEngine.h"

namespace COMMON
{ 
    namespace DBCOMM
    {
        /// @brief Methods implemented for DB2. Some trivial changes will 
		/// make this class work for other DBMS which may use CLI to operate.
        class DB2Engine : public DbEngine
        {
        private:
            /// @brief A handle wrapper for CLI
            class Db2RealHandle : public RealHandle
            {
            public:
                Db2RealHandle()
                    : RealHandle()
                {
                    res    = 0;
                    tmpCol = 0;
                    colNum = 0;
                    maxLen = 0;
                    lengths = 0;
                    
                    //创建环境句柄
                    rc = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&henv);

                    //创建连接句柄
                    rc = SQLAllocHandle(SQL_HANDLE_DBC,henv,&hdbc);
                }
                
                virtual ~Db2RealHandle()
                {
                    rc = SQLFreeHandle(SQL_HANDLE_DBC,hdbc);

                    rc = SQLFreeHandle(SQL_HANDLE_ENV,henv);
                }

            public:
                /// @brief environment handle
                SQLHENV     henv;
                
                /// @brief connection handle
                SQLHDBC     hdbc;
                
                /// @brief statement handle
                SQLHSTMT    hstmt;    
            
                /// @brief return code
                SQLRETURN   rc;
                
                /// @brief result set
                char**    res;
                
				/// @brief a buffer for SQLGetData.
                char*     tmpCol;     
                
                /// @brief how many columns are there in the result set
                int       colNum;     
                
                /// @brief the length of the widest columns in the current row
                int       maxLen;     
                
				/// @brief the defined lengths of each columns in the current row
                unsigned long*   col_max_len;
                
				/// @brief the real length of the each columns in the current row
                unsigned long*   lengths;
            };
            /// @brief handles for each connections
            map<DbLocation, tr1::shared_ptr<Db2RealHandle> > handles_;

        public:
            /// @brief Constructor
            /// @param locations locations to connect
            /// @param task the @c IDbTasks that generate this instance
            DB2Engine(vector<DbLocation>& locations, tr1::shared_ptr<IDbTasks> task);
            
            virtual ~DB2Engine();
                
            virtual bool Connect(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ();
            
            virtual bool Disconnect(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ();

            virtual bool            Query(void* handle, DbLocation* location, const char* statement, size_t length, map<string, int>* colIndexMap, tr1::shared_ptr<IException>& exception) throw ();

            virtual char**          Fetch(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ();

            virtual bool            CloseOpenRslt(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ();

            virtual unsigned long*  GetColumnsActureLength(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ();

            virtual bool            Commit(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ();

            virtual long long       GetAffectedRows(void* handle,  DbLocation* location, tr1::shared_ptr<IException>& exception) throw ();

            virtual long long       Delete(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ();
            
            virtual long long       Update(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ();
            
            virtual long long       Truncate(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ();

            virtual long long       Insert(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ();

            virtual unsigned int    Execute(void* handle,  DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ();

            virtual char*   EscapeString(void* handle, DbLocation* location, const char* src, long length, tr1::shared_ptr<IException>& exception) throw ();
      
        protected:
            long long GetAffectedRows( long long &affectRows, void* handle, int sqlCode, string errorMsg );
            
        protected:
            virtual void InitThread();
                
            virtual void UninitThread();
            
            virtual tr1::shared_ptr<DbEngine::RealHandle> GetRealHandle(DbLocation& location);
            
        private:
            // Get error msg from an statement handle
			string& ExtractErrMsgStmt( int& sqlCode, void* handle,string &errorMsg);
            
            // Get error msg from an connection handle
            string& ExtractErrMsgDbc( int& sqlCode, void* handle,string &errorMsg);
            
            // Get error msg from an environment handle 
            string& ExtractErrMsgEnv( int& sqlCode, void* handle,string &errorMsg);
            
            string FormErrMsg(int recNum, SQLCHAR* sqlstate, int nagativeSqlCode, SQLCHAR* errMsg);
        };
    }
}

#endif

#endif
