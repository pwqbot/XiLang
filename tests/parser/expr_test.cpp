#include "compiler/ast/ast.h"
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

    REQUIRE_THAT(
        Xi_expr("a[0]"),
        AstNodeMatcher(
            Xi_ArrayIndex{
                .array_var_name = "a",
                .index          = Xi_Integer{0},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_expr("a = 1"),
        AstNodeMatcher(Xi_Assign{
            .name = "a",
            .expr =
                Xi_Expr{
                    Xi_Integer{1},
                },
        })
    );

    REQUIRE_THAT(
        Xi_expr("a += 1"),
        AstNodeMatcher(Xi_Assign{
            .name = "a",
            .expr =
                Xi_Expr{
                    Xi_Binop{
                        .lhs = Xi_Iden{.name = "a", .expr = std::monostate{}},
                        .rhs = Xi_Integer{1},
                        .op  = Xi_Op::Add,
                    },
                },
        })
    );

    REQUIRE_THAT(
        Xi_expr("a++"),
        AstNodeMatcher(Xi_Assign{
            .name = "a",
            .expr =
                Xi_Expr{
                    Xi_Binop{
                        .lhs = Xi_Iden{.name = "a", .expr = std::monostate{}},
                        .rhs = Xi_Integer{1},
                        .op  = Xi_Op::Add,
                    },
                },
        })
    );
}

} // namespace xi
