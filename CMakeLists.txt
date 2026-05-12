cmake_minimum_required(VERSION 3.24)
project(OrionEngine)

set(CMAKE_CXX_STANDARD 20)

# Добавление SDL в проект
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(external/SDL)

# Добавление volk в проект
add_subdirectory(external/volk)

add_subdirectory(src)

