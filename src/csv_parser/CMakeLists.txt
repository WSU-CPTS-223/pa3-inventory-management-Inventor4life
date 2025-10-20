cmake_minimum_required(VERSION 3.15)
project(csv_parser)

add_library(csv_parser STATIC src/csv_parser.cc include/csv_parser.h)
target_include_directories(csv_parser PUBLIC include)
