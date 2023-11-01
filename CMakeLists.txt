project (LinkedList)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/headers)

add_executable (${PROJECT_NAME})

add_compile_definitions (_SHOW_STACK_TRACE)

add_subdirectory (libs)
add_subdirectory (src)

target_link_libraries (${PROJECT_NAME} PRIVATE ColorConsole)
target_link_libraries (${PROJECT_NAME} PRIVATE CustomAssert)
