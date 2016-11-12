cmake_minimum_required(VERSION 3.6)
project(hungryCrows)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

set(SOURCE_FILES main.cpp)
add_executable(hungryCrows ${SOURCE_FILES})
find_package( Threads REQUIRED )
target_link_libraries( hungryCrows Threads::Threads )
