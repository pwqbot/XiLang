file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)
add_library(parser INTERFACE ${srcs})
get_filename_component(parent_dir ../ ABSOLUTE)
target_include_directories(parser INTERFACE ${parent_dir})
