file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

find_package(magic_enum REQUIRED)

add_library(parser INTERFACE ${srcs})
get_filename_component(parent_dir ../ ABSOLUTE)
# link boost variant
target_link_libraries(parser INTERFACE magic_enum::magic_enum)

target_include_directories(parser INTERFACE ${parent_dir})
