add_subdirectory(parser)
add_subdirectory(ast)

get_filename_component(parent_dir ../ ABSOLUTE)
target_include_directories(parser INTERFACE ${parent_dir})
