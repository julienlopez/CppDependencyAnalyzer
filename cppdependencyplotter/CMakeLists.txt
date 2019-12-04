include_directories(".")
include_directories("../lib")

file(GLOB_RECURSE SOURCE_FILES ./*)
add_executable(cpp_dependency_plotter ${SOURCE_FILES})

target_link_libraries(cpp_dependency_plotter lib)
