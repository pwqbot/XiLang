#include "compiler/parser/program.h"
#include "test_header.h"

namespace xi
{

TEST_CASE("Parse if stmt")
{
    REQUIRE_THAT(
        Xi_if_stmt("if (true) {}"),
        AstNodeMatcher(Xi_Stmt{
            Xi_If_stmt{
                .cond = Xi_Boolean{true},
                .then = {},
                .els  = {},
            },
        })
    );

    REQUIRE_THAT(
        Xi_if_stmt("if (true) { 1; }"),
        AstNodeMatcher(Xi_If_stmt{
            .cond = Xi_Boolean{true},
            .then =
                {
                    Xi_Expr{
                        Xi_Integer{1},
                    },
                },
            .els = {},
        })
    );

    REQUIRE_THAT(
        Xi_if_stmt("if (true) 1;"),
        AstNodeMatcher(Xi_If_stmt{
            .cond = Xi_Boolean{true},
            .then =
                {
                    Xi_Expr{
                        Xi_Integer{1},
                    },
                },
            .els = {},
        })
    );

    REQUIRE_THAT(
        Xi_if_stmt("if (a < 2) { a = b; }"),
        AstNodeMatcher(Xi_If_stmt{
            .cond =
                Xi_Binop{
                    .lhs = Xi_Iden{.name = "a", .expr = std::monostate{}},
                    .rhs = Xi_Integer{2},
                    .op  = Xi_Op::Lt,
                },
            .then =
                {
                    Xi_Expr{
                        Xi_Assign{
                            .name = "a",
                            .expr =
                                Xi_Iden{
                                    .name = "b",
                                    .expr = std::monostate{},
                                },
                        },
                    },
                },
            .els = {},
        })
    );

    REQUIRE_THAT(
        Xi_if_stmt("if (1 + 1 < 2) { 1; 2; }"),
        AstNodeMatcher(Xi_If_stmt{
            .cond =
                Xi_Binop{
                    .lhs =
                        Xi_Binop{
                            .lhs = Xi_Integer{1},
                            .rhs = Xi_Integer{1},
                            .op  = Xi_Op::Add,
                        },
                    .rhs = Xi_Integer{2},
                    .op  = Xi_Op::Lt,
                },
            .then =
                {
                    Xi_Expr{
                        Xi_Integer{1},
                    },
                    Xi_Expr{
                        Xi_Integer{2},
                    },
                },
            .els = {},
        })
    );

    REQUIRE_THAT(
        Xi_if_stmt("if (1 + 1 < 2) { 1; 2; } else { 3; 4; }"),
        AstNodeMatcher(Xi_If_stmt{
            .cond =
                Xi_Binop{
                    .lhs =
                        Xi_Binop{
                            .lhs = Xi_Integer{1},
                            .rhs = Xi_Integer{1},
                            .op  = Xi_Op::Add,
                        },
                    .rhs = Xi_Integer{2},
                    .op  = Xi_Op::Lt,
                },
            .then =
                {
                    Xi_Expr{
                        Xi_Integer{1},
                    },
                    Xi_Expr{
                        Xi_Integer{2},
                    },
                },
            .els =
                {
                    Xi_Expr{
                        Xi_Integer{3},
                    },
                    Xi_Expr{
                        Xi_Integer{4},
                    },
                },
        })
    );
}

} // namespace xi
