cmake_minimum_required(VERSION 3.16)

project(lizual VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_executable(lizual
src/main.cpp
src/Shader.cpp
src/Shader.h
src/Camera.cpp
src/Camera.h
)

# ----- Dependencies -----
# ---- SDL3 ----
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SDL EXCLUDE_FROM_ALL)
# Link to the actual SDL3 library.
target_link_libraries(lizual PRIVATE SDL3::SDL3)

# ---- GLAD ----
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLAD EXCLUDE_FROM_ALL)
target_link_libraries(lizual PRIVATE glad)

# ---- stb_image ----
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb_image EXCLUDE_FROM_ALL)
target_link_libraries(lizual PRIVATE stb_image)

# ---- glm ----
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm EXCLUDE_FROM_ALL)
target_link_libraries(lizual PRIVATE glm::glm)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# ---- imgui ----
set(IMGUI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui")
target_sources(lizual PRIVATE
    # Normal sources
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    # Backends. We use SDL3 and OpenGL.
    ${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_ROOT}/backends/imgui_impl_sdl3.cpp
    # We need std::string until we decide not to use it
    ${IMGUI_ROOT}/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(lizual PRIVATE
    ${IMGUI_ROOT}
    ${IMGUI_ROOT}/backends
    ${IMGUI_ROOT}/misc/cpp
)

# ----- Assets -----
# Set the assets directory as a compile definition.
target_compile_definitions(lizual PRIVATE LIZUAL_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

# TODO: add installation logic to copy assets
