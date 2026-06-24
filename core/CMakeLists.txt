add_library(core STATIC
        math_utils.h
        console.h
        scene_node.h
        scene_serializer.h
        renderer.h
        renderer.cpp
        shaders/shader.h
        shaders/shader.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    ${CMAKE_SOURCE_DIR}/loaders
    ${CMAKE_SOURCE_DIR}/thirdparty/SDL/include
    ${CMAKE_SOURCE_DIR}/thirdparty/glad/include
    ${CMAKE_SOURCE_DIR}/thirdparty/glm
    ${CMAKE_SOURCE_DIR}/thirdparty/json/include
    ${CMAKE_SOURCE_DIR}/thirdparty/fmt/include
)

# Link SDL3
if (TARGET SDL3::SDL3)
    target_link_libraries(core PUBLIC SDL3::SDL3)
elseif (TARGET SDL3)
    target_link_libraries(core PUBLIC SDL3)
else()
    message(FATAL_ERROR "SDL3 target not found")
endif()

# Link fmt
target_link_libraries(core PUBLIC fmt::fmt)

# Link OpenGL
if (WIN32)
    target_link_libraries(core PUBLIC opengl32)
elseif (APPLE)
    target_link_libraries(core PUBLIC "-framework OpenGL")
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(core PUBLIC OpenGL::GL)
endif()

# Add GLAD source
target_sources(core PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/glad/src/glad.c
)