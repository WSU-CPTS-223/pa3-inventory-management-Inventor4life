cmake_minimum_required(VERSION 3.15)
project(ReplManager)

add_library(repl_manager STATIC include/repl_manager.h include/repl_command.h src/repl_manager.cc src/repl_command.cc
        src/repl_manager_i.h)
target_include_directories(repl_manager PUBLIC include)

#set(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})
#add_subdirectory(../hash_table)
target_link_libraries(repl_manager PRIVATE hash_table)
