cmake_minimum_required(VERSION 3.16)

project(lizual VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SDL EXCLUDE_FROM_ALL)

add_executable(lizual
    src/main.cpp
    src/Shader.cpp
    src/Shader.h
)

# Link to the actual SDL3 library.
target_link_libraries(lizual PRIVATE SDL3::SDL3)

# Include GLAD
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLAD EXCLUDE_FROM_ALL)
target_link_libraries(lizual PRIVATE glad)

# Link to stb_image
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb_image EXCLUDE_FROM_ALL)
target_link_libraries(lizual PRIVATE stb_image)

# Set the assets directory as a compile definition.
target_compile_definitions(lizual PRIVATE LIZUAL_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

# TODO: add installation logic to copy assets
