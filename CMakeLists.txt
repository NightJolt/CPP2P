cmake_minimum_required(VERSION 3.16)
project(CPP2P)

find_package(Lua REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lm -lGL -L/usr/X11R6/lib -lGLU -lGLEW -lXi -lXmu")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(header-files)
include_directories(source-files)
include_directories(imgui)

file (GLOB_RECURSE SOURCE_FILES
        header-files/*.h
        source-files/*.cpp
        imgui/*.h
        imgui/*.cpp
)

add_executable(CPP2P ${SOURCE_FILES})

target_link_libraries(CPP2P ${LUA_LIBRARIES})
