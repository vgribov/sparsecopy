cmake_minimum_required(VERSION 3.19)

project(sparsecopy VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -pedantic -Werror)
add_subdirectory(extern/libsys)

add_executable(sparsecopy main.cpp)
target_link_libraries(sparsecopy PUBLIC sys)
