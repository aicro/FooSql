/// @file CodingException.h
/// @brief The file defines different exception that will happen in coding.

/// @author Aicro Ai
/// @date 2015/2/19

#ifndef COMMON_EXCEPTION_CODINGEXCEPTION_H_
#define COMMON_EXCEPTION_CODINGEXCEPTION_H_

/* Class heritage
IException

**CodingException 
    ----PrevWorkNotFinishedException
    ----UnknownActionException
    ----CodeSequenceException
    ----ObjectNotExistingInContainerException
    ----WorkNumNoMatchResultNumException
    ----DuplicateException
    ----ObjectNotFoundException
    ----ParamTypeNotMatchException
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
		/// @brief The base class for other coding exception.
		class CodingException : public IException
		{
		public:
			virtual ~CodingException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
				return "Coding exception.";
			}

			virtual std::string ToString() const throw()
			{
				return IException::ToString() + "." + "CodingException";
			}
		};

		/// @brief Unknown action exception. The exception will happen when an unknown
		/// action is activated.
		class UnknownActionException : public CodingException
		{
		public:
			/// @brief Constructor
			/// @param actionName the description of the unknown action.
			UnknownActionException(string actionName)
                : action_name_(actionName) {}
			
			virtual ~UnknownActionException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << CodingException::What(needDetail) << " An unknown exception occurs : " << action_name_ << endl;
				return ss.str();
			}

			virtual std::string ToString() const throw()
			{
				return CodingException::ToString() + "." + "UnknownActionException";
			}

        private:
            string action_name_;
		};

		/// @brief The class defines a kind of exception that is associated with previous action.
		/// That is when the last action is not finished, and we are trying to start a new one.
		class PrevWorkNotFinishedException : public CodingException
		{
		public:
			/// @brief Constructor
			PrevWorkNotFinishedException() {}
			
			virtual ~PrevWorkNotFinishedException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << CodingException::What(needDetail) << "A new action cannot start until the last action finished.\n";

				return ss.str();
			}

			virtual std::string ToString() const throw()
			{
				return CodingException::ToString() + "." + "PrevWorkNotFinishedException";
			}
		};
		
		/// @brief The exception happens when the operation is in its wrong sequence.
		class CodeSequenceException : public CodingException
		{
		public:
			/// @brief Constructor
			CodeSequenceException() {}
			
			virtual ~CodeSequenceException() throw () {}

			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << CodingException::What(needDetail) << "\nThe operation is in the wrong sequence.";

				return ss.str();
			}

			virtual std::string ToString() const throw()
			{
				return CodingException::ToString() + "." + "CodeSequenceException";
			}
		};
		
		/// @brief The class represents the exception that the object trying to find is not in the container.
		class ObjectNotExistingInContainerException : public CodingException
		{
		public:
			/// @brief Constructor
			/// @param obj the object not found in the container.
			/// @param the extra information provided by the user.
			ObjectNotExistingInContainerException(string obj, string reason = "")
                : obj_name_(obj), reason_(reason){}
			
			virtual ~ObjectNotExistingInContainerException() throw () {}
		
			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << CodingException::What(needDetail) << "The object [" << obj_name_ << "] is not found in the container.";
                if (reason_ != "")
                {
                    ss << "Detail : " << reason_ << endl;
                }

				return ss.str();
			}

			virtual std::string ToString() const throw()
			{
				return CodingException::ToString() + "." + "ObjectNotExistingInContainerException";
			}

        private:
            string obj_name_;
            string reason_;
		};
		
		/// @brief The class represents that the number of work provided does not match that of the result.
		class WorkNumNoMatchResultNumException : public CodingException
		{
		public:
			/// @brief Constructor
			/// @param workNumber the number of work
			/// @param resultNumber the number of result
			/// @param detail details to inform the user
			WorkNumNoMatchResultNumException(int workNumber, int resultNumber, string detail) 
                : work_number_(workNumber), result_number_(resultNumber), detail_(detail) {}
			
			virtual ~WorkNumNoMatchResultNumException() throw () {}
		
			virtual string What(bool needDetail = false) const throw ()
			{
				stringstream ss;
				ss << CodingException::What(needDetail) << "The number of work provided does not match that of the result.";
                ss << "work number : " << work_number_ << ", result number : " << result_number_ << ", details : " << detail_;
		
				return ss.str();
			}

			virtual std::string ToString() const throw()
			{
				return CodingException::ToString() + "." + "WorkNumNoMatchResultNumException";
			}
            
        protected:
            int work_number_;
            int result_number_;
            string detail_;
		};

		/// @brief The class represents the exception that a duplicate item occurs in the 
		/// case that it should not happen.
        class DuplicateException : public CodingException
        {
        public:
            /// @brief constructor.
			/// @param duplicateItem the duplicate item that caused the exception
            DuplicateException(string duplicateItem) 
                : dup_(duplicateItem) {}

            virtual ~DuplicateException() throw () {}

            virtual string What(bool needDetail = false) const throw ()
            {
                stringstream ss;
                ss << CodingException::What(needDetail) << "A duplicate item [" << dup_ << "] occurs in the case that it should be a unique one.\n";

                return ss.str();
            }

            virtual std::string ToString() const throw()
            {
                return CodingException::ToString() + "." + "DuplicateException";
            }

        private:
            string dup_;
        };

        /// @brief This class represents that the object not found.
        class ObjectNotFoundException : public CodingException
        {
        public:
            /// @brief Constructor
			/// @param objName The object name that is not found.
			/// @param reason The details to inform the user.
            ObjectNotFoundException(string objName, string reason = "") 
                :obj_name_(objName), reason_(reason) {}

            virtual ~ObjectNotFoundException() throw () {}

            virtual string What(bool needDetail = false) const throw ()
            {
                stringstream ss;
                ss << CodingException::What(needDetail) << "The object [" << obj_name_ << "] is not found.";
                if (reason_ != "")
                {
                    ss << "Details : " << reason_ << endl;
                }

                return ss.str();
            }

            virtual std::string ToString() const throw()
            {
                return CodingException::ToString() + "." + "ObjectNotFoundException";
            }

        private:
            string obj_name_;
            string reason_;
        };

		/// @brief The class defines the exception that the input param's type does not match that of the need.
        class ParamTypeNotMatchException : public CodingException
        {
        public:
            /// @brief Constructor
            ParamTypeNotMatchException() {}

            virtual ~ParamTypeNotMatchException() throw () {}

            virtual string What(bool needDetail = false) const throw ()
            {
                stringstream ss;
                ss << CodingException::What(needDetail) << "The type of the parameter does not match that of the need";

                return ss.str();
            }

            virtual std::string ToString() const throw()
            {
                return CodingException::ToString() + "." + "ParamTypeNotMatchException";
            }
        };
	}
}

#endif
