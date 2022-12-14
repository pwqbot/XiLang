find_package(Catch2 3 REQUIRED)
find_package(fmt REQUIRED)

include(Catch fmt::fmt)

include(catchtest)

catchtest("parser;generator")
