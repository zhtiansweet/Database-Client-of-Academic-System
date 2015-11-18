cmake_minimum_required(VERSION 3.3)
project(database)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE)

include_directories(/usr/local/mysql/include)
link_directories(/usr/local/mysql/lib)
link_libraries(libmysqlclient.18.dylib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "~/Desktop")

set(SOURCE_FILES main.cpp utility.cpp utility.h login_information.cpp login_information.h quarter.cpp quarter.h quarter.cpp quarter.h)
add_executable(database ${SOURCE_FILES})