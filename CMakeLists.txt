cmake_minimum_required(VERSION 3.15)

project(inventory_management)
add_executable(main src/base/main.cc
		src/base/product.h
		src/base/functions.cc
		src/base/header.h
		src/base/my_commands.h)

add_subdirectory(src/csv_parser)
target_link_libraries(main PUBLIC csv_parser)

add_subdirectory(src/hash_table)
target_link_libraries(main PUBLIC hash_table)
target_link_libraries(main PUBLIC hash_table_test)

add_subdirectory(src/repl_manager)
target_link_libraries(main PUBLIC repl_manager)
#target_link_libraries(main PUBLIC repl_command)

# Use C++11 standard
set_target_properties(main PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
)
