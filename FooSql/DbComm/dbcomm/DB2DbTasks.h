/// @file DB2DbTasks.h
/// @brief The file is the implement of @c DbTasks specific for DB2

/// @author Aicro Ai

#ifndef COMMON_DBCOMM_DB2DBTASKS_H_
#define COMMON_DBCOMM_DB2DBTASKS_H_

#ifdef DB2_ENV_AVAILABLE

#include "dbcomm/DbTasks.h"

namespace COMMON
{
    namespace DBCOMM
    {
        /// @brief This is the class implements the @c DbTasks specific for DB2
        class DB2DbTasks : public DbTasks
        {
        public:
            /// @brief Constructor
            /// @param dbLocations the database informations to the connections
            /// @param exception using the C++ exception mode?
            ///   - true C++ exception mode
            ///   - false the C style return code
            DB2DbTasks(vector<DbLocation>& dbLocations, bool exception = false);
            
            virtual ~DB2DbTasks() throw ();
            
            virtual DbExecuteAction* BatchInsertIgnore( int commitLimit, int valuesLimit);
            
            DbExecuteAction* BatchReplace( int commitLimit, int valuesLimit);
        
            virtual DbQueryAction* GetPriKeys();
        
        protected:
            virtual bool InitEngine();
            virtual bool UninitEngine();
        };   
    }
}

#endif

#endif