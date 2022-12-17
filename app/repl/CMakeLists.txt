file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

add_executable(repl ${srcs})
target_link_libraries(repl parser ast generator)
