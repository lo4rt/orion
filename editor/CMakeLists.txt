add_executable(orion-editor)

target_sources(orion-editor
    PUBLIC FILE_SET CXX_MODULES FILES
        editor.cppm
    PUBLIC
        main.cpp
)

target_link_libraries(orion-editor
    PRIVATE
        orion-engine
        SDL3::SDL3
)