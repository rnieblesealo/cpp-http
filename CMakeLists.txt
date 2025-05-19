# set cmake version
cmake_minimum_required(VERSION 3.10)

# set the project name and language
project(HttpServer LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# export compile commands
# NOTE: need this for clangd to find stuff!
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add fmt
include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)

# add executables 
# NOTE: we're statically linking helper into each executable
# for larger projects, consider dynamic linking :)
add_executable(AppServer
  src/chat/server.cpp
  src/helper/helper.cpp
)

add_executable(AppClient
  src/chat/client.cpp
  src/helper/helper.cpp
)

# tell cmake where to look for header files
target_include_directories(AppServer PRIVATE 
  src/chat 
  src/helper
)

target_include_directories(AppClient PRIVATE 
  src/chat 
  src/helper
)

# link fmt
target_link_libraries(AppServer PRIVATE fmt::fmt)
target_link_libraries(AppClient PRIVATE fmt::fmt)
