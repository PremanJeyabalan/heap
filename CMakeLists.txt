cmake_minimum_required(VERSION 3.25)
project(heep
        VERSION 0.2
        DESCRIPTION "A static C++ library implementation of malloc and free"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_C_EXTENSIONS OFF)

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    include(CTest)
endif ()

include(FetchContent)

FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.2.1)
FetchContent_MakeAvailable(fmtlib)

add_subdirectory(src)

add_subdirectory(apps)

#add_subdirectory(benchmarks)

if ((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

