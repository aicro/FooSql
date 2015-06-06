set(base_SRCS
  ThrowableException.cpp
  )
  
add_library(foosqlexception SHARED ${base_SRCS})

# install headers to ${CMAKE_INSTALL_PREFIX}/<DESTINATION>
install(DIRECTORY ${PROJECT_SOURCE_DIR}/FooSql/Exception/exception DESTINATION include/FooSql)

# install lib to ${CMAKE_INSTALL_PREFIX}/<DESTINATION>
install(TARGETS foosqlexception DESTINATION lib/FooSql/lib)