/// @file DbLocation.h
/// @brief The file defines a structure that having all the details of a connections, 
/// including IP, port, user name and password

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DBLOCATION_H_
#define COMMON_DBCOMM_DBLOCATION_H_

#include <string>
#include <iostream>
#include <stdio.h>

#include "tool/md5.h"

using namespace std;

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief The class representing all the details of a connection
        class DbLocation
        {
        private:
            string ip_;       
            string port_;     
            string db_id_;    
            string user_;     
            string password_; 
            
            mutable bool comfirmed_;  // is the detail comfirmed
            mutable string id_;       // a unique ID of the structures having the same details

        public:
            /// @brief Default constructor
            DbLocation() 
                :comfirmed_(true) {}

            /// @brief Constructor
            /// @param ip the IP of the connection
            /// @param port the PORT of the connection
            /// @param dbId the Target DB name
            /// @param user the user name
            /// @param password the password associated to the user name
            DbLocation(string ip, string port, string dbId, string user, string password)
            : ip_(ip), port_(port), db_id_(dbId), user_(user), password_(password), comfirmed_(true)
            {
            }

            /// @brief Get the DB name
            /// @return the DB name
            std::string GetDbId() const { return db_id_; }
            
            /// @brief Set the DB name explicitly
            /// @param val the db name
            void SetDbId(std::string val) { comfirmed_ = true; db_id_ = val; }

            /// @brief Get the port explicitly
            /// @return the port
            std::string GetPort() const { return port_; }
            
            /// @brief Set the port explicitly
            /// @param val the port
            void SetPort(std::string val) { comfirmed_ = true; port_ = val; }

            /// @brief Get the ip of the connection
            /// @return the IP
            std::string GetIp() const { return ip_; }
            
            /// @brief Set the IP explicitly
            /// @param val the IP
            void SetIp(std::string val) { comfirmed_ = true; ip_ = val; }

            /// @brief Get the user name explicitly
            /// @return the user name
            std::string GetUser() const { return user_; }
            
            /// @brief Set the user explicitly
            /// @param user the user name
            void SetUser(string user) { comfirmed_ = true; user_ = user; }

            /// @brief Get the password explicitly
            /// @return the password
            std::string GetPassword() const { return password_; }
            
            /// @brief Set the password explicitly
            /// @param psw the password
            void SetPassword(string psw) { comfirmed_ = true; password_ = psw; }

            /// @brief Test whether the two object have the same details
            /// @param other other object to compare
            /// @return whether the two object have the same details
            ///   - true they are the same
            ///   - false they are not the same
            bool operator==(const DbLocation & other) const
            {
                if (this == &other)
                {
                    return true;
                }

                if (   ip_        == other.ip_ 
                    && port_      == other.port_ 
                    && db_id_     == other.db_id_
                    && user_      == other.user_
                    && password_  == other.password_)
                {
                    return true;
                }

                return false;
            }
            
            /// @brief assignment
            /// @param other other object to copy
            /// @return the object copied
            DbLocation& operator=(const DbLocation& other)
            {
                if (this == &other)
                {
                    return *this;
                }
                
                this->ip_        = other.ip_;
                this->port_      = other.port_;
                this->db_id_     = other.db_id_;
                this->user_      = other.user_;
                this->password_  = other.password_;
                
                return *this;
            }
            
            // to make the structure able to be set in the std::set
            friend bool operator < (const DbLocation &my, const DbLocation &other)
            {
                return (my.ip_ + my.port_ + my.db_id_ + my.user_) < (other.ip_ + other.port_ + other.db_id_ + other.user_);
            }

            /// @brief Get the descriptions of the connection
            /// @return the descriptions of the connection
            string ToString() const
            {
                char buffer[512] = {0};
                sprintf(buffer, "DB_ID = [%s] PORT = [%s] IP = [%s] USER = [%s]", db_id_.c_str(), port_.c_str(), ip_.c_str(), user_.c_str());
                return buffer;
            }
            
            /// @brief get a unique ID from the details.
            /// @return the unique ID of the connection
            string ID() const
            {
                if (id_ == "" || comfirmed_ == true)
                {
                    char buffer[512] = {0};
                    sprintf(buffer, "%s%s%s%s", db_id_.c_str(), port_.c_str(), ip_.c_str(), user_.c_str());
                    COMMON::TOOL::MD5 t(buffer);
                    id_ = t.md5();
                    
                    comfirmed_ = false;
                }
                
                return id_;
            }
            
            /// @brief get a unique ID from the details.
            /// @return the unique ID of the connection
            string ID()
            {
                return ((const DbLocation*)this)->ID();
            }
        } ;
    }
}


#endif
