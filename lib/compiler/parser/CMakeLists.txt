file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

find_package(magic_enum REQUIRED)
find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)

add_library(parser STATIC ${srcs})
target_link_libraries(parser PRIVATE magic_enum::magic_enum fmt::fmt
                                       range-v3::range-v3)
