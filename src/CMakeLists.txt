# Main Application
add_executable(Simulator
    main.cpp
)

target_include_directories(Simulator PRIVATE
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/loaders
)

target_link_libraries(Simulator PRIVATE
    core
    loaders
)

if(WIN32)
    set_target_properties(Simulator PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()
