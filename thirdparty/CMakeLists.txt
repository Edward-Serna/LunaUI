#####################################################
# SDL3
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SDL/CMakeLists.txt")
    add_subdirectory(SDL EXCLUDE_FROM_ALL)
else()
    message(FATAL_ERROR "SDL submodule not found. Run: git submodule update --init --recursive")
endif()
#####################################################
# Assimp (with minimal features for OBJ loading)
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/assimp/CMakeLists.txt")
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_FBX_IMPORTER OFF CACHE BOOL "" FORCE)
    set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
    add_subdirectory(assimp EXCLUDE_FROM_ALL)
else()
    message(WARNING "Assimp submodule not found. OBJ loading will be unavailable.
                    Run: git submodule update --init --recursive")
endif()
#####################################################
# Check for GLAD (required for OpenGL loading)
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/glad/include/glad/glad.h")
    message(FATAL_ERROR "GLAD not found. Run: git submodule update --init --recursive")
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/fmt/CMakeLists.txt")
    add_subdirectory(fmt EXCLUDE_FROM_ALL)
endif()

