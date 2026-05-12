add_executable(OrionEngine main.cpp)
target_link_libraries(OrionEngine
        PRIVATE
        SDL3::SDL3
        volk::volk
)