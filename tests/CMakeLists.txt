find_package(Catch2 3 REQUIRED)

include(Catch)

add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test PRIVATE parser)
target_link_libraries(parser_test PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(parser_test)
