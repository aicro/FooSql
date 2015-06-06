/// @file ErrorBox.h
/// @brief The file defines a storage box for exception, which follows MEMORIAL design pattern.

/// @author Aicro Ai
/// @date 2015/2/19

#ifndef COMMON_EXCEPTION_ERRORBOX_H_
#define COMMON_EXCEPTION_ERRORBOX_H_

#include <string>
#include "exception/ThrowableException.h"

using namespace std;

namespace COMMON
{
    namespace EXCEPTION
    {
        /// @brief The class represents a storage box containing exceptions.
		/// It always acts as a bridge between classes to express exceptions.
        class ErrorBox
        {
        private:
            tr1::shared_ptr<ThrowableException> exception_;

        public:
            /// @brief Get details of last error.
            /// @return The detail of the last error.
            string GetLastError()
            {
                return exception_->What();
            }

            /// @brief Set exceptions
            /// @param exception the point of the exception that happens.
            void SetException(tr1::shared_ptr<ThrowableException> exception)
            {
                exception_ = exception;
            }

            /// @brief Get the last error that happens.
            /// @return The points to the last error.
            tr1::shared_ptr<ThrowableException> GetLastException()
            {
                return exception_;
            }
        };
    }
}

#endif