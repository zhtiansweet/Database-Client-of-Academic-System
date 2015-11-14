cmake_minimum_required(VERSION 3.3)
project(database)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/local/mysql/include)
link_directories(/usr/local/mysql/lib)
link_libraries(libmysqlclient.18.dylib)

set(SOURCE_FILES main.cpp utility.cpp utility.h)
add_executable(database ${SOURCE_FILES})