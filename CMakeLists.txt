cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)

project(CppDependencyAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# a garder ?
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	add_definitions("-fprofile-arcs")
	add_definitions("-ftest-coverage")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage  -lstdc++fs")
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) 
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" OFF)

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.14/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(SETTINGS cppstd=17)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	conan_cmake_run(REQUIRES boost/1.70.0@conan/stable BASIC_SETUP BUILD missing)
endif()
conan_cmake_run(REQUIRES Catch2/2.7.2@catchorg/stable BASIC_SETUP BUILD missing)

include_directories(3rd_party/GSL/include)
include_directories(3rd_party/expected/include)

add_subdirectory(lib)
add_subdirectory(unit_tests)
add_subdirectory(cppdependencyplotter)
