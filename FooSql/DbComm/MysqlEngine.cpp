#ifdef MYSQL_ENV_AVAILABLE

#include "dbcomm/CommDef.h"
#include "dbcomm/MysqlEngine.h"
#include "dbcomm/DbActionFilter.h"

#include "exception/IException.h"
#include "dbcomm/DbException.h"

#include "tool/StringHelper.h"

using namespace COMMON::THREAD;

namespace COMMON
{
    namespace DBCOMM
    {
        /////////////////////////////
        // MysqlEngine
        /////////////////////////////
        
        MysqlEngine::MysqlEngine(vector<DbLocation>& locations, tr1::shared_ptr<IDbTasks> task)
            : DbEngine(locations, task)
        {
        }
        
        MysqlEngine::~MysqlEngine()
        {
        }
        
        tr1::shared_ptr<DbEngine::RealHandle> MysqlEngine::GetRealHandle(DbLocation& location)
        {
            if (handles_.find(location) == handles_.end())
            {
                handles_[location].reset(new MysqlRealHandle());
            }
            
            return handles_[location];
        }
        
        bool MysqlEngine::Connect(void* handle, DbLocation* location, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            int sqlCode = 0;
            
            if (   0 == mysql_real_connect(
                        &(((MysqlRealHandle*)handle)->mysql), 
                        const_cast<char*>(location->GetIp().c_str()),
                        const_cast<char*>(location->GetUser().c_str()),
                        const_cast<char*>(location->GetPassword().c_str()),
                        const_cast<char*>(location->GetDbId().c_str()),
                        atoi(location->GetPort().c_str()),
                        0, CLIENT_FOUND_ROWS) 
                || 0 != mysql_autocommit(&(((MysqlRealHandle*)handle)->mysql), 0) )
            {
                sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
            }
                        
            string errorMsg;
            errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
        
            if (sqlCode != 0)
            {
                exception.reset(new COMMON::EXCEPTION::DB::DbConnectException(*location, errorMsg, sqlCode, 0, 0));
            }
        
            return sqlCode == 0;
        }
        
        bool MysqlEngine::Disconnect(void* handle, DbLocation* location, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            // no exception will be thrown from mysql_close()
            mysql_close(&(((MysqlRealHandle*)handle)->mysql));
            
            return true;
        }
        
        bool MysqlEngine::Query(
            void* handle, DbLocation* location, const char* statement, size_t length, map<string, int>* colIndexMap, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            int sqlCode = 0;
        
            if (0 != mysql_real_query(
                        &(((MysqlRealHandle*)handle)->mysql), 
                        statement, length) )
            {
                sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
            }
            
            if (sqlCode != 0)
            {
                string errorMsg;
                errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                exception.reset(new COMMON::EXCEPTION::DB::DbSelectExecuteException(*location, errorMsg, sqlCode, statement, length));
                return false;
            }
        
            // open the result set
            ((MysqlRealHandle*)handle)->res = mysql_use_result(&(((MysqlRealHandle*)handle)->mysql));
            if (((MysqlRealHandle*)handle)->res == 0)
            {
                sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
            }
            
            if (sqlCode != 0)
            {
                string errorMsg;
                errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                exception.reset(new COMMON::EXCEPTION::DB::DbSelectOpenException(*location, errorMsg, sqlCode, statement, length));
                return false;
            }
            
            // find out the map for column name and its index
            colIndexMap->clear();
            MYSQL_FIELD *field = 0;
            int i = 0;
            while((field = mysql_fetch_field(((MysqlRealHandle*)handle)->res)))
            {
                string name(field->name);
                TOOL::StringHelper::ToLower(name);
                TOOL::StringHelper::Trim(name, " \t");
                (*colIndexMap)[name] = i;
                i++;
            }
            
            return sqlCode == 0;
        }
        
        char** MysqlEngine::Fetch(void* handle, DbLocation* location, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            int sqlCode = 0;
        
            char ** rsltRow = 0;
            rsltRow = (char **)mysql_fetch_row(((MysqlRealHandle*)handle)->res);
        
            if (rsltRow == 0)
            {
				// error met or reach the end
                sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
                
                if (sqlCode != 0)
                {
                    // error
                    string errorMsg;
                    errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                    
                    exception.reset(new COMMON::EXCEPTION::DB::DbSelectFetchRowException(*location, errorMsg, sqlCode, 0, 0));
                }
            }
        
            return rsltRow;
        }
        
        unsigned long* MysqlEngine::GetColumnsActureLength( void* handle, DbLocation* location, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            int sqlCode = 0;
            unsigned long* rsltColumnLength = 0;
            
            rsltColumnLength = mysql_fetch_lengths(((MysqlRealHandle*)handle)->res);
            
            if (rsltColumnLength == 0)
            {
                sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
                    
                if (sqlCode != 0)
                {
                    // error
                    string errorMsg;
                    errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                    
                    exception.reset(new COMMON::EXCEPTION::DB::DbSelectGetColumnsLengthException(*location, errorMsg, sqlCode, 0, 0));
                }
            }
        
            return rsltColumnLength;
        }
        
        string& MysqlEngine::ExtractErrMsg( int sqlCode, void* handle, string& errorMsg )
        {
            errorMsg = "";
            if (sqlCode != 0)
            {
                errorMsg = mysql_error(&(((MysqlRealHandle*)handle)->mysql));
            }	
            
            return errorMsg;
        }
        
        bool MysqlEngine::Commit(void* handle, DbLocation* location, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            int sqlCode = 0;
            if (0 != mysql_commit(&(((MysqlRealHandle*)handle)->mysql)))
            {
                sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
                            
                string errorMsg;
                errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                    
                exception.reset(new COMMON::EXCEPTION::DB::DbCommitException(*location, errorMsg, sqlCode, 0, 0));
            }
            return sqlCode == 0;
        }
        
        bool MysqlEngine::CloseOpenRslt(void* handle, DbLocation* location, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            // no exception will be thrown out
            mysql_free_result(((MysqlRealHandle*)handle)->res);
            ((MysqlRealHandle*)handle)->res = 0;
        
            return true;
        }
        
        long long MysqlEngine::GetAffectedRows(void* handle, DbLocation* location, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            // no exception will be thrown out
            return mysql_affected_rows(&(((MysqlRealHandle*)handle)->mysql));
        }
        
        long long MysqlEngine::Delete(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            long long affectRows = 0;
        
            int rslt = mysql_real_query(
                &(((MysqlRealHandle*)handle)->mysql), statement, length);
                
            if (rslt != 0)
            {
                int sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
                string errorMsg;
                errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                exception.reset(new COMMON::EXCEPTION::DB::DbDeleteException(*location, errorMsg, sqlCode, statement, length));
                return 0;
            }
        
            return GetAffectedRows(handle, location, exception);
        }
        
        long long MysqlEngine::Update(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            long long affectRows = 0;
        
            int rslt = mysql_real_query(
                &(((MysqlRealHandle*)handle)->mysql), statement, length);
            if (rslt != 0)
            {
                int sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
                string errorMsg;
                errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                exception.reset(new COMMON::EXCEPTION::DB::DbUpdateException(*location, errorMsg, sqlCode, statement, length));
                return 0;
            }
        
            return GetAffectedRows(handle, location, exception);
        }
        
        long long MysqlEngine::Truncate(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            long long affectRows = 0;
        
            int rslt = mysql_real_query(
                &(((MysqlRealHandle*)handle)->mysql), statement, length);
            if (rslt != 0)
            {
                int sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
                string errorMsg;
                errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                exception.reset(new COMMON::EXCEPTION::DB::DbTruncateException(*location, errorMsg, sqlCode, statement, length));
                return 0;
            }
        
            return GetAffectedRows(handle, location, exception);
        }
        
        long long MysqlEngine::Insert(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            int rslt = mysql_real_query(
                &(((MysqlRealHandle*)handle)->mysql), statement, length);
            
            if (rslt != 0)
            {
                int sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
                string errorMsg;
                errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                
                if (sqlCode == 1062)
                {
                    exception.reset(new COMMON::EXCEPTION::DB::DbInsertDuplicateKeyException(*location, errorMsg, sqlCode, statement, length));
                    return 0;
                }
                else
                {
                    exception.reset(new COMMON::EXCEPTION::DB::DbInsertException(*location, errorMsg, sqlCode, statement, length));
                    return 0;
                }
            }
        
            return GetAffectedRows(handle, location, exception);
        }
        
        unsigned int MysqlEngine::Execute(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {    
            int sqlCode = 0;
            if( 0 != mysql_real_query(&(((MysqlRealHandle*)handle)->mysql), statement, length) )
            {
                string errorMsg;
                sqlCode = mysql_errno(&(((MysqlRealHandle*)handle)->mysql));
                errorMsg = ExtractErrMsg(sqlCode, handle, errorMsg);
                
                exception.reset(new COMMON::EXCEPTION::DB::DbCommonExecuteException(*location, errorMsg, sqlCode, statement, length));
            }
            
            return sqlCode;
        }
        
        char* MysqlEngine::EscapeString(
            void* handle,  DbLocation* location,  const char* src, long length, tr1::shared_ptr<COMMON::EXCEPTION::IException>& exception) throw ()
        {
            // no exception will be thrown out
            unsigned long real_length = 
                mysql_real_escape_string(
                    &(((MysqlRealHandle*)handle)->mysql), 
                    ((MysqlRealHandle*)handle)->inner_buf_, 
                    src, 
                    length);
        
            // a string ending with '\0' should be generated after the above call
            return ((MysqlRealHandle*)handle)->inner_buf_;
        }
        
        void MysqlEngine::InitThread()
        {
            // prepare TLS
            mysql_thread_init();
        }
            
        void MysqlEngine::UninitThread()
        {
            // destroy TLS
            mysql_thread_end();
        }        
        
        /////////////////////////////
        // MysqlLibraryHandler
        /////////////////////////////
        MysqlLibraryHandler::MysqlLibraryHandler()
        {
            mysql_library_init(0, 0, 0);
        }
        
        MysqlLibraryHandler::~MysqlLibraryHandler()
        {
			// TODO: BUG. skip calling the following function will cause some memory leak, 
			// but I am not sure when to call it. An improper call will cause mysql complain,
			// and then some seconds delay will happen.
            
            // mysql_library_end();
        }
        
        
        MysqlLibraryHandler g_mysql_library_handler;
    }
}
#else
#warning "MYSQL ENV NOT AVAILABLE... NOT COMPILED..."

#endif
