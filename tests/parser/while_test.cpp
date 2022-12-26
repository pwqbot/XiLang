#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/program.h>

namespace xi
{

TEST_CASE("Parse While")
{
    REQUIRE_THAT(
        Xi_while("while (true) {}"),
        AstNodeMatcher(Xi_Stmt{
            Xi_While{
                .cond = Xi_Boolean{true},
                .body = {},
            },
        })
    );

    REQUIRE_THAT(
        Xi_while("while (true) { 1; }"),
        AstNodeMatcher(Xi_While{
            .cond = Xi_Boolean{true},
            .body =
                {
                    Xi_Expr{
                        Xi_Integer{1},
                    },
                },
        })
    );

    REQUIRE_THAT(
        Xi_while("while (1 + 1 < 2) { 1; 2; }"),
        AstNodeMatcher(Xi_While{
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
            .body =
                {
                    Xi_Expr{
                        Xi_Integer{1},
                    },
                    Xi_Expr{
                        Xi_Integer{2},
                    },
                },
        })
    );
}

TEST_CASE("Parse for")
{
    REQUIRE_THAT(
        Xi_for("for i; i < 2; i + 1 {}"),
        AstNodeMatcher(Xi_Stmts{
            .stmts =
                {
                    Xi_Iden{
                        .name = "i",
                        .expr = std::monostate{},
                    },
                    Xi_While{
                        .cond =
                            Xi_Binop{
                                .lhs =
                                    Xi_Iden{
                                        .name = "i", .expr = std::monostate{}},
                                .rhs = Xi_Integer{2},
                                .op  = Xi_Op::Lt,
                            },
                        .body =
                            {
                                Xi_Binop{
                                    .lhs =
                                        Xi_Iden{
                                            .name = "i",
                                            .expr = std::monostate{},
                                        },
                                    .rhs = Xi_Integer{1},
                                    .op  = Xi_Op::Add,
                                },
                            },
                    },
                },
        })
    );
}

} // namespace xi
