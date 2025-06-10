# This file is necessary because glm by default sets its target_include_directories to
# "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>", which would be the root of lizual. This is bad
# because the glm folder isn't actually there, and even if it were, it opens up the entire
# project root to being included.
project(glm)
add_subdirectory(glm)
