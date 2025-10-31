cmake_minimum_required(VERSION 3.17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT voxel)
project(${PROJECT} LANGUAGES C)
include(FetchContent)

# glfw
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG master
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# glad
add_library(glad ${PROJECT_SOURCE_DIR}/lib/glad/src/glad.c)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/lib/glad/include)

# build
add_executable(
    ${PROJECT}
    src/main.c
)

target_link_libraries(${PROJECT} PRIVATE glfw glad)
target_include_directories(${PROJECT} PRIVATE ${PROJECT_SOURCE_DIR}/lib/stb)

target_compile_options(${PROJECT} PRIVATE -Wall -Wextra -Wpedantic -Werror)

# copy assets
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
