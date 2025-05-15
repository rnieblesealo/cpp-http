cmake_minimum_required(VERSION 3.10)

# Set the project name and language
project(HttpServer LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add executables 
# NOTE: we're statically linking helper into each executable
# for larger projects, consider dynamic linking :)
add_executable(AppServer
  src/socket/server.cpp
  src/socket/helper.cpp
)

add_executable(AppClient
  src/socket/client.cpp
  src/socket/helper.cpp
)

# tell cmake where to look for header files
target_include_directories(AppServer PRIVATE src/socket)
target_include_directories(AppClient PRIVATE src/socket)
