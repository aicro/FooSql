set(base_SRCS
  DllManager.cpp
  IniParser.cpp
  md5.cpp
  StringHelper.cpp
  TypeCheck.cpp
  )
  
add_library(foosqltool SHARED ${base_SRCS})

# install headers to ${CMAKE_INSTALL_PREFIX}/<DESTINATION>
install(DIRECTORY ${PROJECT_SOURCE_DIR}/FooSql/Tool/tool DESTINATION include/FooSql)

# install lib to ${CMAKE_INSTALL_PREFIX}/<DESTINATION>
install(TARGETS foosqltool DESTINATION lib/FooSql/lib)
