#include "exception/IException.h"
#include "exception/ThrowableException.h"

namespace COMMON
{
    namespace EXCEPTION
    {
        ThrowableException::ThrowableException()
        {
        }

        ThrowableException::ThrowableException(const ThrowableException* other)
        {
            CopyException(other->exceptions_);
        }

        ThrowableException::ThrowableException(ThrowableException& other)
        {
            CopyException(other.exceptions_);
        }

        ThrowableException::ThrowableException(vector<tr1::shared_ptr<IException> >& in)
        {
            CopyException(in);
        }

        ThrowableException::ThrowableException(tr1::shared_ptr<IException> in)
        {
            exceptions_.push_back(in);
        }

        std::string ThrowableException::What(bool needDetail) const
        {
            stringstream ss;

            ss << "There are totally " << exceptions_.size() << " exceptions happened." << endl;
            for (int i = 0; i < exceptions_.size(); i++)
            {
                ss << "Exception No." << i + 1 << endl;    
                ss << exceptions_[i]->WhatWithStackInfo(needDetail);
            }

            return ss.str();
        }

        vector<tr1::shared_ptr<IException> >& ThrowableException::GetInnerExceptions()
        {
            return exceptions_;
        }

        void ThrowableException::CopyException(const vector<tr1::shared_ptr<IException> >& exps)
        {
            exceptions_.clear();

            for (int i = 0; i < exps.size(); i++)
            {
                exceptions_.push_back(exps[i]);
            }
        }

    }
}