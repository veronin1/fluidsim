cmake_minimum_required(VERSION 3.10)
project(fluidsim VERSION 0.1 LANGUAGES CXX)

# c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# executable
add_executable(fluidsim
    src/main.cpp
    src/glad.c
)

# include directories
target_include_directories(fluidsim PRIVATE ${CMAKE_SOURCE_DIR}/include)

# compiler options
if(MSVC)
    target_compile_options(fluidsim PRIVATE /W4 /permissive-)
else()
    target_compile_options(fluidsim PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wformat=2
        -Wconversion
        -Wsign-conversion
        -Wfloat-equal
        -Wnull-dereference
        -Wdouble-promotion
        -Werror
        -fstack-protector-strong
        -D_FORTIFY_SOURCE=2
	-O1
    )
endif()

# debug build sanitizers
if(CMAKE_BUILD_TYPE MATCHES Debug)
    if(NOT MSVC)
        target_compile_options(fluidsim PRIVATE -fsanitize=address,undefined -O1)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
    endif()
endif()

# external libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(fluidsim
    PRIVATE OpenGL::GL
            glfw
            glm
)

# clang-tidy static analysis
set_target_properties(fluidsim PROPERTIES
    CXX_CLANG_TIDY "clang-tidy;-header-filter=${CMAKE_SOURCE_DIR}/src/.*"
)

# output directory
set_target_properties(fluidsim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
