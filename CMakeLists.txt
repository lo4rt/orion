cmake_minimum_required(VERSION 4.0)


project(OrionEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/$<IF:$<CONFIG:Debug>,debug,release>
)

if(MSVC)
    add_compile_options(/wd5244)
    add_compile_options(/utf-8)
endif()


# External libraries
add_subdirectory(external)

# Engine library
add_subdirectory(engine)

# Main applications utilizing the Orion game engine
add_subdirectory(editor)
add_subdirectory(player)

