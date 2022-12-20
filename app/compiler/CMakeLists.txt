file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

find_package(gflags CONFIG REQUIRED)

add_executable(compiler ${srcs})
target_link_libraries(compiler parser ast generator gflags::gflags)
