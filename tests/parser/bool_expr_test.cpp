#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/math_expr.h>

namespace xi
{

TEST_CASE("Parse boolean", "[Xi_Expr][Xi_Boolean]")
{
    // TODO(ding.wang): make this invalid
    REQUIRE_THAT(
        Xi_boolean("trueabc"), AstNodeMatcher(Xi_Boolean{true}, "abc")
    );

    REQUIRE_THAT(
        Xi_boolean("falseabc"), AstNodeMatcher(Xi_Boolean{false}, "abc")
    );
}

TEST_CASE("Parse basic bool expr", "[Xi_Boolean]")
{
    REQUIRE_THAT(
        Xi_expr("1 == 2"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Integer{1},
            .rhs = Xi_Integer{2},
            .op  = Xi_Op::Eq,
        })
    );

    REQUIRE_THAT(
        Xi_expr("1 != 2"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Integer{1},
            .rhs = Xi_Integer{2},
            .op  = Xi_Op::Neq,
        })
    );

    REQUIRE_THAT(
        Xi_expr("1 > 2"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Integer{1},
            .rhs = Xi_Integer{2},
            .op  = Xi_Op::Gt,
        })
    );

    REQUIRE_THAT(
        Xi_expr("1 >= 2"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Integer{1},
            .rhs = Xi_Integer{2},
            .op  = Xi_Op::Geq,
        })
    );

    REQUIRE_THAT(
        Xi_expr("1 < 2"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Integer{1},
            .rhs = Xi_Integer{2},
            .op  = Xi_Op::Lt,
        })
    );

    REQUIRE_THAT(
        Xi_expr("1 <= 2"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Integer{1},
            .rhs = Xi_Integer{2},
            .op  = Xi_Op::Leq,
        })
    );

    REQUIRE_THAT(
        Xi_expr("true && false"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Boolean{true},
            .rhs = Xi_Boolean{false},
            .op  = Xi_Op::And,
        })
    );

    REQUIRE_THAT(
        Xi_expr("true || false"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Boolean{true},
            .rhs = Xi_Boolean{false},
            .op  = Xi_Op::Or,
        })
    );
}

TEST_CASE("Parse composed boolexpr", "[Xi_Expr][Xi_Boolean]")
{
    REQUIRE_THAT(
        Xi_expr("true && false || true"),
        AstNodeMatcher(Xi_Binop{
            .lhs =
                Xi_Binop{
                    Xi_Boolean{true},
                    Xi_Boolean{false},
                    Xi_Op::And,
                },
            .rhs = Xi_Boolean{true},
            .op  = Xi_Op::Or,
        })
    );

    REQUIRE_THAT(
        Xi_expr("true || false && true"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Boolean{true},
            .rhs =
                Xi_Binop{
                    Xi_Boolean{false},
                    Xi_Boolean{true},
                    Xi_Op::And,
                },
            .op = Xi_Op::Or,
        })
    );

    REQUIRE_THAT(
        Xi_expr("true && false || true && false"),
        AstNodeMatcher(Xi_Binop{
            .lhs =
                Xi_Binop{
                    .lhs = Xi_Boolean{true},
                    .rhs = Xi_Boolean{false},
                    .op  = Xi_Op::And,
                },
            .rhs =
                Xi_Binop{
                    .lhs = Xi_Boolean{true},
                    .rhs = Xi_Boolean{false},
                    .op  = Xi_Op::And,
                },
            .op = Xi_Op::Or,
        })
    );

    // test paren
    REQUIRE_THAT(
        Xi_expr("(true || false) && true"),
        AstNodeMatcher(
            Xi_Binop{
                .lhs =
                    Xi_Binop{
                        .lhs = Xi_Boolean{true},
                        .rhs = Xi_Boolean{false},
                        .op  = Xi_Op::Or,
                    },
                .rhs = Xi_Boolean{true},
                .op  = Xi_Op::And,
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_expr("true && (false || true)"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Boolean{true},
            .rhs =
                Xi_Binop{
                    Xi_Boolean{false},
                    Xi_Boolean{true},
                    Xi_Op::Or,
                },
            .op = Xi_Op::And,
        })
    );

    // test mathbool
    REQUIRE_THAT(
        Xi_expr("1 + 2 > 3 * 4"),
        AstNodeMatcher(Xi_Binop{
            .lhs =
                Xi_Binop{
                    Xi_Integer{1},
                    Xi_Integer{2},
                    Xi_Op::Add,
                },
            .rhs =
                Xi_Binop{
                    Xi_Integer{3},
                    Xi_Integer{4},
                    Xi_Op::Mul,
                },
            .op = Xi_Op::Gt,
        })
    );

    REQUIRE_THAT(
        Xi_expr("(1 + (2 + 3) < 3 * 4) || true"),
        AstNodeMatcher(Xi_Binop{
            .lhs =
                Xi_Binop{
                    .lhs =
                        Xi_Binop{
                            .lhs = Xi_Integer{1},
                            .rhs =
                                Xi_Binop{
                                    .lhs = Xi_Integer{2},
                                    .rhs = Xi_Integer{3},
                                    .op  = Xi_Op::Add,
                                },
                            .op = Xi_Op::Add,
                        },
                    .rhs =
                        Xi_Binop{
                            .lhs = Xi_Integer{3},
                            .rhs = Xi_Integer{4},
                            .op  = Xi_Op::Mul,
                        },
                    .op = Xi_Op::Lt,
                },
            .rhs = Xi_Boolean{true},
            .op  = Xi_Op::Or,
        })
    );
}

} // namespace xi
