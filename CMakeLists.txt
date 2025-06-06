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

set(SHADERS_DIR "shaders")
add_custom_command(TARGET lizual POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
        copy_directory_if_different
        ${CMAKE_SOURCE_DIR}/src/${SHADERS_DIR}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${SHADERS_DIR}
    COMMENT "Copying shaders to output directory")
