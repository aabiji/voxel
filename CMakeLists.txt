cmake_minimum_required(VERSION 3.17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT voxel)
project(${PROJECT})
include(FetchContent)

# glm
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

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
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp src/*.h)
add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT} PRIVATE glm::glm glfw glad)
target_include_directories(${PROJECT} PRIVATE ${PROJECT_SOURCE_DIR}/lib/stb)

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD_REQUIRED TRUE)
target_compile_options(${PROJECT} PRIVATE -Wall -Wextra -Wpedantic -Werror)
