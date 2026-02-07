cmake_minimum_required(VERSION 3.20)
project(Simulator VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

message(STATUS "Building third-party libraries...")

add_subdirectory(thirdparty)

#####################################################
# Copy the assets into the bin within assets/
file(GLOB SCENE_FILES "${CMAKE_SOURCE_DIR}/scenes/*")
if(SCENE_FILES)
    file(COPY ${SCENE_FILES} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scenes")
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    file(GLOB ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")
    if(ASSET_FILES)
        file(COPY ${ASSET_FILES} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
    endif()
endif()

add_subdirectory(core)
add_subdirectory(loaders)
add_subdirectory(src)

message(STATUS "Build configuration complete! Run 'cmake --build .' to compile.")
