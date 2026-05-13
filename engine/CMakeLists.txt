add_library(orion-engine)
target_link_libraries(orion-engine
    PRIVATE
        SDL3::SDL3
        volk::volk
)
target_sources(orion-engine
    PUBLIC
        FILE_SET CXX_MODULES FILES
            engine.cppm
    PRIVATE
        engine.cpp
)