include_directories(".")
include_directories("../lib")

include_directories(../3rd_party/Catch2/single_include)

file(GLOB_RECURSE SOURCE_FILES ./*)
add_executable(unit_tests ${SOURCE_FILES})

target_link_libraries(unit_tests lib)
