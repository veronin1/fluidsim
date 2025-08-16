cmake_minimum_required(VERSION 3.10)
project(fluidsim VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(fluidsim
    src/main.cpp
)

target_include_directories(fluidsim PRIVATE ${CMAKE_SOURCE_DIR}/include)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(fluidsim
    PRIVATE OpenGL::GL
            ${GLFW_LIBRARIES}
            GLEW::GLEW
            glm::glm
)

target_include_directories(fluidsim
    PRIVATE ${GLFW_INCLUDE_DIRS}
            ${GLEW_INCLUDE_DIRS}
            ${GLM_INCLUDE_DIRS}
)

if(MSVC)
    target_compile_options(fluidsim PRIVATE /W4 /permissive-)
else()
    target_compile_options(fluidsim PRIVATE -Wall -Wextra -pedantic)
endif()
