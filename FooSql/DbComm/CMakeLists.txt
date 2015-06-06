set(base_SRCS
  BatchFilter.cpp
  DbAction.cpp
  DbBatchAction.cpp
  DbEngine.cpp
  DbExecuteAction.cpp
  DbExecuteRslt.cpp
  DbQueryAction.cpp
  DbQueryRslt.cpp
  DbRslt.cpp
  DbTasks.cpp
  EscapeStringAction.cpp
  Row.cpp
  StmtGenerator.cpp
  DB2DbTasks.cpp
  DB2Engine.cpp
  DB2StmtGen.cpp
  MysqlDbTasks.cpp
  MysqlEngine.cpp
  MysqlStmtGen.cpp
  )

# check for MYSQL, if there has installed MYSQL, the output will be 
# the path of MYSQL header, such as "/usr/include/mysql"
message(STATUS "CHECKING MYSQL ...")

execute_process(COMMAND mysql_config --variable=pkgincludedir OUTPUT_VARIABLE MYSQL_HEADER_PATH)
if(MYSQL_HEADER_PATH)
message(STATUS "MYSQL INSTALLED, HEADER PATH IS " ${MYSQL_HEADER_PATH})

#enable macro MYSQL_ENV_AVAILABLE in the code
add_definitions(-DMYSQL_ENV_AVAILABLE)

#add include path
include_directories(${MYSQL_HEADER_PATH})

#add lib path
execute_process(COMMAND mysql_config --variable=pkglibdir OUTPUT_VARIABLE MYSQL_LIB_PATH)
link_directories(${MYSQL_LIB_PATH})

else(MYSQL_HEADER_PATH)
message(STATUS "MYSQL NOT INSTALLED")
endif(MYSQL_HEADER_PATH)

# check for DB2, if there has installed DB2, the DB2_HOME system 
# environment variable will not be empty
message(STATUS "CHECKING DB2 ...")

# note that no "$" should be added in the "if" statement
if(ENV{DB2_HOME})
message(STATUS "DB2 INSTALLED, HOME PATH IS " $ENV{DB2_HOME})

#enable macro DB2_ENV_AVAILABLE in the code
add_definitions(-DDB2_ENV_AVAILABLE)

#add include path
include_directories($ENV{DB2_HOME}/include)

#add lib path
link_directories($ENV{DB2_HOME}/lib)

else(ENV{DB2_HOME})
message(STATUS "DB2 NOT INSTALLED")
endif(ENV{DB2_HOME})

include_directories(
	${PROJECT_SOURCE_DIR}/FooSql/DbComm 
	${PROJECT_SOURCE_DIR}/FooSql/Exception 
	${PROJECT_SOURCE_DIR}/FooSql/Thread
	${PROJECT_SOURCE_DIR}/FooSql/Tool)

add_library(foosqldbcomm SHARED ${base_SRCS})

# install headers to ${CMAKE_INSTALL_PREFIX}/<DESTINATION>
install(DIRECTORY ${PROJECT_SOURCE_DIR}/FooSql/DbComm/dbcomm DESTINATION include/FooSql)

# install lib to ${CMAKE_INSTALL_PREFIX}/<DESTINATION>
install(TARGETS foosqldbcomm DESTINATION lib/FooSql/lib)