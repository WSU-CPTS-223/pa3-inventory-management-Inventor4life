cmake_minimum_required(VERSION 3.15)
project(HashTableProject)

add_library(hash_table INTERFACE include/hash_table.h) # interface because there are no source files
target_include_directories(hash_table INTERFACE include)

add_library(hash_table_test STATIC tests/include/hash_table_test.h tests/src/hash_table_test.cc)
target_include_directories(hash_table_test PUBLIC tests/include)
target_link_libraries(hash_table_test PRIVATE hash_table)
#set_target_properties(hash_table_test PROPERTIES LINKER_LANGUAGE CXX)
