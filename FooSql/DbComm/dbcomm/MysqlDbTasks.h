/// @file MysqlDbTasks.h
/// @brief 本文件定义了专门为MYSQL定制的 COMMON::DbTasks 对象。

/// @author 艾智杰
/// @date 2014/9/19
/// @file MysqlDbTasks.h
/// @brief The file defines a class that is an implement of @c IDbTasks, specific for MYSQL

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_MYSQLDBTASKS_H_
#define COMMON_DBCOMM_MYSQLDBTASKS_H_

#ifdef MYSQL_ENV_AVAILABLE

#include "dbcomm/DbTasks.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief The class is an implement of @c IDbTasks, specific for MYSQL
        class MysqlDbTasks : public DbTasks
        {
        public:
		    /// @brief Constructor
            /// @param dbLocations the database informations to the connections
            /// @param exception using the C++ exception mode?
            ///   - true C++ exception mode
            ///   - false the C style return code
            MysqlDbTasks(vector<DbLocation>& dbLocations, bool exception = false);
            
            virtual ~MysqlDbTasks() throw ();

            virtual DbExecuteAction* BatchInsertIgnore( int commitLimit, int valuesLimit);
            
            virtual DbExecuteAction* BatchReplace( int commitLimit, int valuesLimit);
            
            virtual DbQueryAction* GetPriKeys();
            
        protected:
            virtual bool InitEngine();
            virtual bool UninitEngine();
        };
    }
}

#endif

#endif