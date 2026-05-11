cmake_minimum_required(VERSION 3.24)
project(OrionEngine)

set(CMAKE_CXX_STANDARD 20)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

add_subdirectory(external/SDL)

add_executable(OrionEngine main.cpp)
target_link_libraries(OrionEngine PRIVATE SDL3::SDL3)