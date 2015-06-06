set(base_SRCS
  Condition.cpp
  Mutex.cpp
  MutexLockGuard.cpp
  Thread.cpp
  ThreadPool.cpp
  )

add_library(foosqlthread SHARED ${base_SRCS})

# install headers to ${CMAKE_INSTALL_PREFIX}/<DESTINATION>
install(DIRECTORY ${PROJECT_SOURCE_DIR}/FooSql/Thread/thread DESTINATION include/FooSql)

# install lib to ${CMAKE_INSTALL_PREFIX}/<DESTINATION>
install(TARGETS foosqlthread DESTINATION lib/FooSql/lib)
