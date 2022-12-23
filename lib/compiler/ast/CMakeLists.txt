file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

find_package(magic_enum REQUIRED)
find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)

add_library(ast STATIC ${srcs})

target_link_libraries(ast PRIVATE magic_enum::magic_enum fmt::fmt
                      range-v3::range-v3)
