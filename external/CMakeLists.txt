# external libraries

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(SDL)

add_subdirectory(volk)

add_subdirectory(spdlog)