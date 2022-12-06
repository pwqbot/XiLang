#include <catch2/catch_test_macros.hpp>
#include <compiler/ast.h>
#include <compiler/ast_format.h>
#include <compiler/utils.h>

namespace xi {

// NOLINTBEGIN(cppcoreguidelines-*, readability*)
TEST_CASE("Test Xi_Integer", "[Xi_Integer]") {
    auto result = Xi_Integer{123};
    REQUIRE(fmt::format("{}", result) == "Xi_Integer 123");

    auto result2 = Xi_Integer{-123};
    REQUIRE(fmt::format("{}", result2) == "Xi_Integer -123");
}

TEST_CASE("Test Xi_Real", "[Xi_Real]") {
    auto result = Xi_Real{123.456};
    REQUIRE(fmt::format("{}", result) == "Xi_Real 123.456");
}

TEST_CASE("Test Xi_Boolean", "[Xi_Boolean]") {
    auto result = Xi_Boolean{true};
    REQUIRE(fmt::format("{}", result) == "Xi_Boolean true");

    auto result2 = Xi_Boolean{false};
    REQUIRE(fmt::format("{}", result2) == "Xi_Boolean false");
}

TEST_CASE("Test Xi_String", "[Xi_String]") {
    auto result = Xi_String{"hello"};
    REQUIRE(fmt::format("{}", result) == "Xi_String \"hello\"");
}

TEST_CASE("Test recursive_wrapper", "[recursive_wrapper]") {
    auto result = recursive_wrapper{Xi_Integer{123}};
    REQUIRE(fmt::format("{}", result) == "Xi_Integer 123");
}

TEST_CASE("Test Xi_Binop", "[Xi_Binop]") {
    auto result = Xi_Binop{Xi_Integer{123}, Xi_Integer{456}, Xi_Op::Add};
    REQUIRE(fmt::format("{}", result) == "Xi_Binop +\n"
                                         "\tXi_Integer 123\n"
                                         "\tXi_Integer 456");
}
// NOLINTEND(cppcoreguidelines-*, readability*)

} // namespace xi
