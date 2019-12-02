cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)

project(CppDependencyAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage" )
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) 
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # a garder ?
						  
include_directories(3rd_party/GSL/include)
include_directories(3rd_party/expected/include)

add_subdirectory(lib)
add_subdirectory(unit_tests)
add_subdirectory(cppdependencyplotter)
