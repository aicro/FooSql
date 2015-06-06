/// @file FileException.h
/// @brief The file defines some exceptions about file operation.

/// @author Aicro Ai
/// @date 2015/2/19

#ifndef COMMON_EXCEPTION_FILE_EXCEPTION_H_
#define COMMON_EXCEPTION_FILE_EXCEPTION_H_

/* Class heritage

IException
	FileException
		----FileOpenException
		----FileCloseException
		----FileReadException
		----FileWriteException
*/
#include <map>
#include <string>
#include <sstream>

#include "exception/IException.h"

using namespace std;

namespace COMMON
{
	namespace EXCEPTION
	{
		/// @brief The base class for other file exception.
		class FileException : public IException
		{
		public:
            /// @brief constructor
			/// @param fileName The name of the file which occur errors
			/// @param errorNo The error number
			/// @param errMsg  The Message for the detail
            FileException(string fileName, int errorNo, string errMsg)
                : file_name_(fileName), err_no_(errorNo), err_msg_(errMsg) {}
        
			virtual ~FileException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
                stringstream ss;
                ss << "File [" << file_name_ << "] occurs error. Error number[" <<  err_no_ << "], Details[" << err_msg_ << "].";
				return ss.str();
			}

			virtual string ToString() const throw()
			{
				return IException::ToString() + "." + "FileException";
			}
            
        protected:
            string file_name_;
            int err_no_;
            string err_msg_;
		};

		/// @brief The class represents the exception during opening file.
		class FileOpenException : public FileException
		{
		public:
			/// @brief constructor
			/// @param fileName The name of the file which occur errors
			/// @param errorNo The error number
			/// @param errMsg  The Message for the detail
			FileOpenException(string fileName, int errorNo, string errMsg)
                : FileException(fileName, errorNo, errMsg) {}
			
			virtual ~FileOpenException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << "Errors occur during trying to open file.\n " << FileException::What(needDetail);
				return ss.str();
			}

			virtual string ToString() const throw()
			{
				return FileException::ToString() + "." + "FileOpenException";
			}
		};

		/// @brief The class represents the exception during closing file.
		class FileCloseException : public FileException
		{
		public:
			/// @brief constructor
			/// @param fileName The name of the file which occur errors
			/// @param errorNo The error number
			/// @param errMsg  The Message for the detail
			FileCloseException(string fileName, int errorNo, string errMsg)
                : FileException(fileName, errorNo, errMsg) {}
			
			virtual ~FileCloseException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << "Errors occur during closing files.\n" << FileException::What(needDetail);
				return ss.str();
			}

			virtual string ToString() const throw()
			{
				return FileException::ToString() + "." + "FileCloseException";
			}
		};
        
		/// @brief The class represents the errors occur during reading file.
		class FileReadException : public FileException
		{
		public:
			/// @brief constructor
			/// @param fileName The name of the file which occur errors
			/// @param errorNo The error number
			/// @param errMsg  The Message for the detail
			FileReadException(string fileName, int errorNo, string errMsg)
                : FileException(fileName, errorNo, errMsg) {}
			
			virtual ~FileReadException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << "Errors occur during reading files.\n" << FileException::What(needDetail);
				return ss.str();
			}

			virtual string ToString() const throw()
			{
				return FileException::ToString() + "." + "FileReadException";
			}
		};
        
		/// @brief The class represents errors happen during writing file.
		class FileWriteException : public FileException
		{
		public:
			/// @brief constructor
			/// @param fileName The name of the file which occur errors
			/// @param errorNo The error number
			/// @param errMsg  The Message for the detail
			FileWriteException(string fileName, int errorNo, string errMsg)
                : FileException(fileName, errorNo, errMsg) {}
			
			virtual ~FileWriteException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << "Error occur during writing file.\n" << FileException::What(needDetail);
				return ss.str();
			}

			virtual string ToString() const throw()
			{
				return FileException::ToString() + "." + "FileWriteException";
			}
		};
	}
}

#endif
