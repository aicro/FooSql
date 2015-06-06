set(base_SRCS
  main.cpp
  )

# check for MYSQL
message(STATUS "CHECKING MYSQL ...")

execute_process(COMMAND mysql_config --variable=pkglibdir OUTPUT_VARIABLE MYSQL_LIB_PATH)
if(MYSQL_LIB_PATH)
#add include path
include_directories(../../FooSql/DbComm)
include_directories(../../FooSql/Exception)
include_directories(../../FooSql/Thread)
include_directories(../../FooSql/Tool)

#add lib path
#for the command "mysql_config --variable=pkglibdir" will give out an "\r\n" to the end,
#therefore, it is necessary to remove the last character
string(STRIP ${MYSQL_LIB_PATH} MYSQL_LIB_PATH_WITHOUT_NEWLINE)
link_directories(
  ${MYSQL_LIB_PATH_WITHOUT_NEWLINE}/mysql)

#to build
add_executable(DeleteSpecifiedTargetsTest ${base_SRCS})

#add link
target_link_libraries(
	DeleteSpecifiedTargetsTest 
	foosqldbcomm
	foosqlthread 
	foosqltool 
	foosqlexception
	mysqlclient
	pthread
	dl)

#enable macro MYSQL_ENV_AVAILABLE in the code
add_definitions(-DMYSQL_ENV_AVAILABLE)
	
message(STATUS "MYSQL INSTALLED, SUCCESSFULLY GENERATE MAKEFILE FOR ${CMAKE_CURRENT_SOURCE_DIR}")
	
else(MYSQL_LIB_PATH)

# refer to http://www.cmake.org/Wiki/CMake_Useful_Variables for more build-in variables
message(SEND_ERROR "MYSQL NOT INSTALLED, NOT ABLE TO GENERATE MAKEFILE FOR ${CMAKE_CURRENT_SOURCE_DIR}")

endif(MYSQL_LIB_PATH)