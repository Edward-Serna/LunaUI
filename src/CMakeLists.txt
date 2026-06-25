add_executable(Simulator main.cpp)

target_include_directories(Simulator PRIVATE
    ${CMAKE_SOURCE_DIR}/core
)

target_link_libraries(Simulator PRIVATE
    core
)

if(WIN32)
    set_target_properties(Simulator PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()
