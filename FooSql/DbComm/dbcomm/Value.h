/// @file Value.h
/// @brief The file defines a class that may help the user to 
/// generate a value with correct format in a SQL statement.

/// @author Aicro Ai
/// @date 2015/2/19

#ifndef COMMON_DBCOMM_VALUE_H_
#define COMMON_DBCOMM_VALUE_H_

#include <string.h>
#include <sstream>
#include <iomanip>

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
		/// @brief The class defines a way to help the user to generate a value with 
		/// correct format in a SQL statement. For example, if we have input a value 
		/// which is defined as CHAR, VARCHAR or some other string related type in DBMS, 
		/// two quotation marks will be added; however, no quotation marks will be put 
		/// around an INTEGER or DOUBLE value.
        class Value
        {
        public:
            /// @brief Default constructor
            Value()
            {
            }
            
            /// @brief Constructor
            /// @param intValue a value represents an INTEGER or related number type in its DBMS
            Value(int intValue)
            {
                stringstream ss;
                ss << intValue;
                contents_ = ss.str();
            }
            /// @brief Constructor
            /// @param longValue a value represents an number type in its DBMS
            Value(long longValue)
            {
                stringstream ss;
                ss << longValue;
                contents_ = ss.str();
            }
            
            /// @brief Constructor
            /// @param longValue a value represents an INTEGER or related number type in its DBMS
            Value(long long longlongValue)
            {
                stringstream ss;
                ss << longlongValue;
                contents_ = ss.str();
            }
            
            /// @brief Constructor
            /// @param doubleValue A value represents as DOUBLE or REAL type in its DBMS
            Value(double doubleValue)
            {
                stringstream ss;
                ss << std::fixed << doubleValue;
                contents_ = ss.str();
            }

            /// @brief Constructor
            /// @param value The value to input
            /// @param noNeedQuote true means no need to put a quotation mark around value, the default is false.
            /// @param needHex true, the default one, means we want to convert the value to its hex format explicitly.
			/// @note This function may be useful in such case as 
			/// 1. we want to input a function rather than an ordinary string value. For example, in many DBMS, NOW() 
			/// is a valid function, and we can use this function in the insert or update statement. In this case, 
			/// we don't want it to be quoted.
			/// 2. Some characters may be invalid in its original format because of the charset or C/C++ grammar. For example, 
			/// a '\0' should be taken special care in a string for it is generally be regarded as the end of a string. Other 
			/// example is that when the user has input some Chinese characters in the C/C++ codes to generate a statement, 
			/// they may be misunderstood as invalid string. Under this circumstance, the best way to deal with is to convert 
			/// them into hex.
            Value(string value, bool noNeedQuote = false, bool needHex = false)
            {
			    SetValue(value, noNeedQuote, needHex);
            }

            /// @brief Constructor
            /// @param length the length of the value buffer
            /// @param value the buffer for the value
            /// @param noNeedQuote true means no need to put a quotation mark around value, the default is false.
            /// @param needHex true, the default one, means we want to convert the value to its hex format explicitly.
            Value(long length, const char* value, bool noNeedQuote = false, bool needHex = false)
            {
                string tmp(value, length);
			    SetValue(tmp, noNeedQuote, needHex);
            }

            /// @brief Copy constructor
            /// @param other other value to copy
            Value(const Value& other)
            {
                *this = other;
            }
			
            /// @brief Get the inner contents
            /// @return the inner contents
			string GetValue()
			{
			    return contents_;
			}
            
            /// @brief Copy the other value
            /// @return The value to be copied
			Value& operator=(const Value& other)
			{
			    contents_ = other.contents_;
                return *this;
			}
		
		private:
		    void SetValue(string& value, bool noNeedQuote, bool needHex)
			{
                stringstream ss;
                if (needHex)
                {
                    ss << "X";
                }
                
	            if (!noNeedQuote)
                {
                    ss << "'";
                }
                
                if (needHex)
                {
                    ss << std::hex << std::setfill('0');
                    for (int i = 0; i < value.size(); i++)
                    {
						// the std::stringstream regards the value to be buffered as a 32 bit integer,
						// rather than a 8 bit single char. Therefore, the high 3 bytes will be completed 
						// with 0xFFFFFF, which is not what we want. 
						// Details can be found in 
                        // http://zhuoqiang.me/string-hex-format-output-in-cpp.html
                        ss << std::setw(2) << (static_cast<unsigned>(value[i]) & 0x000000FF);
                    }
                    ss << std::dec;
                }
                else
                {
                    ss << value;
                }
                
                if (!noNeedQuote)
                {
                    ss << "'";
                }    
                
                contents_ = ss.str();
			}	
			
	    private:
		    string contents_;
        };
    }
}

#endif