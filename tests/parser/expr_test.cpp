#include "test_header.h"

#include <compiler/parser/expr.h>

namespace xi
{

TEST_CASE("Parse expr", "[Xi_Expr]")
{
    auto [integer1, integer2] = Xi_expr("123abc").value();
    REQUIRE(integer1 == Xi_Integer{123});
    REQUIRE(integer2 == "abc");

    auto [real1, real2] = Xi_expr("123.456abc").value();
    REQUIRE(real1 == Xi_Real{123.456});
    REQUIRE(real2 == "abc");

    auto [boolean1, boolean2] = Xi_expr("trueabc").value();
    REQUIRE(boolean1 == Xi_Boolean{true});
    REQUIRE(boolean2 == "abc");

    auto [string1, string2] = Xi_expr("\"abc\"abc").value();
    REQUIRE(string1 == Xi_String{"abc"});
}

} // namespace xi
