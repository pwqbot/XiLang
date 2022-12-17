file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

add_executable(compiler ${srcs})
target_link_libraries(compiler parser ast generator)
