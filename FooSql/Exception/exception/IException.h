/// @file IException.h
/// @brief The file defines an interface for exception used in the frame.

/// @author Aicro Ai
/// @date 2015/2/19

#ifndef COMMON_EXCEPTION_IEXCEPTION_H_
#define COMMON_EXCEPTION_IEXCEPTION_H_

#include <tr1/memory>
#include <string>
#include <execinfo.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

namespace COMMON
{
	namespace EXCEPTION
	{
		/// @brief An abstract exception interface representing exceptions in the frame. 
		/// This is only compatible with the @c ThrowableException .
		class IException
		{
		public:
			/// @brief Constructor
			IException()
			{
				FillStackTrace();
            }
		
			/// @brief Deconstructor
			virtual ~IException() throw () {}
			
            /// @brief Get details of the exception with stack information.
			/// @return Details with stack information.
			virtual std::string WhatWithStackInfo(bool needDetail = false) const
            {
                stringstream ss;
                ss << "Stack information\n" << StackTrace() << "\n" << What(needDetail);
                
                return ss.str();
            }
            
			/// @brief Get details of the exception.
			/// @return Details of the exception.
			virtual std::string What(bool needDetail = false) const throw () = 0;
			
			/// @brief Get the plain descriptions of the exception
			/// @return the plain descriptions of the exception
			virtual std::string ToString() const throw()
			{
				return "COMMON::IException";
			}

			/// @brief Get the stack trace of the exception.
			/// @return the stack trace of the exception.
			const string StackTrace() const throw()
			{
				return stack_;
			}
			
			/// @brief Test the input exception is the same as the inner exception, which is 
			/// equivalent to operator==() .
			/// @return Whether the input exception is the same as the inner exception.
			///   - true Both belong to the same type.
			///   - false They are not the same type.
			virtual bool IsSameType(tr1::shared_ptr<IException> e)
			{
				return ToString() == e->ToString();
			}
			
			/// @brief Test the input exception is the same as the inner exception, which is 
			/// equivalent to IsSameType() .
			/// @return Whether the input exception is the same as the inner exception.
			///   - true Both belong to the same type.
			///   - false They are not the same type.
			virtual bool operator==(tr1::shared_ptr<IException> e)
			{
				return IsSameType(e);
			}
			
		private:
			void FillStackTrace() throw()
			{
				const int len = 512;
				void* buffer[len];
				int nptrs = ::backtrace(buffer, len);
				char** strings = ::backtrace_symbols(buffer, nptrs);
				if (strings)
				{
					for (int i = 0; i < nptrs; ++i)
					{
						stack_.append(strings[i]);
						stack_.push_back('\n');
					}
					free(strings);
				}
			}
			
		private:
			string stack_;
		};
	}
}

#endif