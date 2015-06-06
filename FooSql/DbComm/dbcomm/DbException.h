/// @file DbException.h
/// @brief The file defines some exceptions that may occur during a DB operations.

/// @author Aicro 
/// @date 2015/4/19

#ifndef COMMON_EXCEPTION_DBEXCEPTION_H_
#define COMMON_EXCEPTION_DBEXCEPTION_H_

/* class heirarchy
IException
  
**DbException 
----DbConnectException
----DbDisconnectException
----DbSelectException
------DbSelectFetchRowException
------DbSelectGetColumnsLengthException
------DbSelectExecuteException
------DbSelectOpenException
------DbSelectCloseException
----DbExecuteException
------DbCommonExecuteException
------DbDeleteException
------DbTruncateException
------DbUpdateException
------DbInsertException
--------DbInsertDuplicateKeyException
----DbCommitException
----DbGetAffectedRowsException
*/

#include <sstream>
#include <algorithm>
#include <vector>
#include <execinfo.h>

#include "exception/IException.h"
#include "dbcomm/DbLocation.h"

using namespace std;

namespace COMMON
{
	namespace EXCEPTION
	{
		namespace DB
		{
            //------------------------------------------------------------------------------
            // The base class for DB exceptions
            //------------------------------------------------------------------------------
            
			/// @brief The bas class for DB exceptions. The class has some necessary 
			/// information about the exception, such as error code, descriptions.
			/// Besides, it also provides some methods to express them to the user.
			class DbException : public IException
			{
			private:
				// the DB information that occurs error
				DBCOMM::DbLocation   db_location_;
				
				// the error descriptions about the exception
				string       error_msg_;

				// the error code associated with the exception
				int error_num_;

				// should we print out the password?
				bool print_password_;
				
				// the current SQL statement that cause the exception
				string statement_;

			public:
				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param printPassword should we print out the password? the default is 'false'
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				DbException(DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false)
				{
					print_password_ = printPassword;

					db_location_ = location;
					error_msg_ = error;
					error_num_ = errorCode;
					
					if (statement == 0 || length == 0)
					{
						statement_ = "UNSUPPORTED";
					}
					else
					{
						statement_ = string(statement, length);
					}
				}

				/// @brief Default Constructor
				DbException()
				{
					print_password_ = false;
				}

				virtual ~DbException() throw () {}

            	/// @brief Get the exception description
				/// @param needDetail should we print all the details? the default is false
				/// @return the exception description in detail
				virtual string What(bool needDetail = false) const throw ()
				{
					return "Database Exception : ";
				}

				/// @brief Get the full name of the class, you can use it as the ID
				/// @return the full name of the class
				virtual std::string ToString() const throw()
				{
					return IException::ToString() + "." + "DbException";
				}
                
            public: 
				/// @brief Get the DB information that occur the exception
				/// @return The DB information that occur the exception
				virtual const DBCOMM::DbLocation* GetExpDbLocation() { return &db_location_; }

				/// @brief Get the error descriptions about the exception
				/// @return the error descriptions about the exception
				virtual const string GetExpErrorMsg() { return error_msg_; }

				/// @brief Get the error code about the exception
				/// @return The error code about the exception
				virtual const int GetExpErrorNum() { return error_num_; }
				
				/// @brief Get the current SQL statement when exception occurs
				/// @return The current SQL statement when exception occurs
				virtual const string GetStatementWhenError() { return statement_; }

			protected:
				// Get detail information
				const string GetDetailInfo(bool needStatement = false) const
				{
					stringstream ss;
					ss  << "IP      = " << db_location_.GetIp() << endl
						<< "PORT    = " << db_location_.GetPort() << endl
						<< "DbNm    = " << db_location_.GetDbId() << endl
						<< "USER    = " << db_location_.GetUser() << endl;
					if (print_password_ == true)
					{
						ss << "PASS    = " << db_location_.GetPassword() << endl;
					}

					ss << "SqlError = " << error_num_ << endl;
					ss << "ErrorMsg = " << error_msg_ << endl;
                    
                    if (needStatement)
                    {
                        ss << "Statement = " << statement_ << endl;    
                    }

					return ss.str();
				}
			};

            //------------------------------------------------------------------------------
            // Concrete DB Exception
            //------------------------------------------------------------------------------
			/// @brief A connection exception
			class DbConnectException : public DbException
			{
			public:
				virtual ~DbConnectException() throw () {}
			
				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbConnectException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbException(location, error, errorCode, statement, length, printPassword)
				{
				}

				DbConnectException()
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbException::What(needDetail) << "An exception occurs during CONNECTION. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbException::ToString() + "." + "DbConnectException";
				}
			};
            
			/// @brief A disconnect exception
			class DbDisconnectException : public DbException
			{
			public:
				virtual ~DbDisconnectException() throw () {}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'				
				DbDisconnectException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbException(location, error, errorCode, statement, length, printPassword)
				{
				}

				DbDisconnectException()
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbException::What(needDetail) << "An exception occurs during DISCONNECT. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbException::ToString() + "." + "DbDisconnectException";
				}
			};

			/// @brief An exception happens during QUERY
			class DbSelectException : public DbException
			{
			public:
				virtual ~DbSelectException() throw () {}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbSelectException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbException(location, error, errorCode, statement, length, printPassword)
				{
				}
				
				DbSelectException() :DbException() {}
				
				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbException::What(needDetail) << "An exception occurs during QUERY, here are the details\n";
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbException::ToString() + "." + "DbSelectException";
				}
			};

			/// @brief An exception occurs during trying to fetch a row from an open result set
			class DbSelectFetchRowException : public DbSelectException
			{
			public:
				virtual ~DbSelectFetchRowException() throw () {}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'				
				DbSelectFetchRowException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbSelectException(location, error, errorCode, statement, length, printPassword)
				{
				}

				DbSelectFetchRowException()
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbSelectException::What(needDetail) << "An exception occurs during trying to fetch a row from an open result set. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbSelectException::ToString() + "." + "DbSelectFetchRowException";
				}
			};

			/// @brief An exception occurs during trying to get lengths for each column for current row
			class DbSelectGetColumnsLengthException : public DbSelectException
			{
			public:
				virtual ~DbSelectGetColumnsLengthException() throw () {}
			
				DbSelectGetColumnsLengthException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'	
				DbSelectGetColumnsLengthException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbSelectException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbSelectException::What(needDetail) << "An exception occurs during trying to get lengths for each column for current row. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbSelectException::ToString() + "." + "DbSelectGetColumnsLengthException";
				}
			};

			/// @brief An exception occurs during trying to get affected rows of the last statement
			class DbGetAffectedRowsException : public DbException
			{
			public:
				virtual ~DbGetAffectedRowsException() throw () {}
			
				DbGetAffectedRowsException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbGetAffectedRowsException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbException::What(needDetail) << "An exception occurs during trying to get affected rows of the last statement. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbException::ToString() + "." + "DbGetAffectedRowsException";
				}
			};

			/// @brief An exception occurs during executing a select statement
			class DbSelectExecuteException : public DbSelectException
			{
			public:
				virtual ~DbSelectExecuteException() throw () {}
			
				DbSelectExecuteException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbSelectExecuteException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbSelectException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbSelectException::What(needDetail) << "An exception occurs during executing a select statement. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbSelectException::ToString() + "." + "DbSelectExecuteException";
				}
			};

			/// @brief An exception occurs during open a result set
			class DbSelectOpenException : public DbSelectException
			{
			public:
				virtual ~DbSelectOpenException() throw () {}
			
				DbSelectOpenException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbSelectOpenException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbSelectException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbSelectException::What(needDetail) << "An exception occurs during open a result set. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbSelectException::ToString() + "." + "DbSelectOpenException";
				}
			};

			/// @brief An exception occurs during closing a result set
			class DbSelectCloseException : public DbSelectException
			{
			public:
				virtual ~DbSelectCloseException() throw () {}
			
				DbSelectCloseException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbSelectCloseException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbSelectException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbSelectException::What(needDetail) << "An exception occurs during closing a result set. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbSelectException::ToString() + "." + "DbSelectCloseException";
				}
			};

			/// @brief An exception occurs during a specific execution
			class DbExecuteException : public DbException
			{
			public:
				virtual ~DbExecuteException() throw () {}
				
				DbExecuteException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbExecuteException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbException::What(needDetail) << "An exception occurs during a specific execution. Here are the details\n";
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbException::ToString() + "." + "DbExecuteException";
				}
			};
			
            /// @brief An exception occurs during a common execution
            class DbCommonExecuteException : public DbExecuteException
            {
            public:
				virtual ~DbCommonExecuteException() throw () {}
				
				DbCommonExecuteException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbCommonExecuteException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbExecuteException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbExecuteException::What(needDetail) << "An exception occurs during a common execution. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbExecuteException::ToString() + "." + "DbCommonExecuteException";
				}    
            };
            
			/// @brief The exception occurs during updating
			class DbUpdateException : public DbExecuteException
			{
			public:
				virtual ~DbUpdateException() throw () {}
				
				DbUpdateException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbUpdateException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbExecuteException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbExecuteException::What(needDetail) << "The exception occurs during updating. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbExecuteException::ToString() + "." + "DbUpdateException";
				}
			};
			
			/// @brief The exception occurs during Truncating a table
			class DbTruncateException : public DbExecuteException
			{
			public:
				virtual ~DbTruncateException() throw () {}
				
				DbTruncateException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbTruncateException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbExecuteException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbExecuteException::What(needDetail) << "The exception occurs during Truncating a table. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbExecuteException::ToString() + "." + "DbTruncateException";
				}
			};
			
			/// @brief The exception occurs during deleting
			class DbDeleteException : public DbExecuteException
			{
			public:
				virtual ~DbDeleteException() throw () {}
				
				DbDeleteException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbDeleteException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbExecuteException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbExecuteException::What(needDetail) << "The exception occurs during deleting. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbExecuteException::ToString() + "." + "DbDeleteException";
				}
			};

			/// @brief The exception occurs during inserting
			class DbInsertException : public DbExecuteException
			{
			public:
				virtual ~DbInsertException() throw () {}

				DbInsertException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbInsertException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbExecuteException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbExecuteException::What(needDetail) << "The exception occurs during inserting. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbExecuteException::ToString() + "." + "DbInsertException";
				}
			};

			/// @brief The duplicate key exception
			class DbInsertDuplicateKeyException : public DbInsertException
			{
			public:
				virtual ~DbInsertDuplicateKeyException() throw () {}

				DbInsertDuplicateKeyException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbInsertDuplicateKeyException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbInsertException(location, error, errorCode, statement, length, printPassword)
				{
				}

				// No need to override What(), because we can use the parent's.

				virtual std::string ToString() const throw()
				{
					return DbInsertException::ToString() + "." + "DbInsertDuplicateKeyException";
				}
			};
			
			/// @brief An exception occurs during commit
			class DbCommitException : public DbException
			{
			public:
				virtual ~DbCommitException() throw () {}
			
				DbCommitException()
				{
				}

				/// @brief Constructor
				/// @param locations the DB information that occurs error
				/// @param error descriptions about the exception
				/// @param errorCode the error code associated with the exception
				/// @param statement the buffer for the current SQL statement that cause the exception
				/// @param length the length of the buffer
				/// @param printPassword should we print out the password? the default is 'false'
				DbCommitException( DBCOMM::DbLocation& location, string& error, int& errorCode, const char* statement, long length, bool printPassword = false )
					: DbException(location, error, errorCode, statement, length, printPassword)
				{
				}

				virtual string What(bool needDetail = false) const throw ()
				{
					stringstream ss;
					ss << DbException::What(needDetail) << "An exception occurs during commit. Here are the details\n";
					ss << GetDetailInfo(needDetail);
					return ss.str();
				}

				virtual std::string ToString() const throw()
				{
					return DbException::ToString() + "." + "DbCommitException";
				}
			};
		}
	}
}

#endif
