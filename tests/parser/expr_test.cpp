#include "test_header.h"

#include <compiler/parser/expr.h>
#include <iostream>

namespace xi
{

TEST_CASE("Parse expr", "[Xi_Expr]")
{
    REQUIRE_THAT(Xi_expr("123 abc"), AstNodeMatcher(Xi_Integer{123}, " abc"));

    REQUIRE_THAT(
        Xi_expr("123.456 abc"), AstNodeMatcher(Xi_Real{123.456}, " abc")
    );

    REQUIRE_THAT(Xi_expr("true abc"), AstNodeMatcher(Xi_Boolean{true}, " abc"));

    REQUIRE_THAT(
        Xi_expr("\"abc\"abc"), AstNodeMatcher(Xi_String{"abc"}, "abc")
    );
}

} // namespace xi
