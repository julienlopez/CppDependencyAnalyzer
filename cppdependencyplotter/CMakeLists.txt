include_directories(".")
include_directories("../lib")

file(GLOB_RECURSE SOURCE_FILES ./*)
add_executable(cpp_dependency_plotter ${SOURCE_FILES})
