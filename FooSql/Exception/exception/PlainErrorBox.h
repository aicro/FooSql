/// @file PlainErrorBox.h
/// @brief A plain error box that contains raw information of the exception.

/// @author Aicro Ai
/// @date 2014/2/19

#ifndef COMMON_EXCEPTION_PLAIN_ERRORBOX_H_
#define COMMON_EXCEPTION_PLAIN_ERRORBOX_H_

#include <string>

using namespace std;

namespace COMMON
{
    namespace EXCEPTION
    {
        /// @brief A container for the raw information of the exception.
        class PlainErrorBox
        {
        private:
            string err_msg_;
            int err_no_;

        public:
            /// @brief Constructor
            PlainErrorBox()
            {
                err_no_ = 0;
            }

            /// @brief Get the error message.
            /// @return The detail error message
            string GetErrMsg()
            {
                return err_msg_;
            }

            /// @brief Set the error message
            /// @param errMsg error message to be set
            void SetErrMsg(string errMsg)
            {
                err_msg_ = errMsg;
            }

            /// @brief Get the error number
            /// @return the error number
            int GetErrNo()
            {
                return err_no_;
            }

            /// @brief Set the error number
            /// @param errNo the error number
            void SetErrNo(int errNo)
            {
                err_no_ = errNo;
            }
        };
    }
}

#endif