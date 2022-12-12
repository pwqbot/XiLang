add_subdirectory(parser)
add_subdirectory(ast)

get_filename_component(parent_dir ../ ABSOLUTE)
target_include_directories(parser PUBLIC ${parent_dir})
