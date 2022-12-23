file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.h)

find_package(tl-expected CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_library(utils INTERFACE ${srcs})
target_link_libraries(parser INTERFACE tl::expected fmt::fmt)
