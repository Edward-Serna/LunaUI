add_library(loaders STATIC
    asset_loader.h
    asset_loader.cpp
    obj_loader.h
    obj_loader.cpp
)

target_include_directories(loaders PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/thirdparty/glm
    ${CMAKE_SOURCE_DIR}/thirdparty/assimp/include
)

# Link to core library
target_link_libraries(loaders PUBLIC
    core
    assimp
)
