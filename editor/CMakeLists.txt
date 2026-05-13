add_executable(orion-editor
        main.cpp
)
target_link_libraries(orion-editor
    PRIVATE
        orion-engine
)