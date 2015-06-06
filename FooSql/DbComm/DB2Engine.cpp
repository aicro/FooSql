#ifdef DB2_ENV_AVAILABLE

#include "dbcomm/CommDef.h"
#include "dbcomm/DB2Engine.h"
#include "dbcomm/DbActionFilter.h"
#include "dbcomm/DbException.h"

#include "tool/StringHelper.h"

using namespace COMMON::EXCEPTION::DB;

namespace COMMON
{
    namespace DBCOMM
    {
        DB2Engine::DB2Engine(vector<DbLocation>& locations, tr1::shared_ptr<IDbTasks> task)
            : DbEngine(locations, task)
        {    
        }
        
        DB2Engine::~DB2Engine()
        {
        }
        
        tr1::shared_ptr<DbEngine::RealHandle> DB2Engine::GetRealHandle(DbLocation& location)
        {
            if (handles_.find(location) == handles_.end())
            {
                handles_[location].reset(new Db2RealHandle());
            }
            
            return handles_[location];
        }
        
        bool DB2Engine::Connect(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()
        {
            int sqlCode = 0;
            SQLRETURN    rc=0;
            string errorMsg="";
            
            stringstream connection;
            if (location->GetIp() != "" && location->GetPort() != "")
            {
                connection  << "DRIVER={IBM DB2 ODBC DRIVER};"
                            << "DATABASE=" << location->GetDbId() << ";"
                            << "HOSTNAME=" << location->GetIp() << ";"
                            << "PORT="     << location->GetPort() << ";"
                            << "PROTOCOL=TCPIP;"
                            << "UID=" << location->GetUser() << ";"
                            << "PWD=" << location->GetPassword() << ";";    
            }
            string conn_str = connection.str();

            do
            {
                SQLCHAR out_conn_string[SQL_MAX_OPTION_STRING_LENGTH] = {0}; 
                SQLSMALLINT out_conn_string_len = 0;
                
                // set connect attributes
                rc = SQLSetConnectAttr(
                            ((Db2RealHandle*)handle)->hdbc,
                            SQL_ATTR_AUTOCOMMIT,
                            (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
                            SQL_NTS);
                if (SQL_SUCCESS != rc )
                {
                    break;
                }
                
                // connect
                if (conn_str != "")
                {
                    // we have ip and port information
                    rc = SQLDriverConnect(
                            ((Db2RealHandle*)handle)->hdbc,
                            0, 
                            (SQLCHAR *)(const_cast<char*>(conn_str.c_str())),
                            conn_str.length(),
                            out_conn_string,
                            SQL_MAX_OPTION_STRING_LENGTH,
                            &out_conn_string_len,
                            SQL_DRIVER_NOPROMPT );
                }
                else
                {
					// take advantage of under DBMS driver
                    rc = SQLConnect(((Db2RealHandle*)handle)->hdbc,
                                    (SQLCHAR *)location->GetDbId().c_str(),
                                    location->GetDbId().length(),
                                    (SQLCHAR *)location->GetUser().c_str(),
                                    location->GetUser().length(),
                                    (SQLCHAR *)location->GetPassword().c_str(),
                                    location->GetPassword().length());
                }
                
                if (SQL_SUCCESS != rc )
                {
                    break;
                }
                
                // alloc handle after successful connection
                rc = SQLAllocHandle(SQL_HANDLE_STMT, ((Db2RealHandle*)handle)->hdbc, &(((Db2RealHandle*)handle)->hstmt) );
                if (rc != SQL_SUCCESS )
                {
                    break;
                }
                
            } while (0);
            
            if (rc != SQL_SUCCESS)
            {
                sqlCode = rc;
                errorMsg = ExtractErrMsgDbc(sqlCode, handle, errorMsg);
        
                exception.reset(new DbConnectException(*location, errorMsg, sqlCode, 0, 0));
            }
        
            return sqlCode == 0;
        }
        
        bool DB2Engine::Disconnect(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()
        {
            int sqlCode = 0;
            SQLRETURN rc = 0;
            string errorMsg = "";
            
			// free statement handle, while the connection handle will be freed in deconstructor
            rc = SQLFreeHandle(SQL_HANDLE_STMT, ((Db2RealHandle*)handle)->hstmt);
            if ( SQL_SUCCESS != rc )
            {
                sqlCode=rc;
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg);
        
                exception.reset(new DbDisconnectException(*location, errorMsg, sqlCode, 0, 0));
            }
        
            rc = SQLDisconnect(((Db2RealHandle*)handle)->hdbc);
            if ( SQL_SUCCESS != rc )
            {
                sqlCode=rc;
                errorMsg = ExtractErrMsgDbc(sqlCode, handle,errorMsg);
        
                exception.reset(new DbDisconnectException(*location, errorMsg, sqlCode, 0, 0));
            }
            
            return rc == SQL_SUCCESS;
        }
        
        bool DB2Engine::Query(
            void* handle, DbLocation* location, const char* statement, size_t length, map<string, int>* colIndexMap, tr1::shared_ptr<IException>& exception) throw ()
        {
            int sqlCode = 0;
            SQLRETURN rc = 0;
            colIndexMap->clear();
            
            // execute the sql
            rc = SQLExecDirect(((Db2RealHandle*)handle)->hstmt, (SQLCHAR*)statement, length);
            if (SQL_SUCCESS != rc)
            {
                int sqlCode  = rc;
                string       errorMsg = "";
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg);
                        
                exception.reset(new DbSelectExecuteException(*location, errorMsg, sqlCode, statement, length));
                return false;
            }
        
			// get the number of columns queried
            SQLSMALLINT cols = 0;
            rc = SQLNumResultCols(((Db2RealHandle*)handle)->hstmt,&cols);
            if ( SQL_SUCCESS != rc )
            {
                int sqlCode  = rc;
                string       errorMsg = "";
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg);
        
                exception.reset(new DbSelectFetchRowException(*location, errorMsg, sqlCode, statement, length));
                return false;
            }
            
			// alloc memory if necessary
            ((Db2RealHandle*)handle)->res     = new char* [cols];
            ((Db2RealHandle*)handle)->lengths = new unsigned long [cols];
            ((Db2RealHandle*)handle)->col_max_len = new unsigned long [cols];
            ((Db2RealHandle*)handle)->colNum  = cols;
            
			// check max length for each column
            SQLLEN maxLen = 0;
            for (int i = 0; i < cols; i++ )
            {
                // get column max length
                SQLLEN  colLen = 0;
                rc = SQLColAttribute(((Db2RealHandle*)handle)->hstmt,
                                    (SQLSMALLINT)(i + 1),
                                    SQL_DESC_DISPLAY_SIZE,
                                    0,
                                    0,
                                    0,
                                    &colLen
                                    );
                if ( SQL_SUCCESS != rc )
                {
                    int sqlCode  = rc;
                    string       errorMsg = "";
                    errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg );
        
                    exception.reset(new DbSelectFetchRowException(*location, errorMsg, sqlCode, statement, length));
                    return false;
                }
                
                (((Db2RealHandle*)handle)->res)[i] = new char [colLen + 1];
                (((Db2RealHandle*)handle)->col_max_len)[i] = colLen + 1;
                
                maxLen = ( (colLen > maxLen) ? colLen : maxLen );
                
                // get column name and position map
                char name[SQL_MAX_ID_LENGTH] = {0};
                SQLSMALLINT name_real_len = 0;
                rc = SQLColAttribute(((Db2RealHandle*)handle)->hstmt,
                                    (SQLSMALLINT)(i + 1),
                                    SQL_DESC_NAME,
                                    name,
                                    SQL_MAX_ID_LENGTH,
                                    &name_real_len,
                                    0);
                if ( SQL_SUCCESS != rc )
                {
                    int sqlCode  = rc;
                    string       errorMsg = "";
                    errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg );
                
                    exception.reset(new DbSelectFetchRowException(*location, errorMsg, sqlCode, statement, length));
                    return false;
                }
                
                string tmp_name(name);
                TOOL::StringHelper::ToLower(tmp_name);
                TOOL::StringHelper::Trim(tmp_name, " \t");
                (*colIndexMap)[tmp_name] = i;
            }
            ((Db2RealHandle*)handle)->maxLen = maxLen + 1;
            
			// generate the second dimension of the result, that is the max length of the column defined when the table was build
            ((Db2RealHandle*)handle)->tmpCol = new char [((Db2RealHandle*)handle)->maxLen];
        
            return true;
        }
        
        char** DB2Engine::Fetch(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()
        {
            SQLRETURN rc = 0;
            
            // fetch
            rc = SQLFetch( ((Db2RealHandle*)handle)->hstmt );
            if ( SQL_NO_DATA_FOUND == rc )
            {
                // no extra data
                CloseOpenRslt( handle, location, exception);
                ((Db2RealHandle*)handle)->res = 0;
            
                return (((Db2RealHandle*)handle)->res);
            }
            if ( SQL_SUCCESS != rc )
            {
                int sqlCode  = rc;
                string       errorMsg = "";
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg);
                exception.reset(new DbSelectFetchRowException(*location, errorMsg, sqlCode, 0, 0));
                
                return 0;
            }
            
            // get the real length of each column
            for (int i = 1; i <= ( ((Db2RealHandle*)handle)->colNum ); i++)
            {
                memset( ((Db2RealHandle*)handle)->tmpCol,'\0',((Db2RealHandle*)handle)->maxLen );
                SQLLEN real_len;
                rc = SQLGetData( ((Db2RealHandle*)handle)->hstmt,
                                i,
                                SQL_C_CHAR,
                                ((Db2RealHandle*)handle)->tmpCol,
                                ((Db2RealHandle*)handle)->maxLen,
                                &real_len);
                if ( SQL_SUCCESS != rc )
                {
                    int sqlCode  = rc;
                    string       errorMsg = "";
                    errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg);
                
                    exception.reset(new DbSelectFetchRowException(*location, errorMsg, sqlCode, 0, 0));
                    
                    return 0;
                }

				// the odbc function will return -1 when a null column is met
                (((Db2RealHandle*)handle)->lengths)[i-1] = (unsigned long)( (real_len < 0) ? 0 : real_len );
        
                // assign the data
                memset( (((Db2RealHandle*)handle)->res)[i-1],
                        '\0',
                        (((Db2RealHandle*)handle)->col_max_len)[i-1] );
                if (real_len > 0)
                {
                    memcpy( (((Db2RealHandle*)handle)->res)[i-1],
                        ((Db2RealHandle*)handle)->tmpCol,
                        real_len );
                }  
            }
            
            return ( ((Db2RealHandle*)handle)->res );
        }
        
        unsigned long* DB2Engine::GetColumnsActureLength( void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()
        {
            return ( ((Db2RealHandle*)handle)->lengths );
        }
        
        string& DB2Engine::ExtractErrMsgStmt( int& sqlCode, void* handle, string &errorMsg)
        {
            stringstream ss;
            errorMsg = "";
        
            SQLCHAR     buffer[SQL_MAX_MESSAGE_LENGTH];
            SQLCHAR     sqlstate[SQL_SQLSTATE_SIZE + 1];
            SQLINTEGER  nagative_sql_code;
            SQLSMALLINT length;
        
            if ( sqlCode != 0 )
            {
                int rec_num = 1;
                while (SQLGetDiagRec(
                        SQL_HANDLE_STMT,
                        ((Db2RealHandle*)handle)->hstmt,
                        rec_num,
                        sqlstate,
                        &nagative_sql_code,
                        buffer,
                        SQL_MAX_MESSAGE_LENGTH,
                        &length ) == SQL_SUCCESS )
                {
                    errorMsg = FormErrMsg(rec_num, sqlstate, nagative_sql_code, buffer);
                    rec_num++;
                }
                
                sqlCode = nagative_sql_code;
            }
        
            return errorMsg;
        }
        
        string& DB2Engine::ExtractErrMsgDbc( int& sqlCode, void* handle, string &errorMsg)
        {
            stringstream ss;
            errorMsg = "";
        
            SQLCHAR     buffer[SQL_MAX_MESSAGE_LENGTH];
            SQLCHAR     sqlstate[SQL_SQLSTATE_SIZE + 1];
            SQLINTEGER  nagative_sql_code;
            SQLSMALLINT length;
        
            if ( sqlCode != 0 )
            {
                int rec_num = 1;
                while (SQLGetDiagRec(
                        SQL_HANDLE_DBC,
                        ((Db2RealHandle*)handle)->hdbc,
                        rec_num,
                        sqlstate,
                        &nagative_sql_code,
                        buffer,
                        SQL_MAX_MESSAGE_LENGTH,
                        &length ) == SQL_SUCCESS )
                {
                    errorMsg = FormErrMsg(rec_num, sqlstate, nagative_sql_code, buffer);
                    rec_num++;
                }
                
                sqlCode = nagative_sql_code;
            }
        
            return errorMsg;
        }
        
        string& DB2Engine::ExtractErrMsgEnv( int& sqlCode, void* handle, string &errorMsg)
        {
            stringstream ss;
            errorMsg = "";
        
            SQLCHAR     buffer[SQL_MAX_MESSAGE_LENGTH];
            SQLCHAR     sqlstate[SQL_SQLSTATE_SIZE + 1];
            SQLINTEGER  nagative_sql_code;
            SQLSMALLINT length;
        
            if ( sqlCode != 0 )
            {
                int rec_num = 1;
                while (SQLGetDiagRec(
                        SQL_HANDLE_ENV,
                        ((Db2RealHandle*)handle)->henv,
                        rec_num,
                        sqlstate,
                        &nagative_sql_code,
                        buffer,
                        SQL_MAX_MESSAGE_LENGTH,
                        &length ) == SQL_SUCCESS )
                {
                    errorMsg = FormErrMsg(rec_num, sqlstate, nagative_sql_code, buffer);
                    rec_num++;
                }
                
                sqlCode = nagative_sql_code;
            }
        
            return errorMsg;
        }
        
        string DB2Engine::FormErrMsg(int recNum, SQLCHAR* sqlstate, int nagativeSqlCode, SQLCHAR* errMsg)
        {
            stringstream ss;
            ss << "[" << recNum << "] SQLSTATE:[" << (char*)sqlstate << "] Native Error Code:[" << nagativeSqlCode << "] ErrMsg:" << (char*)errMsg;
            
            return ss.str();
        }
        
        bool DB2Engine::Commit(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()
        {
            int sqlCode = 0;
            SQLRETURN   rc = SQLEndTran(SQL_HANDLE_DBC,((Db2RealHandle*)handle)->hdbc,SQL_COMMIT);
            sqlCode = rc;
            
            if ( SQL_SUCCESS != rc )
            {
                string errorMsg = "";
                errorMsg = ExtractErrMsgDbc(sqlCode, handle, errorMsg);
                
                exception.reset(new DbCommitException(*location, errorMsg, sqlCode, 0, 0));
            }
        
            return sqlCode == 0;
        }
        
        bool DB2Engine::CloseOpenRslt(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()
        {
            if ( ((Db2RealHandle*)handle)->tmpCol != 0 )
            {
                delete [] ((Db2RealHandle*)handle)->tmpCol;
                ((Db2RealHandle*)handle)->tmpCol = 0;
            }
            
            if ( ((Db2RealHandle*)handle)->res != 0 )
            {
                for( int i = 0; i < ((Db2RealHandle*)handle)->colNum; i++ )
                {
                    delete [] (((Db2RealHandle*)handle)->res)[i];
                }
                delete [] ((Db2RealHandle*)handle)->res;
                ((Db2RealHandle*)handle)->res = 0;
            }
            
            if ( ((Db2RealHandle*)handle)->lengths != 0 )
            {
                delete [] ((Db2RealHandle*)handle)->lengths;
                ((Db2RealHandle*)handle)->lengths = 0;
            }
            
            if ( ((Db2RealHandle*)handle)->col_max_len != 0)
            {
                delete [] ((Db2RealHandle*)handle)->col_max_len;
                ((Db2RealHandle*)handle)->col_max_len = 0;
            }
        
            return true;
        }
        
        long long DB2Engine::GetAffectedRows(void* handle, DbLocation* location, tr1::shared_ptr<IException>& exception) throw ()
        {
            SQLLEN     row_num = 0;
            SQLRETURN rc;
            
            rc = SQLRowCount(((Db2RealHandle*)handle)->hstmt,&row_num);
            if (SQL_SUCCESS != rc)
            {
                row_num=0;
            }
            
            return row_num;
        }
        
        long long DB2Engine::Delete(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()
        {
            long long affectRows = 0;
        
            SQLRETURN rc;
            rc = SQLExecDirect(((Db2RealHandle*)handle)->hstmt, (SQLCHAR*)statement, length);
            
            if ( SQL_SUCCESS != rc && SQL_NO_DATA != rc )
            {
                int sqlCode  = rc;
                string errorMsg = "";
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg);
                
                exception.reset(new DbDeleteException(*location, errorMsg, sqlCode, statement, length));
                return 0;
            }
        
            return GetAffectedRows(handle, location, exception);
        }
        
        long long DB2Engine::Update(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()
        {
            long long affectRows = 0;
        
            SQLRETURN rc;
            rc = SQLExecDirect(((Db2RealHandle*)handle)->hstmt, (SQLCHAR*)statement, length);
            
            if ( SQL_SUCCESS != rc && SQL_NO_DATA != rc )
            {
                int sqlCode  = rc;
                string       errorMsg = "";
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg );
                
                exception.reset(new DbUpdateException(*location, errorMsg, sqlCode, statement, length));
                return 0;
            }
        
            return GetAffectedRows(handle, location, exception);
        }
        
        long long DB2Engine::Truncate(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()
        {
            long long affectRows = 0;
        
            SQLRETURN rc;
            rc = SQLExecDirect(((Db2RealHandle*)handle)->hstmt, (SQLCHAR*)statement, length);
            
            if ( SQL_SUCCESS != rc && SQL_NO_DATA != rc )
            {
                int sqlCode  = rc;
                string       errorMsg = "";
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg );
        
                exception.reset(new DbTruncateException(*location, errorMsg, sqlCode, statement, length));
                return 0;
            }
        
            return GetAffectedRows(handle, location, exception);
        }
        
        long long DB2Engine::Insert(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()
        {
            SQLRETURN   rc;
            rc = SQLExecDirect(((Db2RealHandle*)handle)->hstmt, (SQLCHAR*)statement, length);
            
            if ( SQL_SUCCESS != rc && SQL_NO_DATA != rc )
            {
                int sqlCode  = rc;
                string       errorMsg = "";
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg );
        
                exception.reset(new DbInsertException(*location, errorMsg, sqlCode, statement, length));
                return 0;
            }
        
            return GetAffectedRows(handle, location, exception);
        }
        
        unsigned int DB2Engine::Execute(void* handle, DbLocation* location, const char* statement, size_t length, tr1::shared_ptr<IException>& exception) throw ()
        {    
            int sqlCode = 0;
            SQLRETURN   rc;
            rc = SQLExecDirect(((Db2RealHandle*)handle)->hstmt, (SQLCHAR*)statement, length);
            sqlCode = rc;
            
            if( SQL_SUCCESS != rc && SQL_NO_DATA != rc )
            {
                string errorMsg = "";
                errorMsg = ExtractErrMsgStmt(sqlCode, handle,errorMsg);
        
                exception.reset(new DbCommonExecuteException(*location, errorMsg, sqlCode, statement, length));
            }
            
            return sqlCode;
        }
        
        char* DB2Engine::EscapeString(
            void* handle,  DbLocation* location,  const char* src, long length, tr1::shared_ptr<IException>& exception) throw ()
        {
            memset(((Db2RealHandle*)handle)->inner_buf_, '\0', sizeof(((Db2RealHandle*)handle)->inner_buf_) );
            
			// replace "\0","\r","\t","\n","\b" not regarding the current charset
            stringstream ss;
            for (int i = 0; i < length; i++)
            {
                if (src[i] == '\0')
                {
                    ss << "\\0";
                }
                else if (src[i] == '\r')
                {
                    ss << "\\r";
                }
                else if (src[i] == '\t')
                {
                    ss << "\\t";
                }
                else if (src[i] == '\n')
                {
                    ss << "\\n";
                }
                else if (src[i] == '\'')
                {
                    ss << "\\'";
                }
                else
                {
                    ss << src[i];
                }
            }
            string rslt = ss.str();
            memcpy(((Db2RealHandle*)handle)->inner_buf_, rslt.data(), rslt.length());
            
            return (char*)(((Db2RealHandle*)handle)->inner_buf_);
        }
        
        void DB2Engine::InitThread()
        {
            // not necessary
        }
            
        void DB2Engine::UninitThread()
        {
            // not necessary
        }        
    }
}

#else
#warning "DB2 ENV NOT AVAILABLE... NOT COMPILED..."

#endif