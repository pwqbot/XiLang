file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

add_executable(main ${srcs})
target_link_libraries(main parser ast generator)
