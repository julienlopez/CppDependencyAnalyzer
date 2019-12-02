include_directories(".")


file(GLOB_RECURSE SOURCE_FILES ./*)
add_library(lib ${SOURCE_FILES})

target_link_libraries(lib)
